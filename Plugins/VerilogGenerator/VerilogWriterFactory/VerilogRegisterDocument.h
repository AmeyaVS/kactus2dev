//-----------------------------------------------------------------------------
// File: VerilogRegisterDocument.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2019
//
// Description:
// Storage for Verilog register writers.
//-----------------------------------------------------------------------------

#ifndef VERILOGREGISTERDOCUMENT_H
#define VERILOGREGISTERDOCUMENT_H

#include "../veriloggeneratorplugin_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationOutput.h>
#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>

#include <QSharedPointer>

class ComponentRegisterVerilogWriter;

//-----------------------------------------------------------------------------
// Storage for Verilog register writers.
//-----------------------------------------------------------------------------
class VerilogRegisterDocument : public GenerationOutput
{
public:

    /*!
     *  The constructor.
     */
    VerilogRegisterDocument() = default;
    
    /*!
     *  The destructor.
     */
    virtual  ~VerilogRegisterDocument() = default;

    /*!
     *  Writes the content. 
     *
     *      @param [in] outputDirectory         The possible output directory.
     */
	virtual void write(QString const& outputDirectory) override;
    
    /*!
     *  Finds position for body text highlight in document, if any exists.
     *
	 *      @param [out] begin                  The position where the highlight begins, if successful.
     *      @param [out] end                    The position where the highlight ends, if successful.
     */
    virtual void getBodyHighlight(int& begin, int& end) const override;

    /*!
     *  Set the header writer.
     *
     *      @param [in] newHeaderWriter     New file header writer.
     */
    void setHeaderWriter(QSharedPointer<VerilogHeaderWriter> newHeaderWriter);

    /*!
     *  Get the register writer.
     *
     *      @return The register writer.
     */
    QSharedPointer<ComponentRegisterVerilogWriter> getRegisterWriter() const;

    /*!
     *  Set the register writer.
     *
     *      @param [in] newRegisterWriter   New register writer.
     */
    void setRegisterWriter(QSharedPointer<ComponentRegisterVerilogWriter> newRegisterWriter);

private:

    //! This will contain the found implementation, if success.
    QString implementation_;

    //! This may contain the text after the module definition, if success.
    QString postModule_;

    //! Writer for generating file header.
    QSharedPointer<VerilogHeaderWriter> headerWriter_;

    //! Writer for component registers.
    QSharedPointer<ComponentRegisterVerilogWriter> registerWriter_;
};

#endif // VERILOGREGISTERDOCUMENT_H
