//-----------------------------------------------------------------------------
// File: ComponentRegisterVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 06.06.2019
//
// Description:
// Class for writing component registers as a Verilog module.
//-----------------------------------------------------------------------------

#include "ComponentRegisterVerilogWriter.h"

// #include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

#include <IPXACTmodels/Component/Component.h>

namespace
{
    QString const MAINDATAWIDTH_STRING = QLatin1String("DATA_WIDTH");
    QString const MAINADDRESSWIDTH_STRING = QLatin1String("ADDR_WIDTH");

    QString const REGISTERSIZE_STRING = QLatin1String("_SIZE");
    QString const REGISTEROFFSET_STRING = QLatin1String("_ADDROFFSET");
    QString const FIELDWIDTH_STRING = QLatin1String("_BITWIDTH");
    QString const FIELDOFFSET_STRING = QLatin1String("_BITOFFSET");

    QString const ADDRESS_STRING = QLatin1String("addr");
    QString const DATA_STRING = QLatin1String("_data");

    QString const INPUT_STRING = QLatin1String("input");
    QString const OUTPUT_STRING = QLatin1String("output");
    QString const LOGIC_STRING = QLatin1String("logic");

    QString const REGISTERSHORT_STRING = QLatin1String("_reg");

    QString const CLK_STRING = QLatin1String("clk");
    QString const RSTN_STRING = QLatin1String("rstn");
    QString const WDATA_STRING = QLatin1String("wdata");
    QString const WRITENOTREAD_STRING =  QLatin1String("write_not_read");
    QString const RDATA_STRING = QLatin1String("rdata");

    QString const PARAMETER_STRING = QLatin1String("parameter");
    QString const LOCALPARAMETER_STRING = QLatin1String("localparam");

    QString const RESETVALUE_STRING = QLatin1String("_reset_value");
    QString const RESETMASK_STRING = QLatin1String("_reset_mask");

    QString const ALWAYS_FF_STRING = QLatin1String("always_ff @");
    QString const POSEDGE_STRING = QLatin1String("posedge");
    QString const NEGEDGE_STRING = QLatin1String("negedge");

    QString const CONTROLREGISTERS_STRING = QLatin1String("control_register_FFs");
    QString const STATUSREGISTERS_STRING = QLatin1String("status_register_FFs");

    QString const IF_STRING = QLatin1String("if");
    QString const ELSE_STRING = QLatin1String("else");
    QString const ELSEIF_STRING = ELSE_STRING + QLatin1Char(' ') + IF_STRING;
    QString const EQUALCHECK_STRING = QLatin1String("==");
    QString const ZEROBITS_STRING = QLatin1String("1'b0");
    QString const CASE_STRING = QLatin1String("case");
    QString const ENDCASE_STRING = QLatin1String("endcase");

    QString const ALWAYSCOMB_STRING = QLatin1String("always_comb");
    QString const READLOGIC_STRING = QLatin1String("read_logic");
    QString const CONSTRUCTDATA_STRING = QLatin1String("construct_data");
    QString const DRIVEOUTPUTS_STRING = QLatin1String("drive_outputs");

    QString const BEGIN_STRING = QLatin1String("begin");
    QString const END_STRING = QLatin1String("end");
};

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::ComponentRegisterVerilogWriter
//-----------------------------------------------------------------------------
ComponentRegisterVerilogWriter::ComponentRegisterVerilogWriter(QSharedPointer<MetaComponent> component,
    QString const& moduleName):
