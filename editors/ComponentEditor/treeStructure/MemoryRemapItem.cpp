//-----------------------------------------------------------------------------
// File: MemoryRemapItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.04.2015
//
// Description:
// The item for a single memory remap in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "MemoryRemapItem.h"
#include "componenteditoraddrblockitem.h"

#include <editors/ComponentEditor/memoryMaps/SingleMemoryMapEditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::MemoryRemapItem()
//-----------------------------------------------------------------------------
MemoryRemapItem::MemoryRemapItem(QSharedPointer<MemoryMapBase> memoryRemap,
    QSharedPointer<MemoryMap> parentMemoryMap, ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<MemoryMapValidator> memoryMapBaseValidator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryRemap_(memoryRemap),
parentMemoryMap_(parentMemoryMap),
memoryBlocks_(memoryRemap->getMemoryBlocks()),
expressionParser_(expressionParser),
memoryMapValidator_(memoryMapBaseValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapItem"));

    const int blockCount = memoryBlocks_->count();
    for (int i = 0; i < blockCount; ++i)
    {
        createChild(i);
    }

    Q_ASSERT(memoryRemap_);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::~MemoryRemapItem()
//-----------------------------------------------------------------------------
MemoryRemapItem::~MemoryRemapItem()
{
    if (visualizer_)
    {
        delete visualizer_;
        visualizer_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::text()
//-----------------------------------------------------------------------------
QString MemoryRemapItem::text() const
{
    QSharedPointer<MemoryMap> transFormedMemoryRemap = memoryRemap_.dynamicCast<MemoryMap>();
    if (transFormedMemoryRemap && transFormedMemoryRemap == parentMemoryMap_)
    {
        return QString("Default");
    }
    else
    {
        return memoryRemap_->name();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::isValid()
//-----------------------------------------------------------------------------
bool MemoryRemapItem::isValid() const
{
    bool baseIsValid = memoryMapValidator_->MemoryMapBaseValidator::validate(
        memoryRemap_, parentMemoryMap_->getAddressUnitBits());

    QSharedPointer<MemoryRemap> transformedMemoryRemap = memoryRemap_.dynamicCast<MemoryRemap>();

    if (transformedMemoryRemap)
    {
        return baseIsValid && !memoryMapValidator_->remapStateIsNotValid(transformedMemoryRemap);
    }
    else
    {
        return baseIsValid && memoryMapValidator_->hasValidAddressUnitBits(parentMemoryMap_);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* MemoryRemapItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleMemoryMapEditor(component_, memoryRemap_, parentMemoryMap_, libHandler_,
            parameterFinder_, expressionFormatter_, expressionParser_, memoryMapValidator_);
        editor_->setProtection(locked_);
        
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAddressingChanged(int)),
            this, SLOT(onChildAddressingChanged(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        connect(editor_, SIGNAL(addressUnitBitsChanged()),
            this, SLOT(changeAdressUnitBitsOnAddressBlocks()), Qt::UniqueConnection);

        connect(editor_, SIGNAL(addressUnitBitsChanged()),
            this, SIGNAL(addressUnitBitsChanged()), Qt::UniqueConnection);

        connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
            editor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::getTooltip()
//-----------------------------------------------------------------------------
QString MemoryRemapItem::getTooltip() const
{
    if (memoryRemap_->name() == parentMemoryMap_->name())
    {
        return tr("The default memory map of ") + parentMemoryMap_->name() + ".";
    }

    return tr("Contains the details of a memory remap.");
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::createChild()
//-----------------------------------------------------------------------------
void MemoryRemapItem::createChild( int index )
{
	QSharedPointer<AddressBlock> addrBlock = memoryBlocks_->at(index).dynamicCast<AddressBlock>();
	if (addrBlock)
    {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem (new ComponentEditorAddrBlockItem(
            addrBlock, model_, libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_,
            expressionParser_, memoryMapValidator_->getAddressBlockValidator(), this));
		addrBlockItem->setLocked(locked_);

        int addressUnitBits = expressionParser_->parseExpression(parentMemoryMap_->getAddressUnitBits()).toInt();
        addrBlockItem->addressUnitBitsChanged(addressUnitBits);

		if (visualizer_)
        {
			addrBlockItem->setVisualizer(visualizer_);

            auto childItem = static_cast<MemoryVisualizationItem*>(addrBlockItem->getGraphicsItem());
            Q_ASSERT(childItem);

            graphItem_->addChild(childItem);
            childItem->setParent(graphItem_);

            onAddressingChanged();
		}

        connect(addrBlockItem.data(), SIGNAL(addressingChanged()),
            this, SLOT(onAddressingChanged()), Qt::UniqueConnection);

		childItems_.insert(index, addrBlockItem);
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::removeChild()
//-----------------------------------------------------------------------------
void MemoryRemapItem::removeChild(int index)
{
    ComponentEditorItem::removeChild(index);

    onAddressingChanged();
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* MemoryRemapItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::setVisualizer()
//-----------------------------------------------------------------------------
void MemoryRemapItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
	visualizer_ = visualizer;

	graphItem_ = new MemoryMapGraphItem(parentMemoryMap_, memoryRemap_, expressionParser_);
	visualizer_->addMemoryMapItem(graphItem_);
	graphItem_->updateDisplay();

    changeAdressUnitBitsOnAddressBlocks();

	// update the visualizers of address block items
	for (auto item : childItems_)
    {
		auto addrItem = item.staticCast<ComponentEditorAddrBlockItem>();
		addrItem->setVisualizer(visualizer_);

        auto childGraphicItem = static_cast<MemoryVisualizationItem*>(addrItem->getGraphicsItem());
        graphItem_->addChild(childGraphicItem);
	}

    graphItem_->redoChildLayout();

	connect(graphItem_, SIGNAL(selectEditor()),	this, SLOT(onSelectRequest()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* MemoryRemapItem::getGraphicsItem()
{
    return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::updateGraphics()
//-----------------------------------------------------------------------------
void MemoryRemapItem::updateGraphics()
{    
    if (graphItem_)
    {
        graphItem_->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void MemoryRemapItem::removeGraphicsItem()
{
    if (graphItem_)
    {
        // remove the graph item from the scene
        visualizer_->removeMemoryMapItem(graphItem_);

        disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));
        
        graphItem_->setParent(nullptr);

        // delete the graph item
        delete graphItem_;
        graphItem_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void MemoryRemapItem::onAddressingChanged()
{
    if (graphItem_ != nullptr)
    {
        graphItem_->updateDisplay();
        graphItem_->redoChildLayout();

        emit addressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void MemoryRemapItem::onChildAddressingChanged(int index)
{
    if (graphItem_ != nullptr)
    {
        auto childBlock = childItems_.at(index).dynamicCast<ComponentEditorAddrBlockItem>();

        if (childBlock)
        {
            childBlock->updateGraphics();
            childBlock->onAddressingChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryRemapItem::changeAdressUnitBitsOnAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryRemapItem::changeAdressUnitBitsOnAddressBlocks()
{
    QString addressUnitBits = parentMemoryMap_->getAddressUnitBits();

    for (QSharedPointer<ComponentEditorItem> childItem : childItems_)
    {
        QSharedPointer<ComponentEditorAddrBlockItem> castChildItem = 
            qobject_cast<QSharedPointer<ComponentEditorAddrBlockItem> >(childItem);

        int newAddressUnitBits = expressionParser_->parseExpression(addressUnitBits).toInt();
        castChildItem->addressUnitBitsChanged(newAddressUnitBits);
    }

    if (editor_)
    {
        emit assignNewAddressUnitBits(addressUnitBits);
    }
}
