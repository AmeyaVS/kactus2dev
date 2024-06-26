//-----------------------------------------------------------------------------
// File: GlobalMemoryMapHeaderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.05.2015
//
// Description:
// The implementation for creating c-headers of global memory maps.
//-----------------------------------------------------------------------------

#include "GlobalMemoryMapHeaderWriter.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/IPluginUtility.h>
#include <Plugins/MemoryMapHeaderGenerator/fileSaveDialog/filesavedialog.h>

#include <KactusAPI/include/ListParameterFinder.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>

#include <QDate>
#include <QDir>
#include <QDesktopServices>

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::GlobalMemoryMapHeaderWriter()
//-----------------------------------------------------------------------------
GlobalMemoryMapHeaderWriter::GlobalMemoryMapHeaderWriter(IPluginUtility* utility,
    QSharedPointer<Design> componentDesign, QSharedPointer<DesignConfiguration> componentDesignConfig,
    QObject* parentObject):
BaseMemoryMapHeaderWriter(utility),
parentObject_(parentObject),
componentDesign_(componentDesign),
componentDesignConfig_(componentDesignConfig),
operatedInterfaces_(),
saveOptions_()
{

}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::GlobalMemoryMapHeaderWriter()
//-----------------------------------------------------------------------------
GlobalMemoryMapHeaderWriter::~GlobalMemoryMapHeaderWriter()
{

}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::writeMemoryMapHeader()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::writeMemoryMapHeader(QSharedPointer<Component> globalComponent,
    QList<GlobalHeaderSaveModel::SaveFileOptions*> saveOptions)
{
    saveOptions_ = saveOptions;
    QList<GlobalHeaderSaveModel::SaveFileOptions*> options = saveOptions_;

    bool changed = false;

	operatedInterfaces_.clear();
    
    GlobalHeaderSaveModel model(utility_->getLibraryInterface(), parentObject_);
    model.setDesign(globalComponent, componentDesign_);

    if (options.isEmpty())
    {
        // create the dialog to display the headers to be generated
        FileSaveDialog dialog(utility_->getParentWidget());
        dialog.setModel(&model);

        int result = dialog.exec();

        // if user clicked cancel
        if (result == QDialog::Rejected)
        {
            informGenerationAbort();
            return;
        }

        options = model.getHeaderOptions();
    }

    informStartOfGeneration();

	foreach (GlobalHeaderSaveModel::SaveFileOptions* headerOpt, options)
    {
		QFile file(headerOpt->fileInfo_.absoluteFilePath());

        checkDirectoryStructure(headerOpt->fileInfo_.dir());

		if (!file.open(QFile::Truncate | QFile::WriteOnly))
        {
            openFileErrorMessage(headerOpt->fileInfo_.absoluteFilePath());
			break;
		}

		QTextStream stream(&file);

        QString description (" * Header file generated by Kactus2 for instance \"" + headerOpt->instance_ +
            "\" interface \"" + headerOpt->interface_ + "\".\n"+
            " * This file contains addresses of the memories and registers defined in the memory maps " +
            "of connected components.\n" +
            " * Source component: " + headerOpt->comp_.toString() + ".\n" +
        	"*/\n");

        QString headerGuard ("__" + headerOpt->instance_.toUpper() + "_" + headerOpt->interface_.toUpper() + "_H");

        writeTopOfHeaderFile(stream, headerOpt->fileInfo_.fileName(), headerGuard, description);

		QSharedPointer<ActiveInterface> cpuMasterInterface(new ActiveInterface(headerOpt->instance_, 
            headerOpt->interface_));
		operatedInterfaces_.append(cpuMasterInterface);

		// start the address parsing from the cpu's interface
		parseInterface(0, stream, cpuMasterInterface);

		stream << "#endif /* " << headerGuard << " */" << Qt::endl << Qt::endl;

        file.close();

        informWritingFinished(headerOpt->fileInfo_.fileName());

		addHeaderFile(globalComponent, headerOpt->fileInfo_, headerOpt->instance_, QStringList(),
            headerOpt->instanceId_);

		// a header file was added
		changed = true;

		// the list must be cleared when moving to completely new master interface so each header generation
        // starts from scratch.
		operatedInterfaces_.clear();
	}

	if (changed)
    {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(globalComponent);
	}

	// clear the members for next generation run
    componentDesign_.clear();
	operatedInterfaces_.clear();


    informGenerationComplete();
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::parseInterface()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::parseInterface(qint64 offset, QTextStream& stream, QSharedPointer<ActiveInterface> interface)
{
    Q_ASSERT(componentDesign_);
    Q_ASSERT(componentDesign_->containsHWInstance(interface->getComponentReference()));

	// parse the component containing the interface
	VLNV compVLNV = componentDesign_->getHWComponentVLNV(interface->getComponentReference());
	QSharedPointer<Document> libComp = utility_->getLibraryInterface()->getModel(compVLNV);
	QSharedPointer<Component> component = libComp.dynamicCast<Component>();
	Q_ASSERT(component);

    QSharedPointer<BusInterface> busInterface = component->getBusInterface(interface->getBusReference());    
    if (busInterface)
    {
        General::InterfaceMode interfaceMode = busInterface->getInterfaceMode();
        if (interfaceMode == General::MASTER)
        {
            parseMasterInterface(offset, component, stream, interface);
        }
        else if (interfaceMode == General::SLAVE)
        {
            parseSlaveInterface(offset, component, stream, interface);
        }
        else if (interfaceMode == General::MIRRORED_SLAVE)
        {
            parseMirroredSlaveInterface(offset, component, stream, interface);
        }
        else if (interfaceMode == General::MIRRORED_MASTER)
        {
            parseMirroredMasterInterface(offset, component, stream, interface);
        }
    }    
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::parseMasterInterface()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::parseMasterInterface(qint64 offset, QSharedPointer<Component> component,
    QTextStream& stream, QSharedPointer<ActiveInterface> interface)
{
    QString instanceID = getInstanceID(interface->getComponentReference());
    QSharedPointer<ListParameterFinder> finder = createParameterFinder(instanceID, component);

    QString masterBaseAddress = component->getBusInterface(interface->getBusReference())->getMaster()->getBaseAddress();

    offset += parsedValueFor(masterBaseAddress, finder).toInt();

    QList<QSharedPointer<ActiveInterface> > connected = getConnectedInterfaces(interface);
    foreach (QSharedPointer<ActiveInterface> targetInterface, connected)
    {
        if (!operatedInterfaces_.contains(targetInterface))
        {
            operatedInterfaces_.append(targetInterface);
            parseInterface(offset, stream, targetInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::parseSlaveInterface()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::parseSlaveInterface(qint64 offset, QSharedPointer<Component> component,
    QTextStream& stream, QSharedPointer<ActiveInterface> interface)
{
    QSharedPointer<TargetInterface> slave = component->getBusInterface(interface->getBusReference())->getSlave();
    Q_ASSERT(slave);

    QSharedPointer<MemoryMap> memMap;
    foreach (QSharedPointer<MemoryMap> availableMap, *component->getMemoryMaps())
    {
        if (availableMap->name() == slave->getMemoryMapRef())
        {
            memMap = availableMap;
            break;
        }            
    }

    if (memMap && memMap->hasMemoryBlocks())
    {
        stream << "/*" << Qt::endl;
        stream << " * Instance: " << interface->getComponentReference() << " Interface: " << interface->getBusReference() << Qt::endl;
        stream << " * Instance base address: 0x" << QString::number(offset, 16) << Qt::endl;
        stream << " * Source component: " << component->getVlnv().toString() << Qt::endl;

        // if there is a description for the component instance
        QString instanceDesc = componentDesign_->getHWInstanceDescription(interface->getComponentReference());
        if (!instanceDesc.isEmpty())
        {
            stream << " * Description:" << Qt::endl;
            stream << " * " << instanceDesc << Qt::endl;
        }

        QString instanceID = getInstanceID(interface->getComponentReference());

        QSharedPointer<ListParameterFinder> finder = createParameterFinder(instanceID, component);

        stream << " * The defines for the memory map \"" << memMap->name() << "\":" << Qt::endl;
        stream << "*/" << Qt::endl << Qt::endl;

        writeMemoryAddresses(finder, memMap, stream, offset, interface->getComponentReference());

        // if the registers within the instance are unique then do not concatenate with address block name
        /*QStringList regNames;
        if (memMap->uniqueRegisterNames(regNames))
        {*/

        writeRegisterFromMemoryMap(finder, memMap, stream, false, offset, interface->getComponentReference());
        
        /*}
        else
        {
            writeRegisterFromMemoryMap(finder, memMap, stream, true, offset, interface->getComponentReference());
        }*/
    }

    if (slave->hasTransparentBridge())
    {
        QStringList masterNames = slave->getMasterReferences();
        foreach (QString masterRef, masterNames)
        {
            if (!component->hasInterface(masterRef))
            {
                continue;
            }

            QSharedPointer<ActiveInterface> masterIF(new ActiveInterface(interface->getComponentReference(),
                masterRef));
            if (operatedInterfaces_.contains(masterIF))
            {
                continue;
            }

            operatedInterfaces_.append(masterIF);
            parseInterface(offset, stream, masterIF);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::parseMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::parseMirroredSlaveInterface(qint64 offset, QSharedPointer<Component> component,
    QTextStream& stream, QSharedPointer<ActiveInterface> interface)
{
    // increase the offset by the remap address of the mirrored slave interface
    QSharedPointer<BusInterface> busInterface = component->getBusInterface(interface->getBusReference());

    QString instanceId = getInstanceID(interface->getComponentReference());
    QSharedPointer<ListParameterFinder> finder = createParameterFinder(instanceId, component);

    for (QSharedPointer<MirroredTargetInterface::RemapAddress> remap :
        *busInterface->getMirroredSlave()->getRemapAddresses())
    {
        QString remapValue = parsedValueFor(remap->remapAddress_, finder);
        offset += remapValue.toInt();

        QList<QSharedPointer<ActiveInterface> > connected = getConnectedInterfaces(interface);
        for (QSharedPointer<ActiveInterface> targetInterface : connected)
        {
            if (!operatedInterfaces_.contains(targetInterface))
            {
                operatedInterfaces_.append(targetInterface);
                parseInterface(offset, stream, targetInterface);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::parseMirroredMaster()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::parseMirroredMasterInterface(qint64 offset, QSharedPointer<Component> component,
    QTextStream& stream, QSharedPointer<ActiveInterface> interface)
{
    // mirrored master interfaces are connected via channels
    // find the interfaces connected to the specified mirrored master interface
    QStringList connectedInterfaceNames;
    for (QSharedPointer<Channel> channel : *component->getChannels())
    {
        if (channel->getInterfaceNames().contains(interface->getBusReference()))
        {
            connectedInterfaceNames.append(channel->getInterfaceNames());
        }
    }

    connectedInterfaceNames.removeDuplicates();
    connectedInterfaceNames.removeAll(interface->getBusReference());

    QVector<QSharedPointer<const BusInterface> > connectedInterfaces;

    for (QString const& interfaceName : connectedInterfaceNames)
    {
        QSharedPointer<BusInterface> connectedInterface = component->getBusInterface(interfaceName);
        if (connectedInterface)
        {
            connectedInterfaces.append(connectedInterface);
        }
    }

    for (QSharedPointer<const BusInterface> busif: connectedInterfaces)
    {
        QSharedPointer<ActiveInterface> connectedInterface(new ActiveInterface(interface->getComponentReference(),
            busif->name()));
        if (!operatedInterfaces_.contains(connectedInterface))
        {
            operatedInterfaces_.append(connectedInterface);
            parseInterface(offset, stream, connectedInterface);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::displayMemoryMapHeader()
//-----------------------------------------------------------------------------
void GlobalMemoryMapHeaderWriter::displayMemoryMapHeader(QString const& filePath) const
{
    if (saveOptions_.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::checkConfigurableElementValues()
//-----------------------------------------------------------------------------
QSharedPointer<ListParameterFinder> GlobalMemoryMapHeaderWriter::createParameterFinder(QString const& instanceID,
    QSharedPointer<Component> component)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > configurableElementValues
        (new QList<QSharedPointer<Parameter> > ());

    QMap<QString, QString> configurableElementsInInstance;

    if (componentDesign_)
    {
        configurableElementsInInstance = getConfigurableElementsFromInstance(instanceID);
    }

    foreach (QSharedPointer<Parameter> parameterPointer, *component->getParameters())
    {
        QSharedPointer<Parameter> newConfigurableElement (new Parameter(*(parameterPointer)));

        if (configurableElementsInInstance.contains(newConfigurableElement->getValueId()))
        {
            newConfigurableElement->setValue(configurableElementsInInstance.value(
                newConfigurableElement->getValueId()));
        }

        configurableElementValues->append(newConfigurableElement);
    }

    QSharedPointer<ListParameterFinder> listFinder (new ListParameterFinder());
    listFinder->setParameterList(configurableElementValues);

    return listFinder;
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::getConfigurableElementsFromInstance()
//-----------------------------------------------------------------------------
QMap<QString, QString> GlobalMemoryMapHeaderWriter::getConfigurableElementsFromInstance(QString const& instanceId)
    const
{
    QMap<QString, QString> configurableElements;

    foreach (QSharedPointer<ComponentInstance> instance, *componentDesign_->getComponentInstances())
    {
        if (instance->getUuid() == instanceId)
        {
            foreach (QSharedPointer<ConfigurableElementValue> configurable,
                *instance->getConfigurableElementValues())
            {
                configurableElements.insert(configurable->getReferenceId(), configurable->getConfigurableValue());
            }

            break;
        }
    }

    return configurableElements;
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::getInstanceID()
//-----------------------------------------------------------------------------
QString GlobalMemoryMapHeaderWriter::getInstanceID(QString const& interfaceReference) const
{
    foreach (QSharedPointer<ComponentInstance> instance, *componentDesign_->getComponentInstances())
    {
        if (instance->getInstanceName() == interfaceReference)
        {
            return instance->getUuid();
        }
    }

    // This should not be reached.
    return QString();
}

//-----------------------------------------------------------------------------
// Function: GlobalMemoryMapHeaderWriter::getConnectedInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ActiveInterface> > GlobalMemoryMapHeaderWriter::getConnectedInterfaces(
    QSharedPointer<ActiveInterface> interface)
{
    QList<QSharedPointer<ActiveInterface> > connectedInterfaces;

    foreach (QSharedPointer<Interconnection> connection, *componentDesign_->getInterconnections())
    {
        if (connection->getStartInterface()->getComponentReference() == interface->getComponentReference() &&
            connection->getStartInterface()->getBusReference() == interface->getBusReference())
        {
            connectedInterfaces.append(*connection->getActiveInterfaces());
        }
        else
        {
            foreach (QSharedPointer<ActiveInterface> active, *connection->getActiveInterfaces())
            {
                if (active->getComponentReference() == interface->getComponentReference() &&
                    active->getBusReference() == interface->getBusReference())
                {
                    connectedInterfaces.append(connection->getStartInterface());
                }
            }
        }
    }

    return connectedInterfaces;
}
