//-----------------------------------------------------------------------------
// File: MetaComponent.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 03.02.2017
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#include "MetaComponent.h"

#include <common/ui/MessageMediator.h>

#include <IPXACTmodels/utilities/Search.h>

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

//-----------------------------------------------------------------------------
// Function: MetaComponent::MetaComponent
//-----------------------------------------------------------------------------
MetaComponent::MetaComponent(MessageMediator* messages,
    QSharedPointer<Component> component,
    QSharedPointer<View> activeView) :
    messages_(messages),
    component_(component),
    activeView_(activeView),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    moduleParameters_(new QList<QSharedPointer<Parameter> >()),
    metaParameters_(new QMap<QString,QSharedPointer<Parameter> >()),
    ports_(new QMap<QString,QSharedPointer<MetaPort> >()),
    registers_(new QVector<QSharedPointer<MetaRegister> > ()),
    fileSets_(new QList<QSharedPointer<FileSet> >()),
    moduleName_(),
    activeInstantiation_(),
    remapStates_(new QList<QSharedPointer<FormattedRemapState> >())
{
    // Try to find a component instantiation for the view.
    if (activeView_)
    {
        activeInstantiation_ = Search::findByName(activeView_->getComponentInstantiationRef(),
            *component_->getComponentInstantiations());

        if (activeInstantiation_)
        {
            // If there is a named component instantiation, its module name shall be used.
            moduleName_ = activeInstantiation_->getModuleName();
            parsesFileSets();
        }
    }

    if (moduleName_.isEmpty())
    {
        // If no module name is set, take the name from the VLNV of the component.
        moduleName_ = component_->getVlnv().getName();
    }

    // Must parse the parameters before can use them!
    parseParameters();
}

