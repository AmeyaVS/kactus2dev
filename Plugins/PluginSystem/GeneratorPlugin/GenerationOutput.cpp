//-----------------------------------------------------------------------------
// File: GenerationOutput.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2019
//
// Description:
// Output of generation.
//-----------------------------------------------------------------------------

#include "GenerationOutput.h"

//-----------------------------------------------------------------------------
// Function: GenerationOutput::setName()
//-----------------------------------------------------------------------------
void GenerationOutput::setFileName(QString const& newFileName)
{
    fileName_ = newFileName;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::getName()
//-----------------------------------------------------------------------------
QString GenerationOutput::getFileName() const
{
    return fileName_;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::setVLNV()
//-----------------------------------------------------------------------------
void GenerationOutput::setVLNV(QString const& newVLNV)
{
    vlnv_ = newVLNV;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::getVLNV()
//-----------------------------------------------------------------------------
QString GenerationOutput::getVLNV() const
{
    return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::setFileContent()
//-----------------------------------------------------------------------------
void GenerationOutput::setFileContent(QString const& newFileContent)
{
    fileContent_ = newFileContent;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::getFileContent()
//-----------------------------------------------------------------------------
QString GenerationOutput::getFileContent() const
{
    return fileContent_;
}

//-----------------------------------------------------------------------------
// Function: GenerationOutput::clearFileContent()
//-----------------------------------------------------------------------------
void GenerationOutput::clearFileContent()
{
    fileContent_.clear();
}
