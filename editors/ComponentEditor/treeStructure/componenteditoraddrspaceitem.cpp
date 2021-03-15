//-----------------------------------------------------------------------------
// File: componenteditoraddrspaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single address space in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditoraddrspaceitem.h"
#include "componenteditoraddrblockitem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/addressSpaces/localMemoryMap/localmemorymapgraphitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<AddressSpaceValidator> addressSpaceValidator, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
localMemMapVisualizer_(new MemoryMapsVisualizer()),
addrSpaceVisualizer_(new AddressSpaceVisualizer(addrSpace, expressionParser)),
expressionParser_(expressionParser),
spaceValidator_(addressSpaceValidator)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorAddrSpaceItem"));

    if (addrSpace->getLocalMemoryMap())
    {
        graphItem_ = new LocalMemoryMapGraphItem(addrSpace_, addrSpace->getLocalMemoryMap(), expressionParser_);
        localMemMapVisualizer_->addMemoryMapItem(graphItem_);
        graphItem_->updateDisplay();

        if (addrSpace->getLocalMemoryMap())
        {
            for (QSharedPointer<MemoryBlockBase> block : *addrSpace->getLocalMemoryMap()->getMemoryBlocks())
            {
                // if the item is for address block then create child for it
                QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
                if (addrBlock)
                {
                    QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem
                        (new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, 
                        referenceCounter_, parameterFinder_, expressionFormatter_,expressionParser_,
                        spaceValidator_->getLocalMemoryMapValidator()->getAddressBlockValidator(), this));

                    int addressUnitBits =
                        expressionParser_->parseExpression(addrSpace_->getAddressUnitBits()).toInt();
                    addrBlockItem->addressUnitBitsChanged(addressUnitBits);

                    addrBlockItem->setVisualizer(localMemMapVisualizer_);
                    childItems_.append(addrBlockItem);
                }
            }
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem()
//-----------------------------------------------------------------------------
ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem()
{
	if (localMemMapVisualizer_)
    {
		delete localMemMapVisualizer_;
		localMemMapVisualizer_ = 0;
	}
	if (addrSpaceVisualizer_)
    {
		delete addrSpaceVisualizer_;
		addrSpaceVisualizer_ = 0;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpaceItem::text() const
{
	return addrSpace_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorAddrSpaceItem::isValid() const
{
    return spaceValidator_->validate(addrSpace_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorAddrSpaceItem::editor()
{
	if (!editor_)
    {
		editor_ = new AddressSpaceEditor(component_, libHandler_, addrSpace_, parameterFinder_, 
            expressionFormatter_, expressionParser_, spaceValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()), Qt::UniqueConnection);
		
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),	this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(QString const&)), this, SIGNAL(errorMessage(QString const&)));
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
            editor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(newAddressUnitBitsForAddressSpaceChildItems()),
            this, SLOT(changeAdressUnitBitsOnAddressBlocks()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorAddrSpaceItem::getTooltip() const
{
	return tr("Address space defines a logical address space seen by a master bus interface");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::createChild(int index)
{
    if (!graphItem_)
    {
        graphItem_ = new LocalMemoryMapGraphItem(addrSpace_, addrSpace_->getLocalMemoryMap(), expressionParser_);
        localMemMapVisualizer_->addMemoryMapItem(graphItem_);
        graphItem_->updateDisplay();
    }

    QSharedPointer<MemoryBlockBase> block = addrSpace_->getLocalMemoryMap()->getMemoryBlocks()->at(index);
	QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
	if (addrBlock)
    {
		QSharedPointer<ComponentEditorAddrBlockItem> addressBlockItem
            (new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_,
            spaceValidator_->getLocalMemoryMapValidator()->getAddressBlockValidator(), this));
		addressBlockItem->setLocked(locked_);

        int addressUnitBits = expressionParser_->parseExpression(addrSpace_->getAddressUnitBits()).toInt();
        addressBlockItem->addressUnitBitsChanged(addressUnitBits);

		if (localMemMapVisualizer_)
        {
            addressBlockItem->setVisualizer(localMemMapVisualizer_);
            graphItem_->addChild(static_cast<MemoryVisualizationItem*>(addressBlockItem->getGraphicsItem()));
            
            onAddressingChanged();
		}
		childItems_.insert(index, addressBlockItem);

        connect(addressBlockItem.data(), SIGNAL(addressingChanged()), this, SLOT(onAddressingChanged()));
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorAddrSpaceItem::getGraphicsItem()
{
	return graphItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::updateGraphics()
{
	graphItem_->updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::removeGraphicsItem()
{
    if (graphItem_)
    {
        // remove the graph item from the scene
        localMemMapVisualizer_->removeMemoryMapItem(graphItem_);

        disconnect(graphItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

        // delete the graph item
        delete graphItem_;
        graphItem_ = NULL;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorAddrSpaceItem::visualizer()
{
	return addrSpaceVisualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::onGraphicsChanged()
{
	addrSpaceVisualizer_->refresh();

    if (graphItem_ != nullptr)
    {
        graphItem_->updateDisplay();
        for (auto child: childItems_)
        {
            child->updateGraphics();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::onAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::onAddressingChanged()
{
    if (graphItem_ != nullptr)
    {
        graphItem_->redoChildLayout();

        emit addressingChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrSpaceItem::onChildAddressingChanged()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::onChildAddressingChanged(int index)
{
    if (graphItem_ != nullptr)
    {
        childItems_.at(index)->updateGraphics();

        auto childBlock = childItems_.at(index).dynamicCast<ComponentEditorAddrBlockItem>();
        if (childBlock)
        {
            childBlock->onAddressingChanged();
        }

        graphItem_->redoChildLayout();
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditoraddrspaceitem::changeAddressUnitBits()
//-----------------------------------------------------------------------------
void ComponentEditorAddrSpaceItem::changeAdressUnitBitsOnAddressBlocks()
{
    QString addressUnitBits = addrSpace_->getAddressUnitBits();

    for (QSharedPointer<ComponentEditorItem> childItem : childItems_)
    {
        QSharedPointer<ComponentEditorAddrBlockItem> castChildItem = 
            qobject_cast<QSharedPointer<ComponentEditorAddrBlockItem> >(childItem);

        int newAddressUnitBits = expressionParser_->parseExpression(addressUnitBits).toInt();
        castChildItem->addressUnitBitsChanged(newAddressUnitBits);
    }


    emit assignNewAddressUnitBits(addressUnitBits);
}
