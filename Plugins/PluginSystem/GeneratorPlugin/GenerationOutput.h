//-----------------------------------------------------------------------------
// File: GenerationOutput.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2019
//
// Description:
// Output of generation.
//-----------------------------------------------------------------------------

#ifndef GENERATIONOUTPUT_H
#define GENERATIONOUTPUT_H

#include <QString>

//-----------------------------------------------------------------------------
// Output of generation.
//-----------------------------------------------------------------------------
class GenerationOutput
{
public:

    /*!
     *  The constructor.
     */
    GenerationOutput() = default;

    /*!
     *  The destructor.
     */
    virtual ~GenerationOutput() = default;

    /*!
     *  Writes the content.
     *
     *      @param [in] outputDirectory     Directory for the generation output.
     */
    virtual void write(QString const& outputDirectory) = 0;

    /*!
     *  Finds position for body text highlight in document, if any exists.
     *
     *      @param [out] begin  The position where the highlight begins, if successful.
     *      @param [out] end    The position where the highlight ends, if successful.
     */
    virtual void getBodyHighlight(int& begin, int& end) const = 0;

    /*!
     *  Set the file name.
     *
     *      @param [in] newFileName     The new file name.
     */
    void setFileName(QString const& newFileName);

    /*!
     *  Get the file name.
     *
     *      @return The current file name.
     */
    QString getFileName() const;

    /*!
     *  Set the VLNV.
     *
     *      @param [in] newVLNV     The new VLNV.
     */
    void setVLNV(QString const& newVLNV);

    /*!
     *  Get the VLNV.
     *
     *      @return The current VLNV.
     */
    QString getVLNV() const;

    /*!
     *  Set the file content.
     *
     *      @param [in] newFileContent  New file content.
     */
    void setFileContent(QString const& newFileContent);

    /*!
     *  Get the file content.
     *
     *      @return The current file content.
     */
    QString getFileContent() const;

protected:

    /*!
     *  Clear the file content.
     */
    void clearFileContent();

private:

    //! The name for the file created from the output.
    QString fileName_;
    //! The VLNV of the component corresponding the output.
    QString vlnv_;
    //! The content of the output, ie. what will be written to the file.
    QString fileContent_;
};

#endif // GENERATIONOUTPUT_H
