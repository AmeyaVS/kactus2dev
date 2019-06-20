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
     *  Setup the parameter list.
     */
    void setupParameters();

     /*!
      *  Checks if the writer should write nothing.
      *
      *      @return True, if the writer has nothing to write, otherwise false.
      */
    bool nothingToWrite() const;

    /*!
     *  Writes the module declaration.
     *
     *      @param [in] outputStream   The output to write to.
     */
    void writeModuleDeclaration( QTextStream& outputStream ) const;

     /*!
      *  Writes the module parameter declaration.
      *
      *      @param [in] outputStream   The output to write to.
      */
    void writeParameterDeclarations(QTextStream& outputStream) const;

    /*!
     *  Write the basic parameters.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeMainParameters(QTextStream& outputStream) const;

    /*!
     *  Write the parameters of a single register.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] componentRegister   The register being written.
     */
    void writeSingleRegisterParameters(QTextStream& outputStream, QSharedPointer<MetaRegister> componentRegister)
        const;

    /*!
     *  Write the parameters of register fields.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] componentRegister   The register being written.
     */
    void writeRegisterFieldParameters(QTextStream& outputStream, QSharedPointer<MetaRegister> componentRegister)
        const;

    /*!
     *  Writes a single parameter declaration.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] parameterType   Type of the parameter.
     *      @param [in] parameterName   Name of the parameter.
     *      @param [in] parameterValue  Value of the parameter.
     */
    void writeParameter(QTextStream& outputStream, QString const& parameterType, QString const& parameterName,
        QString const& parameterValue) const;

    /*!
     *  Writes port declarations.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writePortDeclarations(QTextStream& outputStream) const;

    /*!
     *  Writes a single port declarations.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] portType        Type of the port.
     *      @param [in] portName        Name of the port.
     *      @param [in] portSize        Size of the port.
     */
    void writePort(QTextStream& outputStream, QString const& portType, QString const& portName,
        QString const& portSize = QLatin1String("")) const;

    /*!
     *  Writes register port declarations.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeRegisterPortDeclarations(QTextStream& outputStream) const;

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
     *  Gets the indentation to use.
     *
     *      @param [in] depth   Depth of the indentation.
     *
     *      @return The indentation to use.
     */
    QString indentation(int depth = 1) const;

    /*!
     *  Writes local parameters.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeLocalParameters(QTextStream& outputStream) const;

    /*!
     *  Writes register data.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeRegisterData(QTextStream& outputStream) const;

    /*!
     *  Writes field data.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeFieldData(QTextStream& outputStream) const;

    /*!
     *  Writes item data.
     *
     *      @param [in] outputStream    The output to write to.
     *      @param [in] width           Width of the item.
     *      @param [in] dataName        Name of the data.
     */
    void writeItemData(QTextStream& outputStream, QString const& width, QString const& dataName) const;

    /*!
     *  Writes local parameters of fields contained within the selected register.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] componentRegister   Register containing the fields.
     */
    void writeLocalFieldParameters(QTextStream& outputStream, QSharedPointer<MetaRegister> componentRegister)
        const;

    /*!
     *  Writes control field register.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeControlRegisters(QTextStream& outputStream) const;

    /*!
     *  Writes the FF line.
     *
     *      @param [in] outputStream    The output to write to.
     */
    void writeFFLine(QTextStream& outputStream) const;

    /*!
     *  Writes a begin line for the FF line.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] lineName            Name of the FF.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeRegisterBeginLine(QTextStream& outputStream, QString const& lineName, int& intendationCount) const;

    /*!
     *  Writes a begin line.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeBeginLine(QTextStream& outputStream, int& indentationCount) const;

    /*!
     *  Writes an end line.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeEndLine(QTextStream& outputStream, int& indentationCount) const;

    /*!
     *  Writes a reset condition.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] isControl           Checks whether the condition is for control or status registers.
     */
    void writeResetCondition(QTextStream& outputStream, int& indentationCount, bool isControl) const;

    /*!
     *  Writes resets for registers.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     *      @param [in] isControl           Checks whether the condition is for control or status registers.
     */
    void writeResetRegisters(QTextStream& outputStream, int& indentationCount, bool isControl) const;

    /*!
     *  Writes write not read condition.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeWriteNotReadCondition(QTextStream& outputStream, int& indentationCount) const;

    /*!
     *  Writes write not read actions for field registers.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeWriteNotReadRegisters(QTextStream& outputStream, int& indentationCount) const;

    /*!
     *  Writes a case start line.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeCaseStartLine(QTextStream& outputStream, int& indentationCount) const;

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
     *  Writes status field registers.
     *
     *      @param [in] outputStream        The output to write to.
     */
    void writeStatusRegisters(QTextStream& outputStream) const;

    /*!
     *  Writes the changes done to status registers.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeStatusRegisterChange(QTextStream& outputStream, int& indentationCount) const;

    /*!
     *  Writes a status register input.
     *
     *      @param [in] outputStream        The output to write to.
     *      @param [in] intendationCount    Current depth of the indentation.
     */
    void writeStatusRegisterInput(QTextStream& outputStream, int& indentationCount) const;

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
    QString getFieldBitOffsetParameter(QString const& combinedName) const;

    /*!
     *  Get the width parameter of the field.
     *
     *      @param [in] combinedName    Field name combined with the containing register.
     *
     *      @return The width parameter of the selected field.
     */
    QString getFieldWidthParameter(QString const& combinedName) const;

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

    //! Contains the parameter names matched with the container.
    QMap<QString, QString> parameters_;

    //! The data width.
    QString dataWidth_;

    //! The address width.
    QString addressWidth_;
};

#endif // COMPONENTREGISTERVERILOGWRITER_H
