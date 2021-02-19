//-----------------------------------------------------------------------------
// File: componenteditorregisterfileitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Item representing a register file in the component editor browser tree.
//-----------------------------------------------------------------------------

#include "componenteditorregisterfileitem.h"

#include "componenteditorregisteritem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <editors/ComponentEditor/memoryMaps/SingleRegisterFileEditor.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/RegisterInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/registerfilegraphitem.h>

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>

#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem()
//-----------------------------------------------------------------------------
ComponentEditorRegisterFileItem::ComponentEditorRegisterFileItem(QSharedPointer<RegisterFile> regFile,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterFileValidator> registerFileValidator, RegisterInterface* registerInterface,
    QSharedPointer<AddressBlock> containingBlock, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
registerFile_(regFile),
visualizer_(nullptr),
registerFileItem_(nullptr),
expressionParser_(expressionParser),
registerFileValidator_(registerFileValidator),
registerInterface_(registerInterface),
containingBlock_(containingBlock)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setObjectName(tr("ComponentEditorRegFileItem"));

    for (QSharedPointer<RegisterBase> regModel : *regFile->getRegisterData())
    {
        QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
        if (reg)
        {
            QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg,
                registerFile_->getRegisterData(), model, libHandler, component, parameterFinder_,
                expressionFormatter_, referenceCounter_, expressionParser_,
                registerFileValidator_->getRegisterValidator(), registerInterface_, this));
            childItems_.append(regItem);

            connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
                regItem.data(), SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

            continue;
        }

        QSharedPointer<RegisterFile> regfile = regModel.dynamicCast<RegisterFile>();
        if(regfile)
        {
            QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(
                regfile, model, libHandler, component, parameterFinder_, expressionFormatter_, referenceCounter_,
                  expressionParser_, registerFileValidator_, registerInterface_, containingBlock_, this));
            childItems_.append(regFileItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::getTooltip() const
{
    return tr("Contains details of a single register file.");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRegisterFileItem::text() const
{
    return registerFile_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRegisterFileItem::isValid() const
{
    return registerFileValidator_->validate(registerFile_);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorAddrBlockItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::createChild( int index )
{
    QSharedPointer<RegisterBase> regmodel = registerFile_->getRegisterData()->at(index);

    QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
    if (reg)
    {
        QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(reg,
            registerFile_->getRegisterData(), model_, libHandler_, component_, parameterFinder_,
            expressionFormatter_, referenceCounter_, expressionParser_,
            registerFileValidator_->getRegisterValidator(), registerInterface_, this));

        connect(this, SIGNAL(registerNameChanged(QString const&, QString const&)),
            regItem.data(), SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        regItem->setLocked(locked_);

        if (visualizer_)
        {
            regItem->setVisualizer(visualizer_);
        }

        if (reg->getFields()->isEmpty())
        {
            QSharedPointer<Field> newField (new Field());
            reg->getFields()->append(newField);

            regItem->createChild(0);
        }

        onGraphicsChanged();

        childItems_.insert(index, regItem);
        return;
    }

    QSharedPointer<RegisterFile> regFile = regmodel.dynamicCast<RegisterFile>();
    if (regFile)
    {
        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem(new ComponentEditorRegisterFileItem(regFile,
            model_, libHandler_, component_, parameterFinder_, expressionFormatter_, referenceCounter_,
            expressionParser_, registerFileValidator_, registerInterface_, containingBlock_, this));
        regFileItem->setLocked(locked_);

        if (visualizer_)
        {
            regFileItem->setVisualizer(visualizer_);
        }
        onGraphicsChanged();

        childItems_.insert(index, regFileItem);
        return;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRegisterFileItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleRegisterFileEditor(registerInterface_, registerFile_, component_, libHandler_,
            parameterFinder_, expressionFormatter_, expressionParser_, registerFileValidator_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(registerNameChanged(QString const&, QString const&)),
            this, SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
    }

    return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorRegisterFileItem::visualizer()
{
    return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::setVisualizer()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::setVisualizer( MemoryMapsVisualizer* visualizer )
{
    visualizer_ = visualizer;

    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    registerFileItem_ = new RegisterFileGraphItem(registerFile_, expressionParser_, parentItem);    
    parentItem->addChild(registerFileItem_);    

    connect(registerFileItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()), Qt::UniqueConnection);

    // update the visualizers for field items
    for (QSharedPointer<ComponentEditorItem> item : childItems_)
    {
        QSharedPointer<ComponentEditorRegisterItem> regItem = item.dynamicCast<ComponentEditorRegisterItem>();
        if (regItem)
        {
          regItem->setVisualizer(visualizer_);
          continue;
        }

        QSharedPointer<ComponentEditorRegisterFileItem> regFileItem = item.dynamicCast<ComponentEditorRegisterFileItem>();
        if (regFileItem)
        {
          regFileItem->setVisualizer(visualizer_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorRegisterFileItem::getGraphicsItem()
{
    return registerFileItem_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::updateGraphics()
{
    if (registerFileItem_ != nullptr)
    {
        registerFileItem_->refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::onGraphicsChanged()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ComponentEditorItem::onGraphicsChanged();

    parent()->updateGraphics();//addressBlock
    parent()->parent()->updateGraphics(); //Memory Map
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRegisterFileItem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorRegisterFileItem::removeGraphicsItem()
{
    // get the graphics item for the address block.
    MemoryVisualizationItem* parentItem = static_cast<MemoryVisualizationItem*>(parent()->getGraphicsItem());
    Q_ASSERT(parentItem);

    parentItem->removeChild(registerFileItem_);
    registerFileItem_->setParent(0);

    disconnect(registerFileItem_, SIGNAL(selectEditor()), this, SLOT(onSelectRequest()));

    delete registerFileItem_;
    registerFileItem_ = nullptr;
}
