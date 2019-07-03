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

#include <QFile>
#include <QRegularExpression>

namespace
{
    QString const MODULENAME_TEMPLATE = QLatin1String("<moduleName>");
    QString const PARAMETERS_TEMPLATE = QLatin1String("<parameters>");
    QString const PORTS_TEMPLATE = QLatin1String("<ports>");

    QString const LOCALPARAMETERS_TEMPLATE = QLatin1String("<localParameters>");
    QString const LOGICREGISTERS_TEMPLATE = QLatin1String("<logicRegisters>");
    
    QString const CONTROLREGISTERSFFS_TEMPLATE = QLatin1String("<controlRegisterFFs>");
    QString const STATUSREGISTERSFFS_TEMPLATE = QLatin1String("<statusRegisterFFs>");

    QString const READLOGICCOMB_TEMPLATE = QLatin1String("<readLogicComb>");
    QString const CONSTRUCTDATACOMB_TEMPLATE = QLatin1String("<constructDataComb>");
    QString const DRIVEOUTPUTSCOMB_TEMPLATE = QLatin1String("<driveOutputsComb>");

    QString const ONELINE = QLatin1String("\n");
    QString const TWOLINE = ONELINE + ONELINE;
    QString const ONELINECOMBINATION = QLatin1String(",") + ONELINE;
    QString const TWOLINECOMBINATION = ONELINECOMBINATION + ONELINE;
    QString const LINEEND = QLatin1Char(';');

    QString const WHITESPACEPATTERN = QLatin1String("\\s");
    QString const SPACEPATTERN = QLatin1String("^ +");
    QString const TABPATTERN = QLatin1String("^\\t");

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

    QFile templateFile("./Plugins/VerilogGenerator/ComponentVerilogWriter/verilogRegister.template");
    QString registerTemplate("");
    if (templateFile.open(QIODevice::ReadOnly))
    {
        QTextStream templateStream(&templateFile);
        registerTemplate = templateStream.readAll();
        templateFile.close();
    }
    else
    {
        return;
    }

    createModuleName(registerTemplate);

    createParameters(registerTemplate);
    
    createPortDeclarations(registerTemplate);
    
    createLocalParameters(registerTemplate);
    createRegisterData(registerTemplate);

    createControlRegisters(registerTemplate);
    createStatusRegisters(registerTemplate);

    createReadLogicComb(registerTemplate);
    createConstructComb(registerTemplate);
    createDriveOutputsComb(registerTemplate);

