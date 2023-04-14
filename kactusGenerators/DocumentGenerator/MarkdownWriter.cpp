#include "MarkdownWriter.h"

#include <QDateTime>
#include <QSettings>
#include <QString>

MarkdownWriter::MarkdownWriter(QSharedPointer<Component> component) :
    component_(component),
    componentNumber_(0)
{
}

MarkdownWriter::~MarkdownWriter()
{
}

void MarkdownWriter::writeHeader(QTextStream& stream)
{
    QSettings settings;
    
    // Write markdown header
    stream << "###### This document was generated by Kactus2 on " <<
        QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") <<
        " by user " << settings.value("General/Username").toString() <<
        Qt::endl;
}

void MarkdownWriter::writeKactusAttributes(QTextStream& stream)
{
    stream << "**Product hierarchy:** " <<
        KactusAttribute::hierarchyToString(component_->getHierarchy()) << "  " << Qt::endl
        << "**Component implementation:** " <<
        KactusAttribute::implementationToString(component_->getImplementation()) << "  " << Qt::endl
        << "**Component firmness:** " <<
        KactusAttribute::firmnessToString(component_->getFirmness()) << "  " << Qt::endl;
}

void MarkdownWriter::writeTableOfContents(QTextStream& stream)
{
    QString vlnv(component_->getVlnv().toString());

    // Write component header
    stream << componentNumber_ << ". [" << "Component " << component_->getVlnv().toString(" - ") << "]" <<
        "(#" << vlnv << ")  " << Qt::endl;

    // subHeader is running number that counts the number of sub headers for component
    int subHeader = 1;

    // Write component subheaders. Component has at least kactus attributes.
    stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[Kactus2 attributes" <<
        "](#" << vlnv << ".attributes)  " << Qt::endl;

    ++subHeader;

    if (component_->hasParameters())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[General parameters]" <<
            "(#" << vlnv << ".parameters)  " << Qt::endl;
        ++subHeader;
    }

    if (!component_->getMemoryMaps()->isEmpty())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[Memory maps]" <<
            "(#" << vlnv << ".memoryMaps)  " << Qt::endl;
        ++subHeader;
    }

    if (component_->hasPorts())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[Ports]" <<
            "(#" << vlnv << ".ports)  " << Qt::endl;
        ++subHeader;
    }
    
    if (component_->hasInterfaces())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[Bus interfaces]" <<
            "(#" << vlnv << ".interfaces)  " << Qt::endl;
        ++subHeader;
    }
    
    if (component_->hasFileSets())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[File sets]" <<
            "(#" << vlnv << ".fileSets)  " << Qt::endl;
        ++subHeader;
    }

    if (component_->hasViews())
    {
        stream << "\t" << componentNumber_ << "." << subHeader << ". " << "[Views]" <<
            "(#" << vlnv << ".views)  " << Qt::endl;
        ++subHeader;
    }
}

void MarkdownWriter::writeParameters(QTextStream& stream, ExpressionFormatter* formatter,
    int& subHeaderNumber)
{
    writeSubHeader(subHeaderNumber, stream, "General parameters", "parameters");

    QStringList headers({        
        QStringLiteral("Name"),
        QStringLiteral("Type"),
        QStringLiteral("Value"),
        QStringLiteral("Resolve"),
        QStringLiteral("Bit vector left"),
        QStringLiteral("Bit vector right"),
        QStringLiteral("Array left"),
        QStringLiteral("Array right"),
        QStringLiteral("Description")
    });
    
    QString tableSeparator(":---- ");
    QStringList tableSeparators = tableSeparator.repeated(headers.length()).split(" ", Qt::SkipEmptyParts);

    writeTableLine(stream, headers);
    writeTableLine(stream, tableSeparators);

    for (auto const& parameter : *component_->getParameters())
    {
        QStringList paramCells(QStringList()
            << parameter->name()
            << parameter->getType()
            << parameter->getValue()
            << parameter->getValueResolve()
            << parameter->getVectorLeft()
            << parameter->getVectorRight()
            << parameter->getArrayLeft()
            << parameter->getArrayRight()
            << parameter->description()
        );

        writeTableLine(stream, paramCells);
    }
}

void MarkdownWriter::writeSubHeader(unsigned int const& subHeaderNumber, QTextStream& stream,
    QString const& headerText, QString const& headerId)
{
    stream << "## " << componentNumber_ << "." << subHeaderNumber << " " << headerText << " <a id=\"" <<
        component_->getVlnv().toString() << "." << headerId << "\">  " << Qt::endl << Qt::endl;
}

void MarkdownWriter::setComponentNumber(unsigned int const& componentNumber)
{
    componentNumber_ = componentNumber;
}

void MarkdownWriter::writeTableLine(QTextStream& stream, QStringList const& cells) const
{
    for (auto const& cell : cells)
    {
        stream << "|" << cell;
    }

    stream << "|" << Qt::endl;
}
