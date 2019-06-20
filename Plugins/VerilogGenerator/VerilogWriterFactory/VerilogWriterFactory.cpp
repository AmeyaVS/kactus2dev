//-----------------------------------------------------------------------------
// File: VerilogWriterFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Creates writes for generating Verilog.
//-----------------------------------------------------------------------------

#include "VerilogWriterFactory.h"

#include <common/ui/MessageMediator.h>

#include <library/LibraryInterface.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentRegisterVerilogWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>
#include <Plugins/VerilogGenerator/VerilogAssignmentWriter/VerilogAssignmentWriter.h>
#include <Plugins/VerilogGenerator/VerilogPortWriter/VerilogTopDefaultWriter.h>
#include <Plugins/VerilogGenerator/VerilogWriterFactory/VerilogRegisterDocument.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::VerilogWriterFactory()
//-----------------------------------------------------------------------------
VerilogWriterFactory::VerilogWriterFactory(LibraryInterface* library,
    MessageMediator* messages, GenerationSettings* settings,
    QString const& kactusVersion, QString const& generatorVersion) :
    library_(library),
    messages_(messages),
    settings_(settings),
    generatorVersion_(generatorVersion),
    kactusVersion_(kactusVersion),
    sorter_(new InterfaceDirectionNameSorter())
{

}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareComponent()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationOutput> VerilogWriterFactory::prepareComponent(QString const& outputPath,
    QSharedPointer<MetaComponent> component)
{
    // If we are not generating based on a design, we must parse the existing implementation.
    QString fileName = component->getModuleName() + ".v";
    QString filePath = outputPath + "/" + fileName;

    QSharedPointer<VerilogDocument> document = QSharedPointer<VerilogDocument>(new VerilogDocument);

    document->setFileName(fileName);
    document->setVLNV(component->getComponent()->getVlnv().toString());

    // Try to find an implementation from the file.
    if (!readImplementation(document, filePath))
    {
        return QSharedPointer<VerilogDocument>();
    }

    // Create writers for module header etc.
    initializeComponentWriters(document, component);

    // Create writers.
    QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(document->getImplementation()));
    document->getTopWriter()->setImplementation(implementationWriter);

    QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(document->getPostModule()));
    document->getTopWriter()->setPostModule(postModuleWriter);

    return document;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareRegisterDocument()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationOutput> VerilogWriterFactory::prepareRegisterDocument(QString const& outputPath,
    QSharedPointer<MetaComponent> component)
{
    if (component->getRegisters()->isEmpty())
    {
        return QSharedPointer<GenerationOutput>();
    }

    QString moduleName = VerilogSyntax::legalizeName(component->getModuleName()) + QLatin1String("_registers");
    QString fileName = moduleName + QLatin1String(".v");
    QString filePath = outputPath + "/" + fileName;

    QSharedPointer<VerilogRegisterDocument> registerDoc(new VerilogRegisterDocument());
    registerDoc->setFileName(fileName);
    registerDoc->setVLNV(component->getComponent()->getVlnv().toString());

    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(component->getComponent()->getVlnv());
    QString documentDescription = QLatin1String("Register file");

    QSharedPointer<VerilogHeaderWriter> newHeaderWriter(new VerilogHeaderWriter(
        component->getComponent()->getVlnv(), componentXmlPath, currentUser,
        documentDescription, kactusVersion_, generatorVersion_));
    QSharedPointer<ComponentRegisterVerilogWriter> newRegisterWriter(
        new ComponentRegisterVerilogWriter(component, moduleName));

    registerDoc->setHeaderWriter(newHeaderWriter);
    registerDoc->setRegisterWriter(newRegisterWriter);

    return registerDoc;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::prepareDesign()
//-----------------------------------------------------------------------------
QList<QSharedPointer<GenerationOutput> > VerilogWriterFactory::prepareDesign(QList<QSharedPointer<MetaDesign> >& designs)
{
    QList<QSharedPointer<GenerationOutput> > retval;

    foreach(QSharedPointer<MetaDesign> mDesign, designs)
    {
        QSharedPointer<VerilogDocument> document = QSharedPointer<VerilogDocument>(new VerilogDocument);
        initializeComponentWriters(document, mDesign->getTopInstance());
        document->setFileName(mDesign->getTopInstance()->getModuleName() + ".v");
        document->setVLNV(mDesign->getTopInstance()->getComponent()->getVlnv().toString());

        initializeDesignWriters(document, mDesign);

        // Finally, add them to the top writer in desired order.
        addWritersToTopInDesiredOrder(document);
        retval.append(document);
    }

    return retval;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getLanguage()
//-----------------------------------------------------------------------------
QString VerilogWriterFactory::getLanguage() const
{
    return "Verilog";
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getSaveToFileset()
//-----------------------------------------------------------------------------
bool VerilogWriterFactory::getSaveToFileset() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::getGroupIdentifier()
//-----------------------------------------------------------------------------
QString VerilogWriterFactory::getGroupIdentifier() const
{
    return QStringLiteral("generatedFiles");
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::readImplementation()
//-----------------------------------------------------------------------------
bool VerilogWriterFactory::readImplementation(QSharedPointer<VerilogDocument> document, QString const& outputPath)
{
    // This will contain the found implementation, if success.
    QString implementation;
    // This may contain the text after the module definition, if success.
    QString postModule;
    // This will contain the error message, if not success.
    QString error;

    // Check if the output file already exists.
    QFile outputFile(outputPath); 

    // If it does not, there is nothing do here.
    if (!outputFile.exists())
    {
        return true;
    }

    // Must be able to open it for reading.
    if (!outputFile.open(QIODevice::ReadOnly))
    {
        messages_->showError(
            QObject::tr("File %1: The output file exists, but could not open it for reading. The path: %2")
            .arg(document->getFileName(), outputPath));
        return false;
    }

    // Read the content.
    QTextStream inputStream(&outputFile);
    QString fileContent = inputStream.readAll();
    // Remove carriage return.
    fileContent.remove('\r');

    // The document sub class is supposed to know how to find its implementation.
    if (!document->selectImplementation(fileContent, implementation, postModule, error))
    {
        messages_->showError(QObject::tr("File %1: %2").arg(document->getFileName(), error));

        return false;
    }

    document->setImplementation(implementation);
    document->setPostModule(postModule);

    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeComponentWriters()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::initializeComponentWriters(QSharedPointer<VerilogDocument> document,
    QSharedPointer<MetaComponent> component)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(component->getComponent()->getVlnv());

    QSharedPointer<VerilogHeaderWriter> newHeaderWriter(new VerilogHeaderWriter(
        component->getComponent()->getVlnv(), componentXmlPath, currentUser,
        component->getComponent()->getDescription(), kactusVersion_, generatorVersion_));
    document->setHeaderWriter(newHeaderWriter);

    QSharedPointer<ComponentVerilogWriter> newTopWriter(new ComponentVerilogWriter(
        component, settings_->generateInterfaces_));
    document->setTopWriter(newTopWriter);
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::initializeDesignWriters()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::initializeDesignWriters(QSharedPointer<VerilogDocument> document,
    QSharedPointer<MetaDesign> design)
{
    document->initializeDesignWriters();

    // Comment for top for assignments.
    if (design->getTopInstance()->getPorts()->size() > 0)
    {
        QSharedPointer<CommentWriter> topAssignmentHeaderWriter(
            new CommentWriter("Assignments for the ports of the encompassing component:"));
        topAssignmentHeaderWriter->setIndent(4);
        document->getTopAssignmentWriters()->add(topAssignmentHeaderWriter);
    }

    // Create assignments fort top ports.
    foreach (QSharedPointer<MetaPort> mPort, *design->getTopInstance()->getPorts())
    {
        if (mPort->downAssignments_.size() > 0)
        {
            // Create a writer for each assignment of the port.
            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->downAssignments_)
            {
                // Current policy dictates that hierarchical in-out ports are directly connected to an instance port.
                // As an exception, it could have a default value assignment.
                if (mPort->port_->getDirection() != DirectionTypes::INOUT || !mpa->wire_)
                {
                    QSharedPointer<VerilogAssignmentWriter> topAssignment = QSharedPointer<VerilogAssignmentWriter>
                        (new VerilogAssignmentWriter(mPort->port_->name(), mpa, mPort, true));
                    document->getTopAssignmentWriters()->add(topAssignment);
                }
            }
        }
        else
        {
            QSharedPointer<VerilogTopDefaultWriter> topDefault = QSharedPointer<VerilogTopDefaultWriter>
                (new VerilogTopDefaultWriter(mPort));
            document->getTopDefaultWriters()->add(topDefault);
        }
    }

    // Create instance writers for the instances.
    foreach(QSharedPointer<MetaInstance> mInstance, *design->getInstances())
    {
        QSharedPointer<ComponentInstance> instance = mInstance->getComponentInstance();

        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
            mInstance, sorter_, settings_->generateInterfaces_));

        document->addInstanceWriter(instanceWriter);
        document->addInstanceHeaderWriter(instanceWriter, createHeaderWriterForInstance(mInstance));

        // Comment for instance assignments.
        if (mInstance->getPorts()->size() > 0)
        {
            QSharedPointer<CommentWriter> portWireHeaderWriter(
                new CommentWriter(instance->getInstanceName() + " port wires:"));
            portWireHeaderWriter->setIndent(4);
            document->getPortWireWriters()->add(portWireHeaderWriter);

            QSharedPointer<CommentWriter> assignmentHeaderWriter(
                new CommentWriter(instance->getInstanceName() + " assignments:"));
            assignmentHeaderWriter->setIndent(4);
            document->getInstanceAssignmentWriters()->add(assignmentHeaderWriter);
        }

        // Create writers for instance ports, wires, and assignments.
        foreach (QSharedPointer<MetaPort> mPort, *mInstance->getPorts())
        {
            if (mPort->upAssignments_.size() < 1)
            {
                continue;
            }

            // Current policy dictates that instance inout ports are directly connected to wire or hierarchical port.
            if (mPort->port_->getDirection() == DirectionTypes::INOUT)
            {
                continue;
            }

            // Determine the name for the connected element: Name of the instance plus port.
            QString physName = VerilogSyntax::legalizeName(instance->getInstanceName() + "_" +
                mPort->port_->name());

            // Create a wire for it.
            document->getPortWireWriters()->add(QSharedPointer<VerilogWireWriter>(
                new VerilogWireWriter(physName, mPort->vectorBounds_, mPort->arrayBounds_)));

            // Then create assignments, that connect it to logical wires.
            foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->upAssignments_)
            {
                QSharedPointer<VerilogAssignmentWriter> instanceAssignment = QSharedPointer<VerilogAssignmentWriter>
                    (new VerilogAssignmentWriter(physName, mpa, mPort, false));
                document->getInstanceAssignmentWriters()->add(instanceAssignment);
            }
        }
    }

    // Create wire writers for the interconnections
    foreach (QSharedPointer<MetaInterconnection> mInterconnect, *design->getInterconnections())
    {
        if (mInterconnect->wires_.size() > 0)
        {
            QSharedPointer<CommentWriter> connectionWireHeaderWriter(
                new CommentWriter(mInterconnect->name_ + " wires:"));
            connectionWireHeaderWriter->setIndent(4);
            document->getConnectionWireWriters()->add(connectionWireHeaderWriter);
        }

        foreach (QSharedPointer<MetaWire> gw, mInterconnect->wires_)
        {
            // Current policy dictates that hierarchical inout ports are directly connected to an instance port.
            bool hierInout = false;
            foreach (QSharedPointer<MetaPort> mPort, gw->hierPorts_)
            {
                if (mPort->port_->getDirection() == DirectionTypes::INOUT)
                {
                    hierInout = true;
                    break;
                }
            }

            if (!hierInout)
            {
                document->getConnectionWireWriters()->add(QSharedPointer<VerilogWireWriter>(
                    new VerilogWireWriter(VerilogSyntax::legalizeName(gw->name_), gw->bounds_, gw->arrayBounds_)));
            }
        }
    }

    // Create wire writers for the ad-hoc connections
    if (design->getAdHocWires()->size() > 0)
    {
        QSharedPointer<CommentWriter> adHocWireHeaderWriter(
            new CommentWriter("Ad-hoc wires:"));
        adHocWireHeaderWriter->setIndent(4);
        document->getAdHocWireWriters()->add(adHocWireHeaderWriter);
    }

    foreach (QSharedPointer<MetaWire> adHoc, *design->getAdHocWires())
    {
        // Current policy dictates that hierarchical in out ports are directly connected to an instance port.
        bool hierInout = false;
        foreach (QSharedPointer<MetaPort> mPort, adHoc->hierPorts_)
        {
            if (mPort->port_->getDirection() == DirectionTypes::INOUT)
            {
                hierInout = true;
                break;
            }
        }

        // Thus, if an ad-hoc wire is connected to any hierarchical inout ports, it is no written.
        if (!hierInout)
        {
            document->getAdHocWireWriters()->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(
                VerilogSyntax::legalizeName(adHoc->name_), adHoc->bounds_, adHoc->arrayBounds_)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogWriterFactory::addWritersToTopInDesiredOrder(QSharedPointer<VerilogDocument> document) const
{
    document->addWritersToTopComponent();
}

//-----------------------------------------------------------------------------
// Function: VerilogWriterFactory::createHeaderWriterForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Writer> VerilogWriterFactory::createHeaderWriterForInstance(QSharedPointer<MetaInstance> instance) const
{
    QString header = instance->getComponentInstance()->getDescription();
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    header.append("IP-XACT VLNV: " + instance->getComponent()->getVlnv().toString());

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}