//-----------------------------------------------------------------------------
// Function:  MetaComponent::formatComponent()
//-----------------------------------------------------------------------------
void MetaComponent::formatComponent()
{
    // Initialize the parameter finders.
    QSharedPointer<MultipleParameterFinder> parameterFinder(new MultipleParameterFinder());
    QSharedPointer<ComponentParameterFinder> componentFinder(new ComponentParameterFinder(component_));
    QSharedPointer<ListParameterFinder> moduleFinder(new ListParameterFinder());
    moduleFinder->setParameterList(getModuleParameters());
    parameterFinder->addFinder(componentFinder);
    parameterFinder->addFinder(moduleFinder);

    //! The formatter for expressions.
    ExpressionFormatter formatter(parameterFinder);
    IPXactSystemVerilogParser expressionParser(parameterFinder);

    formatParameters(formatter);
    formatPorts(formatter);
    formatRegisters(formatter, expressionParser);

    parseRemapStates(formatter);
    parseMetaParameters();
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseMetaParameters()
//-----------------------------------------------------------------------------
void MetaComponent::parseMetaParameters()
{
    for (QSharedPointer<Parameter> original : *getParameters())
    {
        metaParameters_->insert(original->name(), original);
    }

    for (QSharedPointer<Parameter> pOriginal : *getModuleParameters())
    {
        QSharedPointer<ModuleParameter> original = pOriginal.dynamicCast<ModuleParameter>();
        if (original)
        {
            auto i = metaParameters_->find(original->name());
            if (i != metaParameters_->end())
            {
                metaParameters_->erase(i);
            }
            
            metaParameters_->insert(original->name(), original);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseParameters()
//-----------------------------------------------------------------------------
void MetaComponent::parseParameters()
{
    // Copy all the component parameters for the original parameters.
    for (QSharedPointer<Parameter> parameterOrig : *component_->getParameters())
    {       
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameterOrig)));
    }

    // If there is an active component instantiation, take its module parameters as well.
    if (activeInstantiation_)
    {
        for (QSharedPointer<ModuleParameter> parameterOrig : *activeInstantiation_->getModuleParameters())
        {
            moduleParameters_->append(QSharedPointer<ModuleParameter>(new ModuleParameter(*parameterOrig)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatParameters()
//-----------------------------------------------------------------------------
void MetaComponent::formatParameters(ExpressionFormatter const& formatter)
{
    sortParameters(parameters_);
    for (QSharedPointer<Parameter> parameter : *parameters_)
    {
        parameter->setValue(formatter.formatReferringExpression(parameter->getValue()));
    }

    sortParameters(moduleParameters_);
    for (QSharedPointer<Parameter> moduleParameter : *moduleParameters_)
    {
        moduleParameter->setValue(formatter.formatReferringExpression(moduleParameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::sortParameters()
//-----------------------------------------------------------------------------
void MetaComponent::sortParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters) const
{
    // Go through existing ones on the instance.
    foreach (QSharedPointer<Parameter> current, *sortParameters)
    {
        // The value of the inspected parameter.
        QString currentValue = current->getValue();

        // The start position for the second pass.
        auto startPosition = std::find_if(sortParameters->begin(), sortParameters->end(),
            [&currentValue](QSharedPointer<Parameter> referenced) { return currentValue.contains(
                referenced->getValueId()); });

        // If none found, next search starts from the beginning.
        if (startPosition == sortParameters->end())
        {
            startPosition = sortParameters->begin();
        }

        auto firstReferencing = std::find_if(startPosition, sortParameters->end(),
            [&currentValue](QSharedPointer<Parameter> referencing){ return referencing->getValue().contains(
                currentValue); });
        
        // Move to end of the list, unless referencing parameter is found.
        int target = sortParameters->size();
        if (firstReferencing != sortParameters->end())
        {
            target = sortParameters->indexOf(*firstReferencing);

            if (target > sortParameters->indexOf(current))
            {
                --target;
            }
        }

        sortParameters->removeOne(current);
        sortParameters->insert(target, current);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parsesFileSets()
//-----------------------------------------------------------------------------
void MetaComponent::parsesFileSets()
{
    for (QString const& fileSetRef : *activeInstantiation_->getFileSetReferences())
    {
        QSharedPointer<FileSet> fileSet = component_->getFileSet(fileSetRef);
        if (fileSet)
        {
            fileSets_->append(fileSet);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatPorts()
//-----------------------------------------------------------------------------
void MetaComponent::formatPorts(ExpressionFormatter const& formatter)
{
    for (QSharedPointer<Port> cport : *component_->getPorts())
    {
        QSharedPointer<MetaPort> mPort(new MetaPort);
        mPort->port_ = cport;
        mPort->arrayBounds_.first = formatter.formatReferringExpression(cport->getArrayLeft());
        mPort->arrayBounds_.second = formatter.formatReferringExpression(cport->getArrayRight());
        mPort->vectorBounds_.first = formatter.formatReferringExpression(cport->getLeftBound());
        mPort->vectorBounds_.second = formatter.formatReferringExpression(cport->getRightBound());

        ports_->insert(cport->name(), mPort);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatRegisters()
//-----------------------------------------------------------------------------
void MetaComponent::formatRegisters(ExpressionFormatter const& formatter, ExpressionParser const& parser)
{
    registers_->clear();

    for (auto const& componentMap : *component_->getMemoryMaps())
    {
        quint64 mapAUB = parser.parseExpression(componentMap->getAddressUnitBits()).toULongLong();

        for (auto const& blockBase : *componentMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> mapBlock = blockBase.dynamicCast<AddressBlock>();
            if (mapBlock)
            {

                for (auto registerBase : *mapBlock->getRegisterData())
                {
                    QSharedPointer<Register> blockRegister = registerBase.dynamicCast<Register>();
                    if (blockRegister)
                    {
                        formatSingleRegister(formatter, parser, blockRegister, mapAUB);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatSingleRegister()
//-----------------------------------------------------------------------------
void MetaComponent::formatSingleRegister(ExpressionFormatter const& formatter, ExpressionParser const& parser,
    QSharedPointer<Register> blockRegister, quint64 const& mapAUB)
{
//     mPort->arrayBounds_.first = formatter.formatReferringExpression(cport->getArrayLeft());

    QSharedPointer<MetaRegister> metaRegister(new MetaRegister());

    metaRegister->name_ = blockRegister->name();
    metaRegister->offset_ = formatter.formatReferringExpression(blockRegister->getAddressOffset());
    metaRegister->offsetInt_ = parser.parseExpression(metaRegister->offset_).toULongLong();
    metaRegister->size_ = formatter.formatReferringExpression(blockRegister->getSize());

    quint64 registerSize = parser.parseExpression(metaRegister->size_).toULongLong();
    quint64 aubModifiedSize = registerSize / mapAUB;

    metaRegister->sizeInt_ = aubModifiedSize;

    AccessTypes::Access registerAccess = blockRegister->getAccess();
    if (registerAccess == AccessTypes::ACCESS_COUNT)
    {
        registerAccess = blockRegister->getAccess();
        if (registerAccess == AccessTypes::ACCESS_COUNT)
        {
            registerAccess = AccessTypes::READ_WRITE;
        }
    }
    
    metaRegister->access_ = registerAccess;

    formatRegisterFields(formatter, parser, metaRegister, blockRegister);

    registers_->append(metaRegister);
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatRegisterFields()
//-----------------------------------------------------------------------------
void MetaComponent::formatRegisterFields(ExpressionFormatter const& formatter, ExpressionParser const& parser,
    QSharedPointer<MetaRegister> metaRegister, QSharedPointer<Register> blockRegister)
{
    for (auto registerField : *blockRegister->getFields())
    {
        MetaField newMetaField;
        newMetaField.name_ = registerField->name();
        newMetaField.offset_ = formatter.formatReferringExpression(registerField->getBitOffset());
        newMetaField.offsetInt_ = parser.parseExpression(newMetaField.offset_).toULongLong();
        newMetaField.width_ = formatter.formatReferringExpression(registerField->getBitWidth());
        newMetaField.widthInt_ = parser.parseExpression(newMetaField.width_).toULongLong();

        newMetaField.resetValue_ = QLatin1String("0");
        QSharedPointer<FieldReset> reset = getFieldResetValues(registerField);
        if (reset)
        {
            newMetaField.resetValue_ = formatter.formatReferringExpression(reset->getResetValue());
        }

        AccessTypes::Access fieldAccess = registerField->getAccess();
        if (fieldAccess == AccessTypes::ACCESS_COUNT)
        {
            fieldAccess = metaRegister->access_;
        }

        newMetaField.access_ = fieldAccess;

        metaRegister->fields_.append(newMetaField);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::getFieldResetValues()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReset> MetaComponent::getFieldResetValues(QSharedPointer<Field> field) const
{
    if (field->getResets())
    {
        for (auto reset : *field->getResets())
        {
            if (reset->getResetTypeReference().isEmpty())
            {
                return reset;
            }
        }
    }

    return QSharedPointer<FieldReset>();
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseRemapStates
//-----------------------------------------------------------------------------
void MetaComponent::parseRemapStates(ExpressionFormatter const& formatter)
{
    for (QSharedPointer<RemapState> currentState : *component_->getRemapStates())
    {
        QSharedPointer<FormattedRemapState> remapState(new FormattedRemapState);
        remapState->state_ = currentState;
        remapStates_->append(remapState);

        // Each port referred by the state must be listed.
        for (QSharedPointer<RemapPort> remapPort : *currentState->getRemapPorts())
        {
            // Pick the port name, and the value needed for it to remap state become effective.
            QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort
                (new QPair<QSharedPointer<Port>, QString>);
            parsedPort->first = component_->getPort(remapPort->getPortNameRef());
            parsedPort->second = formatter.formatReferringExpression(remapPort->getValue());

            remapState->ports_.append(parsedPort);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::getRegisters()
//-----------------------------------------------------------------------------
QSharedPointer<QVector<QSharedPointer<MetaRegister> > > MetaComponent::getRegisters()
{
    return registers_;
}