WriterGroup(),
registerModuleName_(moduleName),
component_(component),
dataWidth_(QLatin1String("32")),
addressWidth_(getAddressWidth())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getAddressWidth()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getAddressWidth() const
{
    quint64 addressMax = 0;

    for (auto componentRegister : *component_->getRegisters())
    {
        quint64 offset = componentRegister->offsetInt_;
        quint64 size = componentRegister->sizeInt_;

        quint64 registerEndAddress = componentRegister->offsetInt_ + componentRegister->sizeInt_ - 1;
        if (registerEndAddress > addressMax)
        {
            addressMax = registerEndAddress;
        }
    }

    quint64 addressWidth = ceil(log2(qreal(addressMax)));

    return QString::number(addressWidth);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::write
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::write(QTextStream& outputStream) const
{
    if (nothingToWrite())
    {
        return;
    }

    writeModuleDeclaration(outputStream);
    writeLocalParameters(outputStream);
    writeRegisterData(outputStream);
    writeControlRegisters(outputStream);
    writeStatusRegisters(outputStream);

    writeReadLogicComb(outputStream);
    writeConstructComb(outputStream);
    writeDriveOutputsComb(outputStream);

	writeModuleEnd(outputStream);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::nothingToWrite
//-----------------------------------------------------------------------------
bool ComponentRegisterVerilogWriter::nothingToWrite() const
{
    return component_.isNull();
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeModuleDeclaration()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeModuleDeclaration(QTextStream& outputStream) const
{
    outputStream << "module " << registerModuleName_;

    writeParameterDeclarations(outputStream);
    writePortDeclarations(outputStream);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
{
    if (component_->getRegisters()->isEmpty())
    {
        return;
    }

    outputStream << endl << " #(" << endl;

    writeMainParameters(outputStream);

    outputStream << endl;

    QSharedPointer<MetaRegister> lastRegister = component_->getRegisters()->last();
    for (auto const componentRegister : *component_->getRegisters())
    {
        writeSingleRegisterParameters(outputStream, componentRegister);

        if (componentRegister != lastRegister)
        {
            outputStream << QLatin1Char(',');
            outputStream << endl;
        }

        outputStream << endl;
    }

    outputStream << ")" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeMainParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeMainParameters(QTextStream& outputStream) const
{
    writeParameter(outputStream, PARAMETER_STRING, MAINDATAWIDTH_STRING, dataWidth_);
    outputStream << QLatin1Char(',') << endl;

    writeParameter(outputStream, PARAMETER_STRING, MAINADDRESSWIDTH_STRING, addressWidth_);
    outputStream << QLatin1Char(',') << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeSingleRegister()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeSingleRegisterParameters(QTextStream& outputStream,
    QSharedPointer<MetaRegister> componentRegister) const
{
    QString registerName = componentRegister->name_;

    writeParameter(outputStream, PARAMETER_STRING, getRegisterSize(componentRegister), componentRegister->size_);
    outputStream << QLatin1Char(',') << endl;
    writeParameter(
        outputStream, PARAMETER_STRING, getRegisterOffset(componentRegister), componentRegister->offset_);

    if (!componentRegister->fields_.isEmpty())
    {
        outputStream << QLatin1Char(',') << endl;

        writeRegisterFieldParameters(outputStream, componentRegister);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getRegisterSize()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getRegisterSize(QSharedPointer<MetaRegister> componentRegister) const
{
    return componentRegister->name_.toUpper() + REGISTERSIZE_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getRegisterOffset()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getRegisterOffset(QSharedPointer<MetaRegister> componentRegister) const
{
    return componentRegister->name_.toUpper() + REGISTEROFFSET_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeParameter()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeParameter(QTextStream& outputStream, QString const& parameterType,
    QString const& parameterName, QString const& parameterValue) const
{
    QString parameterDeclaration =
        parameterType.leftJustified(39) + parameterName.leftJustified(16) + QLatin1String(" = ") + parameterValue;

    outputStream << indentation() << parameterDeclaration;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterFieldParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterFieldParameters(QTextStream& outputStream,
    QSharedPointer<MetaRegister> componentRegister) const
{
    QString registerName = componentRegister->name_;
    for (int i = 0; i < componentRegister->fields_.size(); ++i)
    {
        MetaField const& field = componentRegister->fields_.at(i);
        QString fieldName = getCombinedRegisterFieldName(componentRegister, field);

        writeParameter(outputStream, PARAMETER_STRING, getFieldBitOffset(fieldName), field.offset_);
        outputStream << QLatin1Char(',') << endl;

        writeParameter(outputStream, PARAMETER_STRING, getFieldWidth(fieldName), field.width_);

        if (i < componentRegister->fields_.size() - 1)
        {
            outputStream << QLatin1Char(',') << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    outputStream << QLatin1Char('(') << endl;

    writePort(outputStream, INPUT_STRING, CLK_STRING);
    outputStream << QLatin1Char(',') << endl;
    writePort(outputStream, INPUT_STRING, RSTN_STRING);
    outputStream << QLatin1Char(',') << endl;
    writePort(outputStream, INPUT_STRING, ADDRESS_STRING, MAINADDRESSWIDTH_STRING);
    outputStream << QLatin1Char(',') << endl;
    writePort(outputStream, INPUT_STRING, WDATA_STRING, MAINDATAWIDTH_STRING);
    outputStream << QLatin1Char(',') << endl;
    writePort(outputStream, INPUT_STRING, WRITENOTREAD_STRING);
    outputStream << QLatin1Char(',') << endl;
    writePort(outputStream, OUTPUT_STRING, RDATA_STRING, MAINDATAWIDTH_STRING);

    writeRegisterPortDeclarations(outputStream);

    outputStream << endl << QLatin1String(");") << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterPortDeclarations()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterPortDeclarations(QTextStream& outputStream) const
{
    for (auto componentRegister : *component_->getRegisters())
    {
        for (auto registerField : componentRegister->fields_)
        {
            outputStream << QLatin1Char(',') << endl;

            QString portType("");

            AccessTypes::Access fieldAccess = registerField.access_;
            if (fieldPortIsInput(fieldAccess))
            {
                portType = INPUT_STRING;
            }
            else if (fieldPortIsOutput(fieldAccess))
            {
                portType = OUTPUT_STRING;
            }

            QString combiFieldName = getCombinedRegisterFieldName(componentRegister, registerField);

            QString portSize = getFieldWidth(combiFieldName);
            QString portName = combiFieldName + QLatin1Char('_') + portType;

            writePort(outputStream, portType, portName, portSize);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::fieldPortIsInput()
//-----------------------------------------------------------------------------
bool ComponentRegisterVerilogWriter::fieldPortIsInput(AccessTypes::Access const& fieldAccess) const
{
    return fieldAccess == AccessTypes::READ_ONLY || fieldAccess == AccessTypes::READ_WRITEONCE;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::fieldPortIsOutput()
//-----------------------------------------------------------------------------
bool ComponentRegisterVerilogWriter::fieldPortIsOutput(AccessTypes::Access const& fieldAccess) const
{
    return fieldAccess == AccessTypes::WRITE_ONLY || fieldAccess == AccessTypes::READ_WRITE ||
        fieldAccess == AccessTypes::WRITEONCE;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getCombinedRegisterFieldName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getCombinedRegisterFieldName(
    QSharedPointer<MetaRegister> containingRegister, MetaField const& field) const
{
    return containingRegister->name_ + QLatin1Char('_') + field.name_;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writePort()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writePort(QTextStream& outputStream, QString const& portType,
    QString const& portName, QString const& portSize) const
{
    QString portDeclaration = QLatin1String("<type> ") + portName;

    QString widthDeclaration = "";
    if (!portSize.isEmpty())
    {
        widthDeclaration = QLatin1Char('[') + portSize + QLatin1String("-1:0]");
    }

    QString portDefinition =
        portType.leftJustified(6) + QLatin1Char(' ') + LOGIC_STRING + QLatin1Char(' ') + widthDeclaration;

    portDeclaration.replace(QLatin1String("<type>"), portDefinition.leftJustified(38));

    outputStream << indentation() << portDeclaration;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::indentation()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::indentation(int depth /*= 1*/) const
{
    QString indent = "    ";

    return indent.repeated(depth);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeLocalParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeLocalParameters(QTextStream& outputStream) const
{
    for (int i = 0; i < component_->getRegisters()->size(); ++i)
    {
        QSharedPointer<MetaRegister> componentRegister = component_->getRegisters()->at(i);

        QString parameterName = getRegisterAddressName(componentRegister);
        QString registerAddress =
            getValueInBaseFormat(componentRegister->offset_, addressWidth_, addressWidth_.toULongLong(), 2);

        writeParameter(outputStream, LOCALPARAMETER_STRING, parameterName, registerAddress);

        writeLocalFieldParameters(outputStream, componentRegister);

        if (i < component_->getRegisters()->size() - 1)
        {
            outputStream << QLatin1Char(',');
        }

        outputStream << endl << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getValueInBitFormat()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getValueInBaseFormat(QString const& value, QString const& bitWidth,
    quint64 const& widthInt, int const& base) const
{
    quint64 fieldWidth = widthInt;

    QString valueIdentifier("");
    if (base == 2)
    {
        valueIdentifier = QLatin1String("'b");
    }
    else if (base == 16)
    {
        qreal widthReal = fieldWidth;
        widthReal = widthReal / 4;
        fieldWidth = ceil(widthReal);

        valueIdentifier = QLatin1String("'h");
    }

    return bitWidth +
        valueIdentifier + QString("%1").arg(value.toULongLong(), fieldWidth, base, QChar('0'));
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeLocalFieldParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeLocalFieldParameters(QTextStream& outputStream,
    QSharedPointer<MetaRegister> componentRegister) const
{
    for (auto registerField : componentRegister->fields_)
    {
        outputStream << QLatin1Char(',') << endl;

        QString combiName = getFieldResetValueName(componentRegister, registerField);
        QString resetValue = registerField.resetValue_;
        if (!resetValue.contains(QLatin1Char('\'')))
        {
            resetValue = getValueInBaseFormat(resetValue, registerField.width_, registerField.widthInt_, 16);
        }

        writeParameter(outputStream, LOCALPARAMETER_STRING, combiName, resetValue);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldResetValueName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldResetValueName(QSharedPointer<MetaRegister> componentRegister,
    MetaField const& field) const
{
    QString combiName = getCombinedRegisterFieldName(componentRegister, field) + RESETVALUE_STRING;
    return combiName;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterData()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterData(QTextStream& outputStream) const
{
    QStringList registerNames;

    for (auto componentRegister : *component_->getRegisters())
    {
        registerNames.append(getRegisterDataName(componentRegister));

        for (auto registerField : componentRegister->fields_)
        {
            QString fieldName = getCombinedRegisterFieldName(componentRegister, registerField);
            QString fieldWidth = getFieldWidth(fieldName);

            writeItemData(outputStream, fieldWidth, getFieldRegisterName(componentRegister, registerField));
            outputStream << QLatin1Char(';') << endl;
        }
    }

    outputStream << endl;
    writeItemData(outputStream, MAINDATAWIDTH_STRING, registerNames.join(", "));
    outputStream << QLatin1Char(';') << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldRegisterName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldRegisterName(QSharedPointer<MetaRegister> containingRegister,
    MetaField const& field) const
{
    QString fieldName = getCombinedRegisterFieldName(containingRegister, field);
    return fieldName + REGISTERSHORT_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeItemData()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeItemData(QTextStream& outputStream, QString const& width,
    QString const& dataName) const
{
    QString dataRange = LOGIC_STRING + QLatin1String(" [") + width + QLatin1String("-1:0]");
    QString dataDeclaration = dataRange.leftJustified(39) + dataName;

    outputStream << indentation() << dataDeclaration;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeControlRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeControlRegisters(QTextStream& outputStream) const
{
    int indentationCount = 1;

    writeFFLine(outputStream);
    writeRegisterBeginLine(outputStream, CONTROLREGISTERS_STRING, indentationCount);
    writeResetCondition(outputStream, indentationCount, true);
    writeWriteNotReadCondition(outputStream, indentationCount);
    writeEndLine(outputStream, indentationCount);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeFFLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeFFLine(QTextStream& outputStream) const
{
    QString ffLine = ALWAYS_FF_STRING + QLatin1String("( ") + POSEDGE_STRING + QLatin1Char(' ') + CLK_STRING +
        QLatin1String(" or ") + NEGEDGE_STRING + QLatin1Char(' ') + RSTN_STRING + QLatin1String(" )");

    outputStream << indentation() << ffLine << QLatin1Char(' ');
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterBeginLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterBeginLine(QTextStream& outputStream, QString const& lineName,
    int& intendationCount) const
{
    outputStream << lineName << QLatin1String(": ") << endl;
    writeBeginLine(outputStream, intendationCount);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeBeginLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeBeginLine(QTextStream& outputStream, int& indentationCount) const
{
    outputStream << indentation(indentationCount) << BEGIN_STRING << endl;
    indentationCount++;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeEndLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeEndLine(QTextStream& outputStream, int& indentationCount) const
{
    indentationCount--;
    outputStream << indentation(indentationCount) << END_STRING << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeResetCondition()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeResetCondition(QTextStream& outputStream, int& indentationCount,
    bool isControl) const
{
    QString resetCondition = IF_STRING + QLatin1String(" ( ") + RSTN_STRING + QLatin1Char(' ') +
        EQUALCHECK_STRING + QLatin1Char(' ') + ZEROBITS_STRING + QLatin1String(" )");

    outputStream << indentation(indentationCount) << resetCondition << endl;

    writeBeginLine(outputStream, indentationCount);
    writeResetRegisters(outputStream, indentationCount, isControl);
    writeEndLine(outputStream, indentationCount);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeResetRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeResetRegisters(QTextStream& outputStream, int& indentationCount,
    bool isControl) const
{
    for (auto componentRegister : *component_->getRegisters())
    {
        for (auto field : componentRegister->fields_)
        {
            QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
            QString fieldResetName = getFieldResetValueName(componentRegister, field);

            if ((isControl && fieldPortIsOutput(field.access_)) || (!isControl && fieldPortIsInput(field.access_)))
            {
                outputStream << indentation(indentationCount) << fieldRegisterName << QLatin1String(" = ") <<
                    fieldResetName << QLatin1Char(';') << endl;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeWriteNotReadCondition()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeWriteNotReadCondition(QTextStream& outputStream, int& indentationCount)
    const
{
    QString condition = ELSEIF_STRING + QLatin1String(" ( ") + WRITENOTREAD_STRING + QLatin1Char(')');
    outputStream << indentation(indentationCount) << condition << endl;

    writeBeginLine(outputStream, indentationCount);
    writeWriteNotReadRegisters(outputStream, indentationCount);
    writeEndLine(outputStream, indentationCount);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeWriteNotReadRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeWriteNotReadRegisters(QTextStream& outputStream, int& indentationCount)
    const
{
    writeCaseStartLine(outputStream, indentationCount);

    for (auto componentRegister : *component_->getRegisters())
    {
        QVector<MetaField> outputFields = getOutputFields(componentRegister);
        if (!outputFields.isEmpty())
        {
            QString registerAddress = getRegisterAddressName(componentRegister);
            outputStream << indentation(indentationCount) << registerAddress << QLatin1Char(':') << endl;
            writeBeginLine(outputStream, indentationCount);

            for (auto field : outputFields)
            {
                QString fieldName = getCombinedRegisterFieldName(componentRegister, field);
                QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
                QString fieldOffsetParameter = getFieldBitOffset(fieldName);
                QString fieldWidthParameter = getFieldWidth(fieldName);

                QString registerData = fieldRegisterName + QLatin1String(" = ") + WDATA_STRING + QLatin1Char('[') +
                    fieldOffsetParameter + QLatin1Char('+') + fieldWidthParameter + QLatin1String("-1:") +
                    fieldOffsetParameter + QLatin1String("];");

                outputStream << indentation(indentationCount) << registerData << endl;
            }

            writeEndLine(outputStream, indentationCount);
        }
    }

    writeCaseEndLine(outputStream, indentationCount);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeCaseStartLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeCaseStartLine(QTextStream& outputStream, int& indentationCount) const
{
    outputStream << indentation(indentationCount) << CASE_STRING << QLatin1Char('(') << ADDRESS_STRING <<
        QLatin1Char(')') << endl;

    indentationCount++;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeCaseEndLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeCaseEndLine(QTextStream& outputStream, int& indentationCount) const
{
    indentationCount--;

    outputStream << indentation(indentationCount) << ENDCASE_STRING << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getOutputFields()
//-----------------------------------------------------------------------------
QVector<MetaField> ComponentRegisterVerilogWriter::getOutputFields(QSharedPointer<MetaRegister> componentRegister)
    const
{
    QVector<MetaField> outputFields;

    for (auto field : componentRegister->fields_)
    {
        if (fieldPortIsOutput(field.access_))
        {
            outputFields.append(field);
        }
    }

    return outputFields;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getInputRegisters()
//-----------------------------------------------------------------------------
QVector<MetaField> ComponentRegisterVerilogWriter::getInputFields(QSharedPointer<MetaRegister> componentRegister)
    const
{
    QVector<MetaField> outputFields;

    for (auto field : componentRegister->fields_)
    {
        if (fieldPortIsInput(field.access_))
        {
            outputFields.append(field);
        }
    }

    return outputFields;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeStatusRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeStatusRegisters(QTextStream& outputStream) const
{
    int indentationCount = 1;

    writeFFLine(outputStream);
    writeRegisterBeginLine(outputStream, STATUSREGISTERS_STRING, indentationCount);
    writeResetCondition(outputStream, indentationCount, false);
    writeStatusRegisterChange(outputStream, indentationCount);
    writeEndLine(outputStream, indentationCount);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeStatusRegisterChange()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeStatusRegisterChange(QTextStream& outputStream, int& indentationCount)
    const
{
    outputStream << indentation(indentationCount) << ELSE_STRING << endl;

    writeBeginLine(outputStream, indentationCount);
    writeStatusRegisterInput(outputStream, indentationCount);
    writeEndLine(outputStream, indentationCount);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeStatusRegisterInput()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeStatusRegisterInput(QTextStream& outputStream, int& indentationCount)
    const
{
    for (auto componentRegister : *component_->getRegisters())
    {
        QVector<MetaField> inputFields = getInputFields(componentRegister);
        for (auto field : inputFields)
        {
            QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
            QString inputPortName = getCombinedRegisterFieldName(componentRegister, field) + QLatin1Char('_') +
                INPUT_STRING;

            QString registerData = fieldRegisterName + QLatin1String(" = ") + inputPortName + QLatin1Char(';');
            outputStream << indentation(indentationCount) << registerData << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldBitOffsetName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldBitOffset(QString const& combinedName) const
{
    return combinedName.toUpper() + FIELDOFFSET_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldWidthName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldWidth(QString const& combinedName) const
{
    return combinedName.toUpper() + FIELDWIDTH_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeReadLogicComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeReadLogicComb(QTextStream& outputStream) const
{
    int indentationCount = 1;

    writeCombLine(outputStream, indentationCount);
    outputStream << indentation(indentationCount);
    writeRegisterBeginLine(outputStream, READLOGIC_STRING, indentationCount);
    writeDataZeroLine(outputStream, indentationCount, RDATA_STRING);

    writeReadLogicRegisters(outputStream, indentationCount);

    writeEndLine(outputStream, indentationCount);
    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeCombLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeCombLine(QTextStream& outputStream, int& indentationCount) const
{
    outputStream << indentation(indentationCount) << ALWAYSCOMB_STRING << endl;
    indentationCount++;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeDataZeroLine()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeDataZeroLine(QTextStream& outputStream, int& indentationCount,
    QString const& dataName) const
{
    QString dataString = dataName + QLatin1String(" = 0;");
    outputStream << indentation(indentationCount) << dataString << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeReadLogicRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeReadLogicRegisters(QTextStream& outputStream, int& indentationCount)
    const
{
    writeCaseStartLine(outputStream, indentationCount);

    for (QSharedPointer<MetaRegister> currentRegister : *component_->getRegisters())
    {
        QString registerAddress = getRegisterAddressName(currentRegister);
        outputStream << indentation(indentationCount) << registerAddress << QLatin1Char(':') << endl;
        writeBeginLine(outputStream, indentationCount);

        outputStream << indentation(indentationCount) << RDATA_STRING << QLatin1String(" = ") <<
            getRegisterDataName(currentRegister) << QLatin1Char(';') << endl;

        writeEndLine(outputStream, indentationCount);
    }

    writeCaseEndLine(outputStream, indentationCount);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeConstructComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeConstructComb(QTextStream& outputStream) const
{
    int indentationCount = 1;

    writeCombLine(outputStream, indentationCount);
    outputStream << indentation(indentationCount);
    writeRegisterBeginLine(outputStream, CONSTRUCTDATA_STRING, indentationCount);

    writeRegisterDataZeroLines(outputStream, indentationCount);
    writeRegisterConstructData(outputStream, indentationCount);

    writeEndLine(outputStream, indentationCount);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterDataZeroLines()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterDataZeroLines(QTextStream& outputStream, int& indentationCount)
    const
{
    for (auto componentRegister : *component_->getRegisters())
    {
        QString registerData = getRegisterDataName(componentRegister) + QLatin1String(" = 0;");
        outputStream << indentation(indentationCount) << registerData << endl;
    }

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeRegisterConstructData()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeRegisterConstructData(QTextStream& outputStream, int& indentationCount)
    const
{
    for (auto componentregister : *component_->getRegisters())
    {
        QString registerName = getRegisterDataName(componentregister);

        for (auto field : componentregister->fields_)
        {
            QString fieldCombiName = getCombinedRegisterFieldName(componentregister, field);
            QString fieldOffset = getFieldBitOffset(fieldCombiName);
            QString fieldWidth = getFieldWidth(fieldCombiName);

            QString dataLine = registerName + QLatin1Char('[') + fieldOffset + QLatin1Char('+') + fieldWidth +
                QLatin1String("-1:") + fieldOffset + QLatin1String("] = ") + fieldCombiName + QLatin1Char(';');
            outputStream << indentation(indentationCount) << dataLine << endl;
        }

        if (componentregister != component_->getRegisters()->last())
        {
            outputStream << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeDriveOutputsComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeDriveOutputsComb(QTextStream& outputStream) const
{
    int indentationCount = 1;

    writeCombLine(outputStream, indentationCount);
    outputStream << indentation(indentationCount);
    writeRegisterBeginLine(outputStream, DRIVEOUTPUTS_STRING, indentationCount);

    writeOutputControls(outputStream, indentationCount);

    writeEndLine(outputStream, indentationCount);

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeOutputControls()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeOutputControls(QTextStream& outputStream, int& indentationCount) const
{
    for (auto componentRegister : *component_->getRegisters())
    {
        for (auto field : componentRegister->fields_)
        {
            if (fieldPortIsOutput(field.access_))
            {
                QString portName = getPortName(componentRegister, field, OUTPUT_STRING);
                QString fieldRegister = getFieldRegisterName(componentRegister, field);

                outputStream << indentation(indentationCount) << portName << QLatin1String(" = ") << fieldRegister <<
                    QLatin1Char(';') << endl;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getPortName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getPortName(QSharedPointer<MetaRegister> containingRegister,
    MetaField const& field, QString const& portType) const
{
    return getCombinedRegisterFieldName(containingRegister, field) + QLatin1Char('_') + portType;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::writeModuleEnd()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getRegisterAddressName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getRegisterAddressName(QSharedPointer<MetaRegister> componentRegister)
    const
{
    return componentRegister->name_ + QLatin1Char('_') + ADDRESS_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getRegisterDataName()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getRegisterDataName(QSharedPointer<MetaRegister> componentRegister) const
{
    return componentRegister->name_ + DATA_STRING;
}
