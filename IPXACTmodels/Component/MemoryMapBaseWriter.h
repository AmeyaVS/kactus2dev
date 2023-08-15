//-----------------------------------------------------------------------------
// File: MemoryMapBaseWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Writer class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#ifndef MemoryMapBaseWRITER_H
#define MemoryMapBaseWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class MemoryMapBase;
class MemoryMapBaseBase;

//-----------------------------------------------------------------------------
//! Writer class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapBaseWriter : public CommonItemsWriter
{
public:

    /*!
     *  The constructor.
     */
    MemoryMapBaseWriter();

    /*!
     *  The destructor.
     */
    ~MemoryMapBaseWriter();

    /*!
     *  Write a memory map to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] MemoryMapBase   The memory map to be written.
     *      @param [in] docRevision     The standard revision in use.
     */
    void writeMemoryMapBase(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, 
        Document::Revision docRevision) const;

protected:

    /*!
     *  Write the name group.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] MemoryMapBaseBase   The selected memory map base.
     *      @param [in] docRevision         The standard revision in use.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, 
        Document::Revision docRevision) const;

    /*!
     *  Write the memory blocks.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] MemoryMapBaseBase   The selected memory map base.
     */
    void writeMemoryBlocks(QXmlStreamWriter& writer, QSharedPointer<MemoryMapBase> MemoryMapBase, Document::Revision docRevision) const;

private:

    //! No copying allowed.
    MemoryMapBaseWriter(MemoryMapBaseWriter const& rhs);
    MemoryMapBaseWriter& operator=(MemoryMapBaseWriter const& rhs);
};

#endif // MemoryMapBaseWRITER_H