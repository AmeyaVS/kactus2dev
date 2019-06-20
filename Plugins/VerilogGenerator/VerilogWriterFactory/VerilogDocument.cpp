//-----------------------------------------------------------------------------
// File: VerilogDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers.
//-----------------------------------------------------------------------------

#include "VerilogDocument.h"

#include <QDateTime>
#include <QTextStream>

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: VerilogDocument::write()
//-----------------------------------------------------------------------------
void VerilogDocument::write(QString const& /*outputDirectory*/)
{
    QString newFileContent;

    QTextStream outputStream(&newFileContent);
    headerWriter_->write(outputStream, getFileName(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    setFileContent(newFileContent);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getImplementation()
//-----------------------------------------------------------------------------
void VerilogDocument::getBodyHighlight(int& begin, int& end) const
{
    QString error;
    VerilogSyntax::findImplementation(getFileContent(), begin, end, error);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::selectImplementation()
//-----------------------------------------------------------------------------
bool VerilogDocument::selectImplementation(QString const& code, QString& implementation,
    QString& postModule, QString& error)
{
    // The Verilog syntax defines how it should be done.
    return VerilogSyntax::selectImplementation(code, implementation, postModule, error);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getImplementation()
//-----------------------------------------------------------------------------
QString VerilogDocument::getImplementation() const
{
    return implementation_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::setImplementation()
//-----------------------------------------------------------------------------
void VerilogDocument::setImplementation(QString const& newImplementation)
{
    implementation_ = newImplementation;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getPostModule()
//-----------------------------------------------------------------------------
QString VerilogDocument::getPostModule() const
{
    return postModule_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::setPostModule()
//-----------------------------------------------------------------------------
void VerilogDocument::setPostModule(QString const& newPostModule)
{
    postModule_ = newPostModule;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::setHeaderWriter()
//-----------------------------------------------------------------------------
void VerilogDocument::setHeaderWriter(QSharedPointer<VerilogHeaderWriter> newHeaderWriter)
{
    headerWriter_ = newHeaderWriter;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getTopWriter()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentVerilogWriter> VerilogDocument::getTopWriter() const
{
    return topWriter_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::setTopWriter()
//-----------------------------------------------------------------------------
void VerilogDocument::setTopWriter(QSharedPointer<ComponentVerilogWriter> newTopWriter)
{
    topWriter_ = newTopWriter;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::initializeDesignWriters()
//-----------------------------------------------------------------------------
void VerilogDocument::initializeDesignWriters()
{
    instanceWriters_.clear();

    interconnectionWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    connectionWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    adHocWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    portWireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    topAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    topDefaultWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    instanceAssignmentWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getTopDefaultWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getTopDefaultWriters() const
{
    return topDefaultWriters_;

}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getInterconnectionWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getInterconnectionWriters() const
{
    return interconnectionWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getConnectionWireWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getConnectionWireWriters() const
{
    return connectionWireWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getAdHocWireWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getAdHocWireWriters() const
{
    return adHocWireWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getPortWireWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getPortWireWriters() const
{
    return portWireWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getTopAssignmentWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getTopAssignmentWriters() const
{
    return topAssignmentWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getInstanceAssignmentWriters()
//-----------------------------------------------------------------------------
QSharedPointer<WriterGroup> VerilogDocument::getInstanceAssignmentWriters() const
{
    return instanceAssignmentWriters_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::addInstanceWriter()
//-----------------------------------------------------------------------------
void VerilogDocument::addInstanceWriter(QSharedPointer<ComponentInstanceVerilogWriter> newInstanceWriter)
{
    instanceWriters_.append(newInstanceWriter);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::addInstanceHeaderWriter()
//-----------------------------------------------------------------------------
void VerilogDocument::addInstanceHeaderWriter(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter,
    QSharedPointer<Writer> headerWriter)
{
    instanceHeaderWriters_.insert(instanceWriter, headerWriter);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::addWritersToTopComponent()
//-----------------------------------------------------------------------------
void VerilogDocument::addWritersToTopComponent()
{
    topWriter_->add(interconnectionWriters_);
    topWriter_->add(connectionWireWriters_);
    topWriter_->add(adHocWireWriters_);
    topWriter_->add(portWireWriters_);
    topWriter_->add(topAssignmentWriters_);
    topWriter_->add(topDefaultWriters_);
    topWriter_->add(instanceAssignmentWriters_);

    for (auto instanceWriter : instanceWriters_)
    {
        QSharedPointer<Writer> headerWriter = instanceHeaderWriters_[instanceWriter];

        QSharedPointer<WriterGroup> instanceGroup(new WriterGroup);
        instanceGroup->add(headerWriter);
        instanceGroup->add(instanceWriter);

        topWriter_->add(instanceGroup);
    }
}
