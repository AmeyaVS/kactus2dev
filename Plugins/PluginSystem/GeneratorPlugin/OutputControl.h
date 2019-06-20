//-----------------------------------------------------------------------------
// File: OutputControl.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Helps to control output of generation.
//-----------------------------------------------------------------------------

#ifndef OUTPUTCONTROL_H
#define OUTPUTCONTROL_H

#include <QString>
#include <QMap>
#include <QSharedPointer>

#include "ViewSelection.h"

class GenerationOutput;

//-----------------------------------------------------------------------------
// Container class for file output of generation.
//-----------------------------------------------------------------------------
class OutputControl
{
public:

	//! The constructor.
    OutputControl();

	//! The destructor.
	~OutputControl();
    
    /*!
     *  Checks if the file output configuration is valid.
     *
     *      @param [out] warning   The stated reason for not accepting.
     */
    bool validSelections(QString &warning);

    /*!
     *  Sets the path for the output file for the generation.
     *
     *      @param [in] path   The path to set.
     */
    void setOutputPath(QString const& path);

    /*!
     *  Gets the output path for generation.
     *
     *      @return The path to output file.
     */
    QString getOutputPath() const;
    
    /*!
     *  Gets reference to the output file paths.
     */
    QSharedPointer<QList<QSharedPointer<GenerationOutput> > > getOutputs();
    
    /*!
     *  Tries to change the file name of an output. Will rewrite the content.
     *
     *      @param [in] index           The position of the output within the list.
     *      @param [in] newName         The would-be new name for the file.
     *
     *      @return The changed output if the change was successful.
     */
    QSharedPointer<GenerationOutput> changeFileName(int index, QString const& newName);

private:

	// Disable copying.
	OutputControl(OutputControl const& rhs);
	OutputControl& operator=(OutputControl const& rhs);

    //! The base directory for output paths.
    QString outputPath_;
    //! The directory of the component which may refer to the files.
    QString componentPath_;
    //! The potential new files.
    QSharedPointer<QList<QSharedPointer<GenerationOutput> > > outputs_;
};

#endif // OUTPUTCONTROL_H
