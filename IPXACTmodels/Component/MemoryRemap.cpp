//-----------------------------------------------------------------------------
// File: MemoryRemap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryRemap element.
//-----------------------------------------------------------------------------

#include "MemoryRemap.h"

#include <IPXACTmodels/Component/ModeReference.h>

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(QString const& name /* = QString() */, QString const& remapState /* = QString() */):
MemoryMapBase(name),
remapState_(remapState)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::MemoryRemap(const MemoryRemap& other):
MemoryMapBase(other),
remapState_(other.remapState_),
memoryRemapDefinitionReference_(other.memoryRemapDefinitionReference_),
typeDefinitionsReference_(other.typeDefinitionsReference_)
{
    Utilities::copyList(modeReferences_, other.modeReferences_);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::operator=()
//-----------------------------------------------------------------------------
MemoryRemap& MemoryRemap::operator=(const MemoryRemap& other)
{
    if (this != &other)
    {
        MemoryMapBase::operator=(other);
        remapState_ = other.remapState_;
        memoryRemapDefinitionReference_ = other.memoryRemapDefinitionReference_;
        typeDefinitionsReference_ = other.typeDefinitionsReference_;

        Utilities::copyList(modeReferences_, other.modeReferences_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::~MemoryRemap()
//-----------------------------------------------------------------------------
MemoryRemap::~MemoryRemap()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::elementName()
//-----------------------------------------------------------------------------
QString MemoryRemap::elementName() const
{
    return QLatin1String("memory remap");
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getRemapState()
//-----------------------------------------------------------------------------
QString MemoryRemap::getRemapState() const
{
    return remapState_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setRemapState()
//-----------------------------------------------------------------------------
void MemoryRemap::setRemapState(QString newRemapState)
{
    remapState_ = newRemapState;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getMemoryRemapDefinitionReference()
//-----------------------------------------------------------------------------
QString MemoryRemap::getMemoryRemapDefinitionReference() const
{
    return memoryRemapDefinitionReference_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setMemoryRemapDefinitionReference()
//-----------------------------------------------------------------------------
void MemoryRemap::setMemoryRemapDefinitionReference(QString const& newDefinitionRef)
{
    memoryRemapDefinitionReference_ = newDefinitionRef;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getTypeDefinitionsReference()
//-----------------------------------------------------------------------------
QString MemoryRemap::getTypeDefinitionsReference() const
{
    return typeDefinitionsReference_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setTypeDefinitionsReference()
//-----------------------------------------------------------------------------
void MemoryRemap::setTypeDefinitionsReference(QString const& newTypeDefinitionsRef)
{
    typeDefinitionsReference_ = newTypeDefinitionsRef;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::getModeReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > MemoryRemap::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemap::setModeReferences()
//-----------------------------------------------------------------------------
void MemoryRemap::setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > newModeRefs)
{
    modeReferences_ = newModeRefs;
}
