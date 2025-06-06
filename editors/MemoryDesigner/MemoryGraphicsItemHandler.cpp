//-----------------------------------------------------------------------------
// File: MemoryGraphicsItemHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2017
//
// Description:
// Constructs the memory graphics items for the memory design diagram.
//-----------------------------------------------------------------------------

#include "MemoryGraphicsItemHandler.h"

#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/MemoryItem.h>

#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryCollisionItem.h>
#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::MemoryGraphicsItemHandler()
//-----------------------------------------------------------------------------
MemoryGraphicsItemHandler::MemoryGraphicsItemHandler():
QObject()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::~MemoryGraphicsItemHandler()
//-----------------------------------------------------------------------------
MemoryGraphicsItemHandler::~MemoryGraphicsItemHandler()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressSpaceSegments(bool filterSegments)
{
    filterAddressSpaceSegments_ = filterSegments;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressSpaceSegmentsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressSpaceSegmentsAreFiltered() const
{
    return filterAddressSpaceSegments_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressBlocks(bool filterBlocks)
{
    filterAddressBlocks_ = filterBlocks;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressBlocksAreFiltered() const
{
    return filterAddressBlocks_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressBlockRegisters(bool filterRegisters)
{
    filterRegisters_ = filterRegisters;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressBlockRegistersAreFiltered() const
{
    return filterRegisters_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterFields()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterFields(bool filterFields)
{
    filterFields_ = filterFields;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::fieldsAreFiltered() const
{
    return filterFields_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::filterUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::filterUnconnectedMemoryItems(bool filterUnconnected)
{
    filterUnconnectedMemoryItems_ = filterUnconnected;

    for (AddressSpaceGraphicsItem* spaceItem : spaceItems_)
    {
        filterUnconnectedMemoryItem(spaceItem);
    }

    for (MemoryMapGraphicsItem* mapItem : memoryMapItems_)
    {
        filterUnconnectedMemoryItem(mapItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::filterUnconnectedMemoryItem()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::filterUnconnectedMemoryItem(MainMemoryGraphicsItem* memoryItem)
{
    if (memoryItem->getMemoryConnections().isEmpty())
    {
        memoryItem->setVisible(!filterUnconnectedMemoryItems_);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::unconnectedMemoryItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::unconnectedMemoryItemsAreFiltered() const
{
    return filterUnconnectedMemoryItems_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::filterMemoryOverlapItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::filterMemoryOverlapItems(bool filterOverlap)
{
    filterMemoryOverlapItems_ = filterOverlap;

    for (auto addrSpace : spaceItems_)
    {
        for (auto overlapItem : addrSpace->getMemoryCollisions())
        {
            overlapItem->setVisible(!filterOverlap);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::memoryOverlapItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::memoryOverlapItemsAreFiltered() const
{
    return filterMemoryOverlapItems_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createMemoryItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createMemoryItems(QSharedPointer<ConnectivityGraph> connectionGraph,
    MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    memoryMapItems_.clear();
    spaceItems_.clear();

    QVector<QString> addressSpaceIdentifiers;
    addressSpaceIdentifiers.append(QStringLiteral("Address spaces"));

    QVector<QString> memoryMapIdentifiers;
    memoryMapIdentifiers.append(QStringLiteral("Memory maps"));

    for (auto componentInstance : connectionGraph->getInstances())
    {
        for (auto memoryItem : componentInstance->getMemories())
        {
            if (memoryItem->getType().compare(
                MemoryDesignerConstants::ADDRESSSPACE_TYPE, Qt::CaseInsensitive) == 0)
            {
                createAddressSpaceItem(
                    memoryItem, addressSpaceIdentifiers, componentInstance, spaceColumn, memoryMapColumn);
            }
            else if (memoryItem->getType().compare(
                MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                createMemoryMapItem(memoryItem, memoryMapIdentifiers, componentInstance, memoryMapColumn);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createAddressSpaceItem()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createAddressSpaceItem(QSharedPointer<MemoryItem const> spaceItem,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> containingInstance,
    MemoryColumn* spaceColumn, MemoryColumn* mapColumn)
{
    if (spaceColumn)
    {
        AddressSpaceGraphicsItem* spaceGraphicsItem = new AddressSpaceGraphicsItem(
            spaceItem, identifierChain, containingInstance, filterAddressSpaceSegments_, spaceColumn);
        spaceColumn->addItem(spaceGraphicsItem);

        spaceItems_.append(spaceGraphicsItem);

        connectGraphicsItemSignals(spaceGraphicsItem);

        if (!spaceItem->getChildItems().isEmpty())
        {
            QSharedPointer<MemoryItem> lastChild = spaceItem->getChildItems().last();
            if (lastChild->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                QVector<QString> localMapIdentifierChain = spaceGraphicsItem->getIdentifierChain();
                createMemoryMapItem(lastChild, localMapIdentifierChain, containingInstance, mapColumn);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> containingInstance,
    MemoryColumn* containingColumn)
{
    if (containingColumn)
    {
        MemoryMapGraphicsItem* mapGraphicsItem = new MemoryMapGraphicsItem(mapItem, identifierChain,
            filterAddressBlocks_, filterRegisters_, filterFields_, containingInstance, containingColumn);
        containingColumn->addItem(mapGraphicsItem);

        memoryMapItems_.append(mapGraphicsItem);

        connectGraphicsItemSignals(mapGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::connectGraphicsItemSignals()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::connectGraphicsItemSignals(MainMemoryGraphicsItem* graphicsItem)
{
    connect(graphicsItem, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createFieldOverlapItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createFieldOverlapItems()
{
    for (MemoryMapGraphicsItem* mapItem : memoryMapItems_)
    {
        mapItem->createFieldOverlapItems();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::cloneMemoryItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryGraphicsItemHandler::cloneMemoryItem(MainMemoryGraphicsItem* targetItem, MemoryColumn* containingColumn)
{
    MainMemoryGraphicsItem* clonedItem = nullptr;

    auto originalItem = getOriginalItem(targetItem);
    if (containingColumn && originalItem != nullptr)
    {
        if (auto mapItem = dynamic_cast<MemoryMapGraphicsItem const*>(originalItem); mapItem)
        {
            auto clonedMap = new MemoryMapGraphicsItem(*mapItem);
            memoryMapItems_.append(clonedMap);

            connectGraphicsItemSignals(clonedMap);
            containingColumn->addItem(clonedMap, true);

            clonedItem = clonedMap;
        }
        else if (auto spaceItem = dynamic_cast<AddressSpaceGraphicsItem const*>(originalItem); spaceItem)
        {
            auto clonedSpace = new AddressSpaceGraphicsItem(*spaceItem);
            spaceItems_.append(clonedSpace);

            connectGraphicsItemSignals(clonedSpace);
            containingColumn->addItem(clonedSpace, true);

            clonedItem = clonedSpace;
        }
    }

    if (clonedItem != nullptr)
    {
        originalItem->getClones()->append(clonedItem);
    }

    return clonedItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::getOriginalItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryGraphicsItemHandler::getOriginalItem(MainMemoryGraphicsItem* suspiciousItem)
{
    MainMemoryGraphicsItem* originalItem = suspiciousItem;

    if (originalItem != nullptr && originalItem->isOriginal() == false)
    {
        if (originalItem->type() == GraphicsItemTypes::GFX_TYPE_ADDRESS_SPACE_ITEM)
        {
            for (auto spaceItem : spaceItems_)
            {
                if (spaceItem->name() == originalItem->name() && spaceItem->isOriginal())
                {
                    originalItem = spaceItem;
                    break;
                }
            }
        }
        else if (originalItem->type() == GraphicsItemTypes::GFX_TYPE_MEMORY_ITEM)
        {
            for (auto mapItem : memoryMapItems_)
            {
                if (mapItem->name() == originalItem->name() && mapItem->isOriginal())
                {
                    originalItem = mapItem;
                    break;
                }
            }
        }
    }

    return originalItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::itemHasCloneWithBaseAddress()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::itemHasCloneWithBaseAddress(MainMemoryGraphicsItem* originalItem, quint64 const& baseAddress) const
{
    for (auto cloneItem : *originalItem->getClones())
    {
        if (cloneItem->getBaseAddress() == baseAddress)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::getClonedItemWithBaseAddress()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryGraphicsItemHandler::getClonedItemWithBaseAddress(MainMemoryGraphicsItem* originalItem, quint64 const& baseAddress)
{
    for (auto cloneItem : *originalItem->getClones())
    {
        if (cloneItem->getBaseAddress() == baseAddress)
        {
            return cloneItem;
        }
    }

    return nullptr;
}
