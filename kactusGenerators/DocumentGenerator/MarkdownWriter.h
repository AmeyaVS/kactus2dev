//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Writes markdown documentation
//-----------------------------------------------------------------------------

#ifndef MARKDOWNWRITER_H
#define MARKDOWNWRITER_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>
#include <QList>

class ExpressionFormatter;

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter);
    virtual ~MarkdownWriter();
    void writeHeader(QTextStream& stream);
    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber);
    void writeTableOfContents(QTextStream& stream);
    void writeParameters(QTextStream& stream, int subHeaderNumber);
    void writeSubHeader(unsigned int subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId);
    virtual void writeMemoryMaps(QTextStream& stream, int subHeaderNumber);
    void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber,int memoryMapNumber);
    void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber);
    void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister);

    void setComponentNumber(unsigned int componentNumber);

private:

    // TODO: Move these "finder" methods to DocumentationWriter
    // Finds the address blocks of a memory map
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    // Finds the registers of an address block
    QList<QSharedPointer <Register> > getAddressBlockRegisters(QSharedPointer<AddressBlock> addressBlock) const;

    // Get the information for the reset values of the selected field.
    QString getFieldResetInfo(QSharedPointer<Field> field) const;

    // Writes a line with specified cells to a MD table
    void writeTableLine(QTextStream& stream, QStringList const& cells) const;

    // Writes the table separator to a MD table
    void writeTableSeparator(QTextStream& stream, int columns);
    
    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    // Stringified VLNV of the component
    QString vlnvString_;
    
    // The current component
    QSharedPointer<Component> component_;

    // Component number in the hierarchy
    unsigned int componentNumber_;
};

#endif // MARKDOWNWRITER_H