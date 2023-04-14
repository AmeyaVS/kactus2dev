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

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component);
    virtual ~MarkdownWriter();
    void writeHeader(QTextStream& stream);
    void writeKactusAttributes(QTextStream& stream);
    void writeTableOfContents(QTextStream& stream);
    void writeParameters(QTextStream& stream, ExpressionFormatter* formatter,
        unsigned int& subHeaderNumber);
    void writeSubHeader(unsigned int const& subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId);

    void setComponentNumber(unsigned int const& componentNumber);

private:
    
    // The current component
    QSharedPointer<Component> component_;

    // Component number for table of contents
    unsigned int componentNumber_;
};

#endif // MARKDOWNWRITER_H