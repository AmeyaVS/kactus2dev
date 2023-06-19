//-----------------------------------------------------------------------------
// File: SingleCpuItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// The single cpu item used in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "SingleCpuItem.h"

#include <editors/ComponentEditor/cpus/SingleCpuEditor.h>

#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::SingleCpuItem()
//-----------------------------------------------------------------------------
SingleCpuItem::SingleCpuItem(QSharedPointer<Cpu> cpu, ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<CPUValidator> validator,
    ComponentEditorItem* parent) :
ComponentEditorItem(model, libHandler, component, parent),
    expressionParser_(expressionParser),
    cpu_(cpu),
    validator_(validator)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleCpuItem::getTooltip() const
{
    return tr("Specifies a CPU of the component");
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::text()
//-----------------------------------------------------------------------------
QString SingleCpuItem::text() const
{
    return cpu_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::isValid()
//-----------------------------------------------------------------------------
bool SingleCpuItem::isValid() const
{
    return validator_->validate(cpu_);
}

//-----------------------------------------------------------------------------
// Function: SingleCpuItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleCpuItem::editor()
{
    if (!editor_)
    {
        editor_ = new SingleCpuEditor(component_, cpu_, libHandler_, parameterFinder_,
            expressionParser_);
        editor_->setProtection(locked_);

        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
        connectItemEditorToReferenceCounter();
    }

    return editor_;
}