    outputStream << registerTemplate;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::nothingToWrite
//-----------------------------------------------------------------------------
bool ComponentRegisterVerilogWriter::nothingToWrite() const
{
    return component_.isNull() || component_->getRegisters()->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createModuleName()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createModuleName(QString& output) const
{
    output.replace(MODULENAME_TEMPLATE, registerModuleName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createParameters(QString& output) const
{
    QString expressionPattern = WHITESPACEPATTERN + PARAMETERS_TEMPLATE;
    QString parameterPattern = getPatternInTemplate(expressionPattern, output);
    if (parameterPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(parameterPattern);

    QStringList parameters;
    parameters.append(createMainParameters(indentation));
    parameters.append(createRegisterParameters(indentation));

    QString completeParameters = parameters.join(TWOLINECOMBINATION);
    output.replace(parameterPattern, completeParameters);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createMainParameters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createMainParameters(QString const& indentation) const
{
    QStringList mainParameters;

    mainParameters.append(createParameter(indentation, PARAMETER_STRING, MAINDATAWIDTH_STRING, dataWidth_));
    mainParameters.append(createParameter(indentation, PARAMETER_STRING, MAINADDRESSWIDTH_STRING, addressWidth_));

    return mainParameters.join(ONELINECOMBINATION);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createRegisterParameters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createRegisterParameters(QString const& indentation) const
{
    QStringList registerParameters;
    for (auto componentRegister : *component_->getRegisters())
    {
        registerParameters.append(getSingleRegisterParameters(indentation, componentRegister));
    }

    return registerParameters.join(TWOLINECOMBINATION);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getPatternInTemplate()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getPatternInTemplate(QString const& expressionPattern,
    QString const& templateContents) const
{
    QRegularExpression registerParameterExpression(expressionPattern);
    QRegularExpressionMatch registerParameterMatch = registerParameterExpression.match(templateContents);
    if (registerParameterMatch.hasMatch())
    {
        return registerParameterMatch.captured();
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getIndentation()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getIndentation(QString const& expressionPattern) const
{
    int indentationCount = 0;

    QRegularExpression spaceExpression(SPACEPATTERN);
    QRegularExpressionMatch spaceMatch = spaceExpression.match(expressionPattern);
    if (spaceMatch.hasMatch())
    {
        indentationCount = spaceMatch.captured().size();
    }
    else
    {
        QRegularExpression tabExpression(TABPATTERN);
        QRegularExpressionMatch tabMatch = tabExpression.match(expressionPattern);
        if (tabMatch.hasMatch())
        {
            indentationCount = tabMatch.captured().size() * 4;
        }
    }

    QString indendation = QLatin1String(" ");
    indendation = indendation.repeated(indentationCount);

    return indendation;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getSingleRegisterParameters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getSingleRegisterParameters(QString const& indentation,
    QSharedPointer<MetaRegister> componentRegister) const
{
    QStringList registerParameters;

    registerParameters.append(createParameter(
        indentation, PARAMETER_STRING, getRegisterSize(componentRegister), componentRegister->size_));
    registerParameters.append(createParameter(
        indentation, PARAMETER_STRING, getRegisterOffset(componentRegister), componentRegister->offset_));

    if (!componentRegister->fields_.isEmpty())
    {
        registerParameters.append(createRegisterFieldParameters(indentation, componentRegister));
    }

    return registerParameters.join(ONELINECOMBINATION);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createParameter()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createParameter(QString const& indentation, QString const& parameterType,
    QString const& parameterName, QString const& parameterValue, bool endLine) const
{
    QString parameterDeclaration = indentation + parameterType.leftJustified(39) + parameterName.leftJustified(16)
        + QLatin1String(" = ") + parameterValue;

    if (endLine)
    {
        parameterDeclaration += LINEEND;
    }

    return parameterDeclaration;
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
// Function: ComponentRegisterVerilogWriter::getRegisterFieldParameters()
//-----------------------------------------------------------------------------
QStringList ComponentRegisterVerilogWriter::createRegisterFieldParameters(QString const& indentation,
    QSharedPointer<MetaRegister> componentRegister) const
{
    QStringList fieldParameters;

    for (int i = 0; i < componentRegister->fields_.size(); ++i)
    {
        MetaField const& field = componentRegister->fields_.at(i);
        QString fieldName = getCombinedRegisterFieldName(componentRegister, field);

        fieldParameters.append(
            createParameter(indentation, PARAMETER_STRING, getFieldBitOffset(fieldName), field.offset_));
        fieldParameters.append(
            createParameter(indentation, PARAMETER_STRING, getFieldWidth(fieldName), field.width_));
    }

    return fieldParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createPortDeclarations()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createPortDeclarations(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + PORTS_TEMPLATE;
    QString portPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (portPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(portPattern);
    QStringList ports;

    ports.append(createMainPorts(indentation));

    for (auto componentRegister : *component_->getRegisters())
    {
        QStringList registerPorts;

        for (auto registerField : componentRegister->fields_)
        {
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

            registerPorts.append(createPort(indentation, portType, portName, portSize));
        }

        ports.append(registerPorts.join(ONELINECOMBINATION));
    }

    registerTemplate.replace(portPattern, ports.join(TWOLINECOMBINATION));
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createMainPorts()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createMainPorts(QString const& indentation) const
{
    QStringList mainPorts;
    mainPorts.append(createPort(indentation, INPUT_STRING, CLK_STRING));
    mainPorts.append(createPort(indentation, INPUT_STRING, RSTN_STRING));
    mainPorts.append(createPort(indentation, INPUT_STRING, ADDRESS_STRING, MAINADDRESSWIDTH_STRING));
    mainPorts.append(createPort(indentation, INPUT_STRING, WDATA_STRING, MAINDATAWIDTH_STRING));
    mainPorts.append(createPort(indentation, INPUT_STRING, WRITENOTREAD_STRING));
    mainPorts.append(createPort(indentation, OUTPUT_STRING, RDATA_STRING, MAINDATAWIDTH_STRING));

    return mainPorts.join(ONELINECOMBINATION);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createPort()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createPort(QString const& indentation, QString const& portType,
    QString const& portName, QString const& portSize) const
{
    QString portDeclaration = indentation + QLatin1String("<type> ") + portName;

    QString widthDeclaration = "";
    if (!portSize.isEmpty())
    {
        widthDeclaration = QLatin1Char('[') + portSize + QLatin1String("-1:0]");
    }

    QString portDefinition =
        portType.leftJustified(6) + QLatin1Char(' ') + LOGIC_STRING + QLatin1Char(' ') + widthDeclaration;

    portDeclaration.replace(QLatin1String("<type>"), portDefinition.leftJustified(38));

    return portDeclaration;
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
// Function: ComponentRegisterVerilogWriter::additionalIndentation()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::additionalIndentation(QString const& indentation, int const& depth) const
{
    QString indent = indentation;
    if (indent.isEmpty())
    {
        indent = "    ";
    }

    return indent.repeated(depth);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createLocalParameters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createLocalParameters(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + LOCALPARAMETERS_TEMPLATE;
    QString localParameterPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (localParameterPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(localParameterPattern);

    QStringList localParameters;
    for (auto componentRegister : *component_->getRegisters())
    {
        QStringList registerParameters;

        QString parameterName = getRegisterAddressName(componentRegister);
        QString registerAddress =
            getValueInBaseFormat(componentRegister->offset_, addressWidth_, addressWidth_.toULongLong(), 2);

        registerParameters.append(
            createParameter(indentation, LOCALPARAMETER_STRING, parameterName, registerAddress, true));

        registerParameters.append(
            createLocalFieldParameters(indentation, componentRegister).join(ONELINE));

        localParameters.append(registerParameters.join(ONELINE));
    }

    registerTemplate.replace(localParameterPattern, localParameters.join(TWOLINE));
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
// Function: ComponentRegisterVerilogWriter::createLocalFieldParameters()
//-----------------------------------------------------------------------------
QStringList ComponentRegisterVerilogWriter::createLocalFieldParameters(QString const& indentation,
    QSharedPointer<MetaRegister> componentRegister) const
{
    QStringList fieldParameters;

    for (auto registerField : componentRegister->fields_)
    {
        QString combiName = getFieldResetValueName(componentRegister, registerField);
        QString resetValue = registerField.resetValue_;
        if (!resetValue.contains(QLatin1Char('\'')))
        {
            resetValue = getValueInBaseFormat(resetValue, registerField.width_, registerField.widthInt_, 16);
        }

        fieldParameters.append(createParameter(indentation, LOCALPARAMETER_STRING, combiName, resetValue, true));
    }

    return fieldParameters;
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
// Function: ComponentRegisterVerilogWriter::createRegisterData()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createRegisterData(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + LOGICREGISTERS_TEMPLATE;
    QString logicRegistersPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (logicRegistersPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(logicRegistersPattern);

    QStringList fieldLogicalData;
    QStringList registerNames;
    for (auto componentRegister : *component_->getRegisters())
    {
        registerNames.append(getRegisterDataName(componentRegister));

        for (auto registerField : componentRegister->fields_)
        {
            QString fieldName = getCombinedRegisterFieldName(componentRegister, registerField);
            QString fieldWidth = getFieldWidth(fieldName);

            fieldLogicalData.append(
                createItemData(indentation, fieldWidth, getFieldRegisterName(componentRegister, registerField)));
        }
    }

    QStringList finalLogicalData;
    finalLogicalData.append(fieldLogicalData.join(ONELINE));
    finalLogicalData.append(
        createItemData(indentation, MAINDATAWIDTH_STRING, registerNames.join(QLatin1String(", "))));

    QString combinedData = finalLogicalData.join(TWOLINE);
    registerTemplate.replace(logicRegistersPattern, combinedData);
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
// Function: ComponentRegisterVerilogWriter::()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createItemData(QString const& indentation, QString const& width,
    QString const& dataName) const
{
    QString dataRange = LOGIC_STRING + QLatin1String(" [") + width + QLatin1String("-1:0]");
    return indentation + dataRange.leftJustified(39) + dataName + LINEEND;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createControlRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createControlRegisters(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + CONTROLREGISTERSFFS_TEMPLATE;
    QString controlFFsPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (controlFFsPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(controlFFsPattern);

    int additionalIndentationCount = 0;

    QStringList controlFFs;
    controlFFs.append(createFFLine(indentation, CONTROLREGISTERS_STRING));
    controlFFs.append(createBeginLine(indentation, additionalIndentationCount));
    controlFFs.append(createResetCondition(indentation, additionalIndentationCount, true));
    controlFFs.append(createWriteNotReadCondition(indentation, additionalIndentationCount));
    controlFFs.append(createEndLine(indentation, additionalIndentationCount));

    QString finishedlControlFFs = controlFFs.join(ONELINE);
    registerTemplate.replace(controlFFsPattern, finishedlControlFFs);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createFFLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createFFLine(QString const& indentation, QString const& lineName) const
{
    return indentation + ALWAYS_FF_STRING + QLatin1String("( ") + POSEDGE_STRING + QLatin1Char(' ') + CLK_STRING +
        QLatin1String(" or ") + NEGEDGE_STRING + QLatin1Char(' ') + RSTN_STRING + QLatin1String(" ) ") + lineName +
        QLatin1Char(':');
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createBeginLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createBeginLine(QString const& indentation, int& indentationCount) const
{
    QString beginLine = indentation + additionalIndentation(indentation, indentationCount) + BEGIN_STRING;
    indentationCount++;

    return beginLine;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createEndLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createEndLine(QString const& indentation, int& indentationCount) const
{
    indentationCount--;
    return indentation + additionalIndentation(indentation, indentationCount) + END_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createResetCondition()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createResetCondition(QString const& indentation, int& indentationCount,
    bool isControl) const
{
    QStringList resetCondition;
    resetCondition.append(indentation + additionalIndentation(indentation, indentationCount) + IF_STRING +
        QLatin1String(" ( ") + RSTN_STRING + QLatin1Char(' ') + EQUALCHECK_STRING + QLatin1Char(' ') +
        ZEROBITS_STRING + QLatin1String(" )"));

    indentationCount++;

    resetCondition.append(createBeginLine(indentation, indentationCount));
    resetCondition.append(createResetRegisters(indentation, indentationCount, isControl));
    resetCondition.append(createEndLine(indentation, indentationCount));

    indentationCount--;

    return resetCondition.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createResetRegisters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createResetRegisters(QString const& indentation, int& indentationCount,
    bool isControl) const
{
    QStringList resetRegisters;

    for (auto componentRegister : *component_->getRegisters())
    {
        for (auto field : componentRegister->fields_)
        {
            QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
            QString fieldResetName = getFieldResetValueName(componentRegister, field);

            if ((isControl && fieldPortIsOutput(field.access_)) || (!isControl && fieldPortIsInput(field.access_)))
            {
                resetRegisters.append(indentation + additionalIndentation(indentation, indentationCount) +
                    fieldRegisterName + QLatin1String(" = ") + fieldResetName + LINEEND);
            }
        }
    }

    return resetRegisters.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createWriteNotReadCondition()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createWriteNotReadCondition(QString const& indentation,
    int& indentationCount) const
{
    QStringList writeNotReadCondition;

    writeNotReadCondition.append(indentation + additionalIndentation(indentation, indentationCount) +
        ELSEIF_STRING + QLatin1String(" ( ") + WRITENOTREAD_STRING + QLatin1String(" )"));

    indentationCount++;

    writeNotReadCondition.append(createBeginLine(indentation, indentationCount));
    writeNotReadCondition.append(createWriteNotReadRegisters(indentation, indentationCount));
    writeNotReadCondition.append(createEndLine(indentation, indentationCount));
    
    indentationCount--;

    return writeNotReadCondition.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createWriteNotReadRegisters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createWriteNotReadRegisters(QString const& indentation,
    int& indentationCount) const
{
    QStringList writeNotReadRegisters;

    writeNotReadRegisters.append(createCaseStartLine(indentation, indentationCount));

    for (auto componentRegister : *component_->getRegisters())
    {
        QVector<MetaField> outputFields = getOutputFields(componentRegister);
        if (!outputFields.isEmpty())
        {
            writeNotReadRegisters.append(
                createRegisterAddressLine(indentation, indentationCount, componentRegister));
            writeNotReadRegisters.append(createBeginLine(indentation, indentationCount));

            for (auto field : outputFields)
            {
                writeNotReadRegisters.append(
                    createFieldReset(indentation, indentationCount, componentRegister, field));
            }

            writeNotReadRegisters.append(createEndLine(indentation, indentationCount));
        }
    }

    writeNotReadRegisters.append(createCaseEndLine(indentation, indentationCount));

    return writeNotReadRegisters.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createRegisterAddressLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createRegisterAddressLine(QString const& indentation,
    int& indentationCount, QSharedPointer<MetaRegister> componentRegister) const
{
    QString registerAddress = getRegisterAddressName(componentRegister);
    return indentation + additionalIndentation(indentation, indentationCount) + registerAddress + QLatin1Char(':');
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createFieldReset()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createFieldReset(QString const& indentation, int& indentationCount,
    QSharedPointer<MetaRegister> componentRegister, MetaField const& field) const
{
    QString fieldName = getCombinedRegisterFieldName(componentRegister, field);
    QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
    QString fieldOffsetParameter = getFieldBitOffset(fieldName);
    QString fieldWidthParameter = getFieldWidth(fieldName);

    QString registerData = fieldRegisterName + QLatin1String(" = ") + WDATA_STRING + QLatin1Char('[') +
        fieldOffsetParameter + QLatin1Char('+') + fieldWidthParameter + QLatin1String("-1:") +
        fieldOffsetParameter + QLatin1String("];");

    return indentation + additionalIndentation(indentation, indentationCount) + registerData;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createCaseStartLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createCaseStartLine(QString const& indentation, int& indentationCount)
    const
{
    QString caseStart = indentation + additionalIndentation(indentation, indentationCount) + CASE_STRING +
        QLatin1Char('(') + ADDRESS_STRING + QLatin1Char(')');
    indentationCount++;

    return caseStart;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createCaseEndLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createCaseEndLine(QString const& indentation, int& indentationCount) const
{
    indentationCount--;
    return indentation + additionalIndentation(indentation, indentationCount) + ENDCASE_STRING;
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
// Function: ComponentRegisterVerilogWriter::createStatusRegisters()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createStatusRegisters(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + STATUSREGISTERSFFS_TEMPLATE;
    QString statusFFsPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (statusFFsPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(statusFFsPattern);

    int additionalIndentationCount = 0;

    QStringList statusFFs;
    statusFFs.append(createFFLine(indentation, STATUSREGISTERS_STRING));
    statusFFs.append(createBeginLine(indentation, additionalIndentationCount));
    statusFFs.append(createResetCondition(indentation, additionalIndentationCount, false));
    statusFFs.append(createStatusRegisterChange(indentation, additionalIndentationCount));
    statusFFs.append(createEndLine(indentation, additionalIndentationCount));


    QString finishedStatusFFs = statusFFs.join(ONELINE);
    registerTemplate.replace(statusFFsPattern, finishedStatusFFs);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createStatusRegisterChange()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createStatusRegisterChange(QString const& indentation,
    int& indentationCount) const
{
    QStringList statusChange;
    statusChange.append(createElseString(indentation, indentationCount));

    statusChange.append(createBeginLine(indentation, indentationCount));
    statusChange.append(createStatusRegisterInput(indentation, indentationCount));
    statusChange.append(createEndLine(indentation, indentationCount));

    indentationCount--;

    return statusChange.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createElseString()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createElseString(QString const& indentation, int& indentationCount) const
{
    QString elseString = indentation + additionalIndentation(indentation, indentationCount) + ELSE_STRING;
    indentationCount++;
    return elseString;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createStatusRegisterInput()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createStatusRegisterInput(QString const& indentation,
    int& indentationCount) const
{
    QStringList statusRegisters;

    for (auto componentRegister : *component_->getRegisters())
    {
        QVector<MetaField> inputFields = getInputFields(componentRegister);
        for (auto field : inputFields)
        {
            QString fieldRegisterName = getFieldRegisterName(componentRegister, field);
            QString inputPortName = getCombinedRegisterFieldName(componentRegister, field) + QLatin1Char('_') +
                INPUT_STRING;

            QString registerData = fieldRegisterName + QLatin1String(" = ") + inputPortName + QLatin1Char(';');
            statusRegisters.append(
                indentation + additionalIndentation(indentation, indentationCount) + registerData);
        }
    }

    return statusRegisters.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldBitOffset()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldBitOffset(QString const& combinedName) const
{
    return combinedName.toUpper() + FIELDOFFSET_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::getFieldWidth()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::getFieldWidth(QString const& combinedName) const
{
    return combinedName.toUpper() + FIELDWIDTH_STRING;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createReadLogicComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createReadLogicComb(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + READLOGICCOMB_TEMPLATE;
    QString readLogicPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (readLogicPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(readLogicPattern);

    int additionalIndentationCount = 0;

    QStringList readLogic;
    readLogic.append(createCombLine(indentation, additionalIndentationCount, READLOGIC_STRING));
    readLogic.append(createBeginLine(indentation, additionalIndentationCount));
    readLogic.append(createDataZeroLine(indentation, additionalIndentationCount, RDATA_STRING));
    readLogic.append(createReadLogicRegisters(indentation, additionalIndentationCount));
    readLogic.append(createEndLine(indentation, additionalIndentationCount));

    additionalIndentationCount--;

    QString finishedReadLogic = readLogic.join(ONELINE);
    registerTemplate.replace(readLogicPattern, finishedReadLogic);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createCombLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createCombLine(QString const& indentation, int& indentationCount,
    QString const& lineName) const
{
    QString combLine = indentation + additionalIndentation(indentation, indentationCount) + ALWAYSCOMB_STRING +
        QLatin1Char(' ') + lineName + QLatin1Char(':');
    indentationCount++;

    return combLine;
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createDataZeroLine()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createDataZeroLine(QString const& indentation, int& indentationCount,
    QString const& dataName) const
{
    return indentation + additionalIndentation(indentation, indentationCount) + dataName + QLatin1String(" = 0;");
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createReadLogicRegisters()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createReadLogicRegisters(QString const& indentation, int& indentationCount)
    const
{
    QStringList readLogicRegisters;
    readLogicRegisters.append(createCaseStartLine(indentation, indentationCount));

    for (QSharedPointer<MetaRegister> currentRegister : *component_->getRegisters())
    {
        QString registerAddress = indentation + additionalIndentation(indentation, indentationCount) +
            getRegisterAddressName(currentRegister) + QLatin1Char(':');
        readLogicRegisters.append(registerAddress);

        readLogicRegisters.append(createBeginLine(indentation, indentationCount));

        QString dataAssign = indentation + additionalIndentation(indentation, indentationCount) + RDATA_STRING +
            QLatin1String(" = ") + getRegisterDataName(currentRegister) + QLatin1Char(';');
        readLogicRegisters.append(dataAssign);

        readLogicRegisters.append(createEndLine(indentation, indentationCount));
    }

    readLogicRegisters.append(createCaseEndLine(indentation, indentationCount));

    return readLogicRegisters.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createConstructComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createConstructComb(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + CONSTRUCTDATACOMB_TEMPLATE;
    QString constructPattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (constructPattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(constructPattern);

    int additionalIndentationCount = 0;

    QStringList construct;
    construct.append(createCombLine(indentation, additionalIndentationCount, CONSTRUCTDATA_STRING));
    construct.append(createBeginLine(indentation, additionalIndentationCount));

    QStringList constructRegisterData;
    constructRegisterData.append(createRegisterDataZeroLines(indentation, additionalIndentationCount));
    constructRegisterData.append(createRegisterConstructData(indentation, additionalIndentationCount));
    construct.append(constructRegisterData.join(TWOLINE));

    construct.append(createEndLine(indentation, additionalIndentationCount));

    additionalIndentationCount--;

    QString finishedConstruct = construct.join(ONELINE);
    registerTemplate.replace(constructPattern, finishedConstruct);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createRegisterDataZeroLines()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createRegisterDataZeroLines(QString const& indentation,
    int& indentationCount) const
{
    QStringList zeroLines;

    for (auto componentRegister : *component_->getRegisters())
    {
        QString registerData = indentation + additionalIndentation(indentation, indentationCount) +
            getRegisterDataName(componentRegister) + QLatin1String(" = 0;");
        zeroLines.append(registerData);
    }

    return zeroLines.join(ONELINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createRegisterConstructData()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createRegisterConstructData(QString const& indentation,
    int& indentationCount) const
{
    QStringList constructData;

    for (auto componentregister : *component_->getRegisters())
    {
        QStringList registerConstructData;
        QString registerName = getRegisterDataName(componentregister);

        for (auto field : componentregister->fields_)
        {
            QString fieldCombiName = getCombinedRegisterFieldName(componentregister, field);
            QString fieldOffset = getFieldBitOffset(fieldCombiName);
            QString fieldWidth = getFieldWidth(fieldCombiName);

            QString dataLine = registerName + QLatin1Char('[') + fieldOffset + QLatin1Char('+') + fieldWidth +
                QLatin1String("-1:") + fieldOffset + QLatin1String("] = ") + fieldCombiName + QLatin1Char(';');

            registerConstructData.append(
                indentation + additionalIndentation(indentation, indentationCount) + dataLine);
        }

        constructData.append(registerConstructData.join(ONELINE));
    }

    return constructData.join(TWOLINE);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createDriveOutputsComb()
//-----------------------------------------------------------------------------
void ComponentRegisterVerilogWriter::createDriveOutputsComb(QString& registerTemplate) const
{
    QString expressionPattern = WHITESPACEPATTERN + DRIVEOUTPUTSCOMB_TEMPLATE;
    QString drivePattern = getPatternInTemplate(expressionPattern, registerTemplate);
    if (drivePattern.isEmpty())
    {
        return;
    }

    QString indentation = getIndentation(drivePattern);

    int additionalIndentationCount = 0;

    QStringList drive;
    drive.append(createCombLine(indentation, additionalIndentationCount, DRIVEOUTPUTS_STRING));
    drive.append(createBeginLine(indentation, additionalIndentationCount));
    drive.append(createOutputControls(indentation, additionalIndentationCount));
    drive.append(createEndLine(indentation, additionalIndentationCount));

    additionalIndentationCount--;

    QString finishedDrive = drive.join(ONELINE);
    registerTemplate.replace(drivePattern, finishedDrive);
}

//-----------------------------------------------------------------------------
// Function: ComponentRegisterVerilogWriter::createOutputControls()
//-----------------------------------------------------------------------------
QString ComponentRegisterVerilogWriter::createOutputControls(QString const& indentation, int& indentationCount)
    const
{
    QStringList driveOutputs;

    for (auto componentRegister : *component_->getRegisters())
    {
        for (auto field : componentRegister->fields_)
        {
            if (fieldPortIsOutput(field.access_))
            {
                QString portName = getPortName(componentRegister, field, OUTPUT_STRING);
                QString fieldRegister = getFieldRegisterName(componentRegister, field);

                QString drive = indentation + additionalIndentation(indentation, indentationCount) + portName +
                    QLatin1String(" = ") + fieldRegister + QLatin1Char(';');
                driveOutputs.append(drive);
            }
        }
    }

    return driveOutputs.join(ONELINE);
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
