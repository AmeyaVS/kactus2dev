//-----------------------------------------------------------------------------
// File: ComponentRegisterVerilogWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 06.06.2019
//
// Description:
// Class for writing component registers as a Verilog module.
//-----------------------------------------------------------------------------

#ifndef COMPONENTREGISTERVERILOGWRITER_H
#define COMPONENTREGISTERVERILOGWRITER_H

#include "../veriloggeneratorplugin_global.h"

#include <Plugins/VerilogGenerator/common/WriterGroup.h>

#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/HDLParser/MetaComponent.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QMap>

//-----------------------------------------------------------------------------
//! Class for writing component registers as a Verilog module.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT ComponentRegisterVerilogWriter : public WriterGroup
{
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component   Component containing the written registers.
     *      @param [in] moduleName  Name of the register module.
	 */
    ComponentRegisterVerilogWriter(QSharedPointer<MetaComponent> component, QString const& moduleName);

	/*!
     *  The destructor.
     */
    ~ComponentRegisterVerilogWriter() = default;

	/*!
     *  Writes the Verilog module into a text stream.
	 *
     *      @param [in] output   The output to write to.
	 */
    virtual void write(QTextStream& outputStream) const;

    // Disable copying.
    ComponentRegisterVerilogWriter(ComponentRegisterVerilogWriter const& rhs) = delete;
    ComponentRegisterVerilogWriter& operator=(ComponentRegisterVerilogWriter const& rhs) = delete;

private:

    /*!
     *  Calculate the address width.
     *
     *      @return The address width.
     */
    QString getAddressWidth() const;

     /*!
      *  Checks if the writer should write nothing.
      *
      *      @return True, if the writer has nothing to write, otherwise false.
      */
    bool nothingToWrite() const;

    /*!
     *  Creates the module declaration.
     *
     *      @param [in] output  The register template to write to.
     */
    void createModuleName(QString& output) const;

     /*!
      *  Creates the module parameter declarations.
      *
      *     @param [in] output  The register template to write to.
      */
    void createParameters(QString& output) const;

    /*!
     *  Creates the basic parameters.
     *
     *      @param [in] indentation     The parameter indentation.
     *
     *      @return The basic parameters.
     */
    QString createMainParameters(QString const& indentation) const;

    /*!
     *  Creates the register parameters.
     *
     *      @param [in] indentation     The parameter indentation.
     *
     *      @return The register parameters.
     */
    QString createRegisterParameters(QString const& indentation) const;

    /*!
     *  Get the matching row from the selected template.
     *
     *      @param [in] expressionPattern   The expression to look for in the template.
     *      @param [in] templateContents    The register template.
     *
     *      @return The matching row from the template..
     */
    QString getPatternInTemplate(QString const& expressionPattern, QString const& templateContents) const;

    /*!
     *  Get the indentation of the selected row.
     *
     *      @param [in] expressionPattern   The selected pattern row.
     *
     *      @return The indentation of the selected row.
     */
    QString getIndentation(QString const& expressionPattern) const;

    /*!
     *  Creates the parameters of a single register.
     *
     *      @param [in] indentation         The parameter indentation.
     *      @param [in] componentRegister   The selected register.
     *
     *      @return The parameters of a single register.
     */
    QString getSingleRegisterParameters(QString const& indentation, QSharedPointer<MetaRegister> componentRegister)
        const;

    /*!
     *  Creates a row for a single parameter declaration.
     *
     *      @param [in] indentation     The parameter indentation.
     *      @param [in] parameterType   Type of the parameter.
     *      @param [in] parameterName   Name of the parameter.
     *      @param [in] parameterValue  Value of the parameter.
     *      @param [in] endLine         Should the parameter end with a ;.
     *
     *      @return The created parameter row.
     */
    QString createParameter(QString const& indentation, QString const& parameterType, QString const& parameterName,
        QString const& parameterValue, bool endLine = false) const;

    /*!
     *  Get the size parameter of the selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return Size parameter of the selected register.
     */
    QString getRegisterSize(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Get the offset parameter of the selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return Offset parameter of the selected register.
     */
    QString getRegisterOffset(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Create parameters for the selected register fields.
     *
     *      @param [in] indentation         The parameter indentation.
     *      @param [in] componentRegister   The containing register.
     *
     *      @return List of field parameter rows.
     */
    QStringList createRegisterFieldParameters(QString const& indentation,
        QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Creates the port declarations.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createPortDeclarations(QString& registerTemplate) const;

    /*!
     *  Creates the standard port declarations.
     *
     *      @param [in] indentation     The parameter indentation.
     *
     *      @return The standard port declarations.
     */
    QString createMainPorts(QString const& indentation) const;

    /*!
     *  Writes a single port declarations.
     *
     *      @param [in] indentation     The parameter indentation.
     *      @param [in] portType        Type of the port.
     *      @param [in] portName        Name of the port.
     *      @param [in] portSize        Size of the port.
     *
     *      @return The created port row.
     */
    QString createPort(QString const& indentation, QString const& portType, QString const& portName,
        QString const& portSize = QLatin1String("")) const;

    /*!
     *  Checks if a field port should be an input port.
     *
     *      @param [in] fieldAccess     Access value of the field.
     */
    bool fieldPortIsInput(AccessTypes::Access const& fieldAccess) const;

    /*!
     *  Checks if a field port should be an output port.
     *
     *      @param [in] fieldAccess     Access value of the field.
     */
    bool fieldPortIsOutput(AccessTypes::Access const& fieldAccess) const;

    /*!
     *  Gets the additional indentation to use.
     *
     *      @param [in] indentation     The template indentation.
     *      @param [in] depth           Depth of the additional indentation.
     *
     *      @return The additional indentation to use.
     */
    QString additionalIndentation(QString const& indentation, int const& depth = 1) const;

    /*!
     *  Create local parameters.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createLocalParameters(QString& registerTemplate) const;

    /*!
     *  Creates the register data.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createRegisterData(QString& registerTemplate) const;

    /*!
     *  Creates item data.
     *
     *      @param [in] indentation     The template indentation.
     *      @param [in] width           Width of the item.
     *      @param [in] dataName        Name of the data.
     *
     *      @return The created item data row.
     */
    QString createItemData(QString const& indentation, QString const& width, QString const& dataName) const;

    /*!
     *  Creates local parameters of fields contained within the selected register.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] componentRegister   Register containing the fields.
     *
     *      @return List of local field parameter rows of the selected register.
     */
    QStringList createLocalFieldParameters(QString const& indentation,
        QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Creates control field registers.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createControlRegisters(QString& registerTemplate) const;

    /*!
     *  Creates the FF line.
     *
     *      @param [in] indentation     The template indentation.
     *      @param [in] lineName        Name of the FF line.
     *
     *      @return The created FF row.
     */
    QString createFFLine(QString const& indentation, QString const& lineName) const;

    /*!
     *  Creates a begin line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created begin row.
     */
    QString createBeginLine(QString const& indentation, int& indentationCount) const;

    /*!
     *  Writes an end line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created end row.
     */
    QString createEndLine(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates a reset condition.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] isControl           Checks whether the condition is for control or status registers.
     *
     *      @return The created reset condition.
     */
    QString createResetCondition(QString const& indentation, int& indentationCount, bool isControl) const;

    /*!
     *  Creates resets for registers.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] isControl           Checks whether the condition is for control or status registers.
     *
     *      @return The created register reset rows.
     */
    QString createResetRegisters(QString const& indentation, int& indentationCount, bool isControl) const;

    /*!
     *  Creates write not read condition.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created write not read condition row.
     */
    QString createWriteNotReadCondition(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates write not read actions for field registers.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created write not read register rows.
     */
    QString createWriteNotReadRegisters(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates a case start line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created case start row.
     */
    QString createCaseStartLine(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates a case end line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created case end row.
     */
    QString createCaseEndLine(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates a register address line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] componentRegister   The selected register.
     *
     *      @return The created register address row.
     */
    QString createRegisterAddressLine(QString const& indentation, int& indentationCount,
        QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Creates a register address line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] componentRegister   The selected register.
     *
     *      @return The created register address row.
     */
    QString createFieldReset(QString const& indentation, int& indentationCount,
        QSharedPointer<MetaRegister> componentRegister, MetaField const& field) const;

    /*!
     *  Gets a list of the output fields of the selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return A list of the output fields of the selected register.
     */
    QVector<MetaField> getOutputFields(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Gets a list of the input fields of the selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return A list of the input fields of the selected register.
     */
    QVector<MetaField> getInputFields(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Creates status field registers.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createStatusRegisters(QString& registerTemplate) const;

    /*!
     *  Creates the changes done to status registers.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created status register change rows.
     */
    QString createStatusRegisterChange(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates the else row.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created else row.
     */
    QString createElseString(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates a status register input.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created status register input rows.
     */
    QString createStatusRegisterInput(QString const& indentation, int& indentationCount) const;

    /*!
     *  Creates the always comb for read logic.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createReadLogicComb(QString& registerTemplate) const;

    /*!
     *  Create the comb line.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] lineName            Name of the comb line.
     *
     *      @return The created comb row.
     */
    QString createCombLine(QString const& indentation, int& indentationCount, QString const& lineName) const;

    /*!
     *  Create a line for setting data to 0.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] dataName            Name of the data.
     *
     *      @return The created data zero row.
     */
    QString createDataZeroLine(QString const& indentation, int& indentationCount, QString const& dataName) const;

    /*!
     *  Create the read logic registers.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] intendationCount    Current depth of the indentation.
     *
     *      @return The created read logic register rows.
     */
    QString createReadLogicRegisters(QString const& indentation, int& indentationCount) const;

    /*!
     *  Create the always comb for construct data.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createConstructComb(QString& registerTemplate) const;

    /*!
     *  Create the assign register data to zero lines.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] indentationCount    Current depth of the indentation.
     *
     *      @return The created register data zero rows.
     */
    QString createRegisterDataZeroLines(QString const& indentation, int& indentationCount) const;

    /*!
     *  Create the register construct data.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] indentationCount    Current depth of the indentation.
     *
     *      @return The created register construct data rows.
     */
    QString createRegisterConstructData(QString const& indentation, int& indentationCount) const;

    /*!
     *  Create the always comb for drive outputs.
     *
     *      @param [in] registerTemplate    The register template to write to.
     */
    void createDriveOutputsComb(QString& registerTemplate) const;

    /*!
     *  Create the drive outputs.
     *
     *      @param [in] indentation         The template indentation.
     *      @param [in] indentationCount    Current depth of the indentation.
     *
     *      @return The created drive output rows.
     */
    QString createOutputControls(QString const& indentation, int& indentationCount) const;

    /*!
     *  Get the data name of a selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return Data name of the selected register.
     */
    QString getRegisterDataName(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Get the address name of a selected register.
     *
     *      @param [in] componentRegister   The selected register.
     *
     *      @return Address name of the selected register.
     */
    QString getRegisterAddressName(QSharedPointer<MetaRegister> componentRegister) const;

    /*!
     *  Get the selected field port name.
     *
     *      @param [in] containingRegister  Register containing the selected field.
     *      @param [in] field               The selected field.
     *      @param [in] portType            Type of the selected port.
     *
     *      @return The port name.
     */
    QString getPortName(QSharedPointer<MetaRegister> containingRegister, MetaField const& field,
        QString const& portType) const;

    /*!
     *  Get the offset parameter of the field.
     *
     *      @param [in] combinedName    Field name combined with the containing register.
     *
     *      @return The offset parameter of the selected field.
     */
    QString getFieldBitOffset(QString const& combinedName) const;

    /*!
     *  Get the width parameter of the field.
     *
     *      @param [in] combinedName    Field name combined with the containing register.
     *
     *      @return The width parameter of the selected field.
     */
    QString getFieldWidth(QString const& combinedName) const;

    /*!
     *  Get the selected value in the selected base format.
     *
     *      @param [in] value       The current value.
     *      @param [in] bitWidth    The bit width.
     *      @param [in] widthInt    Width in integer.
     *      @param [in] base        The desired base formate.
     *
     *      @return The selected value in the selected base format.
     */
    QString getValueInBaseFormat(QString const& value, QString const& bitWidth, quint64 const& widthInt,
        int const& base) const;

    /*!
     *  Get name of the field combined with the containing register.
     *
     *      @param [in] containingRegister  Register containing the field.
     *      @param [in] field               The selected field.
     *
     *      @return The name of the field combined with the containing register.
     */
    QString getCombinedRegisterFieldName(QSharedPointer<MetaRegister> containingRegister, MetaField const& field)
        const;

    /*!
     *  Get name of the field register.
     *
     *      @param [in] containingRegister  Register containing the field.
     *      @param [in] field               The selected field.
     *
     *      @return The name of the field register.
     */
    QString getFieldRegisterName(QSharedPointer<MetaRegister> containingRegister, MetaField const& field) const;

    /*!
     *  Get name of the field reset value parameter.
     *
     *      @param [in] containingRegister  Register containing the field.
     *      @param [in] field               The selected field.
     *
     *      @return The name of the field reset value parameter.
     */
    QString getFieldResetValueName(QSharedPointer<MetaRegister> componentRegister, MetaField const& field) const;

    /*!
     *  Writes the ending clause for the module.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeModuleEnd(QTextStream& outputStream) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the register module.
    QString registerModuleName_;

    //! The component to write to registers module from.
    QSharedPointer<MetaComponent> component_;

    //! The data width.
    QString dataWidth_;

    //! The address width.
    QString addressWidth_;
};

#endif // COMPONENTREGISTERVERILOGWRITER_H
