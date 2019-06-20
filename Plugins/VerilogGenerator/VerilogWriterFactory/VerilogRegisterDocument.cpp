//-----------------------------------------------------------------------------
// File: VerilogRegisterDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2019
//
// Description:
// Storage for Verilog register writers.
//-----------------------------------------------------------------------------

#include "VerilogRegisterDocument.h"

#include <QDateTime>
#include <QTextStream>

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentRegisterVerilogWriter.h>


//-----------------------------------------------------------------------------
// Function: VerilogRegisterDocument::write()
//-----------------------------------------------------------------------------
void VerilogRegisterDocument::write(QString const& )
{
    QString newFileContent;

    QTextStream outputStream(&newFileContent);
    headerWriter_->write(outputStream, getFileName(), QDateTime::currentDateTime());

    if (registerWriter_)
    {
        registerWriter_->write(outputStream);
    }

    setFileContent(newFileContent);
}

//-----------------------------------------------------------------------------
// Function: VerilogRegisterDocument::getImplementation()
//-----------------------------------------------------------------------------
void VerilogRegisterDocument::getBodyHighlight(int& begin, int& end) const
{
    QString error;
//     VerilogSyntax::findImplementation(getFileContent(), begin, end, error);
}

//-----------------------------------------------------------------------------
// Function: VerilogRegisterDocument::setHeaderWriter()
//-----------------------------------------------------------------------------
void VerilogRegisterDocument::setHeaderWriter(QSharedPointer<VerilogHeaderWriter> newHeaderWriter)
{
    headerWriter_ = newHeaderWriter;
}

//-----------------------------------------------------------------------------
// Function: VerilogRegisterDocument::getRegisterParameterWriters()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentRegisterVerilogWriter> VerilogRegisterDocument::getRegisterWriter() const
{
    return registerWriter_;
}

//-----------------------------------------------------------------------------
// Function: VerilogRegisterDocument::setRegisterWriter()
//-----------------------------------------------------------------------------
void VerilogRegisterDocument::setRegisterWriter(QSharedPointer<ComponentRegisterVerilogWriter> newRegisterWriter)
{
    registerWriter_ = newRegisterWriter;
}
