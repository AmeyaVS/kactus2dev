//-----------------------------------------------------------------------------
// File: CollectionValidators.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 02.08.2024
//
// Description:
// Validators for validating top-level items inside a component.
//-----------------------------------------------------------------------------

#include "CollectionValidators.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/utilities/Search.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/validators/ViewValidator.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>

#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>

#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/Component/validators/PowerDomainValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapsValidator::MemoryMapsValidator()
//-----------------------------------------------------------------------------
MemoryMapsValidator::MemoryMapsValidator(QSharedPointer<MemoryMapValidator> mapValidator) : 
    HierarchicalValidator(),
    memMapValidator_(mapValidator)
{
    setChildValidator(mapValidator);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsValidator::validate()
//-----------------------------------------------------------------------------
bool MemoryMapsValidator::validate(MapList memoryMaps)
{
    if (auto memoryMapsAsNameGroup = CollectionValidators::itemListToNameGroupList(memoryMaps); 
        !childrenHaveUniqueNames(memoryMapsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *memoryMaps)
    {
        if (!memMapValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetsValidator::FileSetsValidator()
//-----------------------------------------------------------------------------
FileSetsValidator::FileSetsValidator(QSharedPointer<FileSetValidator> fileSetValidator):
    HierarchicalValidator(),
    fileSetValidator_(fileSetValidator)
{
    setChildValidator(fileSetValidator);
}

//-----------------------------------------------------------------------------
// Function: FileSetsValidator::validate()
//-----------------------------------------------------------------------------
bool FileSetsValidator::validate(FileSetList fileSets)
{
    if (auto fileSetsAsNameGroup = CollectionValidators::itemListToNameGroupList(fileSets);
        !childrenHaveUniqueNames(fileSetsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *fileSets)
    {
        if (!fileSetValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesValidator::AddressSpacesValidator()
//-----------------------------------------------------------------------------
AddressSpacesValidator::AddressSpacesValidator(QSharedPointer<AddressSpaceValidator> addressSpaceValidator):
    HierarchicalValidator(),
    addressSpaceValidator_(addressSpaceValidator)
{
    setChildValidator(addressSpaceValidator);
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesValidator::validate()
//-----------------------------------------------------------------------------
bool AddressSpacesValidator::validate(AddressSpaceList addressSpaces)
{
    if (auto addressSpacesAsNameGroup = CollectionValidators::itemListToNameGroupList(addressSpaces);
        !childrenHaveUniqueNames(addressSpacesAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *addressSpaces)
    {
        if (!addressSpaceValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::AllInstantiationsValidator()
//-----------------------------------------------------------------------------
AllInstantiationsValidator::AllInstantiationsValidator(QSharedPointer<InstantiationsValidator> singleInstantiationValidator, LibraryInterface* library):
    HierarchicalValidator(),
    singleInstantiationValidator_(singleInstantiationValidator),
    library_(library)
{
    setChildValidator(singleInstantiationValidator);
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::validate()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::validate(QSharedPointer<Component> component)
{
    singleInstantiationValidator_->clearChildItemValidities();
    auto instantiationsAsNameGroups = CollectionValidators::itemListToNameGroupList(component->getComponentInstantiations());
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component->getDesignInstantiations()));
    instantiationsAsNameGroups->append(*CollectionValidators::itemListToNameGroupList(component->getDesignConfigurationInstantiations()));

    // Check for name uniqueness among all instantiations
    if (!childrenHaveUniqueNames(instantiationsAsNameGroups))
    {
        return false;
    }

    return hasValidDesignRefs(component) && hasValidComponentInstantiations(component) && hasValidDesignConfigurationInstantiations(component) &&
        hasValidDesignInstantiations(component);
}

void AllInstantiationsValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Component> component, QString const& context) const
{
    // Find duplicate names among instantiations and errors in each instantiation of the given component

    QSharedPointer<QList<QSharedPointer<NameGroup> > > instantiationsAsNameGroups(new QList<QSharedPointer<NameGroup> >());

    auto componentInstantiations = CollectionValidators::itemListToNameGroupList(component->getComponentInstantiations());
    int nComponentInstantiations = componentInstantiations->size();
    instantiationsAsNameGroups->append(*componentInstantiations);

    auto designInstantiations = CollectionValidators::itemListToNameGroupList(component->getDesignInstantiations());
    int nDesignInstantiations = designInstantiations->size();
    instantiationsAsNameGroups->append(*designInstantiations);

    auto designConfigInstantiations = CollectionValidators::itemListToNameGroupList(component->getDesignConfigurationInstantiations());
    instantiationsAsNameGroups->append(*designConfigInstantiations);

    QSet<QString> occupiedNames;
    int i = 0;
    for (auto const& instantiation : *instantiationsAsNameGroups)
    {
        bool isComponent = i < nComponentInstantiations;
        bool isDesign = nComponentInstantiations <= i && i < nComponentInstantiations + nDesignInstantiations;
        bool isDesignConf = nComponentInstantiations + nDesignInstantiations <= i;
        i++;

        QString prefix;
        if (isComponent)
        {
            auto casted = instantiation.dynamicCast<ComponentInstantiation>();
            prefix = QStringLiteral("Component");
            singleInstantiationValidator_->findErrorsInComponentInstantiation(errors, casted, context, component->getRevision());
        }
        else if (isDesign)
        {
            auto casted = instantiation.dynamicCast<DesignInstantiation>();
            prefix = QStringLiteral("Design");
            singleInstantiationValidator_->findErrorsInDesignInstantiation(errors, casted, context);
        }
        else if (isDesignConf)
        {
            auto casted = instantiation.dynamicCast<DesignConfigurationInstantiation>();
            prefix = QStringLiteral("Design configuration");
            singleInstantiationValidator_->findErrorsInDesignConfigurationInstantiation(errors, casted, context);
        }

        if (occupiedNames.contains(instantiation->name()))
        {
            errors.append(QObject::tr("%1 instantiation name %2 among instantiations of %3 is not unique.")
                .arg(prefix).arg(instantiation->name()).arg(context));
        }
        else
        {
            occupiedNames.insert(instantiation->name());
        }
    }

    findErrorsInDesignRefs(errors, component, context);
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidComponentInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidComponentInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& componentInstantiation : *component->getComponentInstantiations())
    {
        if (!singleInstantiationValidator_->validateComponentInstantiation(componentInstantiation))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidDesignConfigurationInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& designConfigInstantiation : *component->getDesignConfigurationInstantiations())
    {
        if (!singleInstantiationValidator_->validateDesignConfigurationInstantiation(designConfigInstantiation))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AllInstantiationsValidator::hasValidDesignInstantiations()
//-----------------------------------------------------------------------------
bool AllInstantiationsValidator::hasValidDesignInstantiations(QSharedPointer<Component> component) const
{
    for (auto const& designInstantiation : *component->getDesignInstantiations())
    {
        if (!singleInstantiationValidator_->validateDesignInstantiation(designInstantiation))
        {
            return false;
        }
    }

    return true;
}

bool AllInstantiationsValidator::hasValidDesignRefs(QSharedPointer<Component> component)
{
    // Check for each view, in case a design configuration instantiation with a design reference and a design instantiation are referenced by the view, that
    // the design reference in the design configuration matches the design reference of the design instantiation.

    for (auto const& view : *component->getViews())
    {
        QString designConfDesignRef;
        QString designInstDesingRef;

        QSharedPointer<DesignConfigurationInstantiation> foundDesignConfInst;
        QSharedPointer<DesignInstantiation> foundDesignInst;

        if (view->getDesignConfigurationInstantiationRef().isEmpty() == false)
        {
            auto designConfInst = Search::findByName(view->getDesignConfigurationInstantiationRef(), component->getDesignConfigurationInstantiations());
            if (designConfInst != nullptr)
            {
                Q_ASSERT_X(library_, "AllInstantionsValidator::hasValidDesignRefs", "Library is null");
                auto doc = library_->getModelReadOnly(VLNV(VLNV::DESIGNCONFIGURATION, designConfInst->getDesignConfigurationReference()->toString()));
                auto designConf = doc.dynamicCast<DesignConfiguration const>();
                foundDesignConfInst = designConfInst;
                designConfDesignRef = designConf->getDesignRef().toString();
            }
        }

        if (view->getDesignInstantiationRef().isEmpty() == false)
        {
            auto designInstIt = std::find_if(component->getDesignInstantiations()->cbegin(), component->getDesignInstantiations()->cend(),
                [&view](QSharedPointer<DesignInstantiation> designInstantiation)
                {
                    return view->getDesignInstantiationRef().compare(designInstantiation->name()) == 0;
                });

            if (designInstIt != component->getDesignInstantiations()->cend())
            {
                designInstDesingRef = (*designInstIt)->getDesignReference()->toString();
                foundDesignInst = *designInstIt;
            }

            // In a view, if design config has design reference, then design reference in design instantiation must match
            if (designConfDesignRef.isEmpty() == false && designConfDesignRef.compare(designInstDesingRef) != 0)
            {
                if (foundDesignConfInst)
                    childValidator_->setChildItemValidity(foundDesignConfInst, false);
                if (foundDesignInst)
                    childValidator_->setChildItemValidity(foundDesignInst, false);
                
                return false;
            }
        }
    }

    return true;
}

void AllInstantiationsValidator::findErrorsInDesignRefs(QVector<QString>& errors, QSharedPointer<Component> component, QString const& context) const
{
    for (auto const& view : *component->getViews())
    {
        QString designConfDesignRef;
        QString designInstDesingRef;

        if (view->getDesignConfigurationInstantiationRef().isEmpty() == false)
        {
            auto designConfInst = Search::findByName(view->getDesignConfigurationInstantiationRef(), component->getDesignConfigurationInstantiations());
            if (designConfInst != nullptr)
            {   
                auto doc = library_->getModelReadOnly(VLNV(VLNV::DESIGNCONFIGURATION, designConfInst->getDesignConfigurationReference()->toString()));
                auto designConf = doc.dynamicCast<DesignConfiguration const>();
                designConfDesignRef = designConf->getDesignRef().toString();
            }
        }

        if (view->getDesignInstantiationRef().isEmpty() == false)
        {
            auto designInstIt = std::find_if(component->getDesignInstantiations()->cbegin(), component->getDesignInstantiations()->cend(),
                [&view](QSharedPointer<DesignInstantiation> designInstantiation)
                {
                    return view->getDesignInstantiationRef().compare(designInstantiation->name()) == 0;
                });

            if (designInstIt != component->getDesignInstantiations()->cend())
            {
                designInstDesingRef = (*designInstIt)->getDesignReference()->toString();
            }    
    
            // In a view, if design config has design reference, then design reference in design instantiation must match
            if (designConfDesignRef.isEmpty() == false && designConfDesignRef.compare(designInstDesingRef) != 0)
            {
                errors.append(QObject::tr("View %1: Design reference must match for design configuration referenced by design configuration instantiation and design instantiation in %2")
                    .arg(view->name()).arg(context));
            }
        }            
    }
}

//-----------------------------------------------------------------------------
// Function: ViewsValidator::ViewsValidator()
//-----------------------------------------------------------------------------
ViewsValidator::ViewsValidator(QSharedPointer<ViewValidator> viewValidator) : 
    HierarchicalValidator(),
    viewValidator_(viewValidator)
{
    setChildValidator(viewValidator);
}

//-----------------------------------------------------------------------------
// Function: ViewsValidator::validate()
//-----------------------------------------------------------------------------
bool ViewsValidator::validate(QSharedPointer<QList<QSharedPointer<View> > > views)
{
    if (auto viewsAsNameGroup = CollectionValidators::itemListToNameGroupList(views);
        !childrenHaveUniqueNames(viewsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *views)
    {
        if (!viewValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsValidator::PortsValidator()
//-----------------------------------------------------------------------------
PortsValidator::PortsValidator(QSharedPointer<PortValidator> portValidator):
    HierarchicalValidator(),
    portValidator_(portValidator)
{
    setChildValidator(portValidator);
}

//-----------------------------------------------------------------------------
// Function: PortsValidator::validate()
//-----------------------------------------------------------------------------
bool PortsValidator::validate(QSharedPointer<QList<QSharedPointer<Port> > > ports)
{
    if (auto portsAsNameGroup = CollectionValidators::itemListToNameGroupList(ports);
        !childrenHaveUniqueNames(portsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *ports)
    {
        if (!portValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesValidator::BusInterfacesValidator()
//-----------------------------------------------------------------------------
BusInterfacesValidator::BusInterfacesValidator(QSharedPointer<BusInterfaceValidator> busInterfaceValidator) :
    HierarchicalValidator(),
    busInterfaceValidator_(busInterfaceValidator)
{
    setChildValidator(busInterfaceValidator);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesValidator::validate()
//-----------------------------------------------------------------------------
bool BusInterfacesValidator::validate(QSharedPointer<QList<QSharedPointer<BusInterface> > > interfaces,
    Document::Revision docRevision)
{
    if (auto busInterfacesAsNameGroup = CollectionValidators::itemListToNameGroupList(interfaces);
        !childrenHaveUniqueNames(busInterfacesAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *interfaces)
    {
        if (!busInterfaceValidator_->validate(child, docRevision))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesValidator::IndirectInterfacesValidator()
//-----------------------------------------------------------------------------
IndirectInterfacesValidator::IndirectInterfacesValidator(QSharedPointer<IndirectInterfaceValidator> indirectInterfaceValidator) :
    HierarchicalValidator(),
    indirectInterfaceValidator_(indirectInterfaceValidator)
{
    setChildValidator(indirectInterfaceValidator);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfacesValidator::validate()
//-----------------------------------------------------------------------------
bool IndirectInterfacesValidator::validate(QSharedPointer<QList<QSharedPointer<IndirectInterface> > > indirectInterfaces)
{
    if (auto indirectInterfacesAsNameGroup = CollectionValidators::itemListToNameGroupList(indirectInterfaces);
        !childrenHaveUniqueNames(indirectInterfacesAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *indirectInterfaces)
    {
        if (!indirectInterfaceValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CPUsValidator::CPUsValidator()
//-----------------------------------------------------------------------------
CPUsValidator::CPUsValidator(QSharedPointer<CPUValidator> cpuValidator):
    cpuValidator_(cpuValidator)
{
    setChildValidator(cpuValidator);
}

//-----------------------------------------------------------------------------
// Function: CPUsValidator::validate()
//-----------------------------------------------------------------------------
bool CPUsValidator::validate(QSharedPointer<QList<QSharedPointer<Cpu> > > cpus)
{
    if (auto cpusAsNameGroup = CollectionValidators::itemListToNameGroupList(cpus);
        !childrenHaveUniqueNames(cpusAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *cpus)
    {
        if (!cpuValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsValidator::PowerDomainsValidator()
//-----------------------------------------------------------------------------
PowerDomainsValidator::PowerDomainsValidator(QSharedPointer<PowerDomainValidator> powerDomainValidator):
    HierarchicalValidator(),
    powerDomainValidator_(powerDomainValidator)
{
    setChildValidator(powerDomainValidator);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsValidator::validate()
//-----------------------------------------------------------------------------
bool PowerDomainsValidator::validate(QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains)
{
    if (auto powerDomainsAsNameGroup = CollectionValidators::itemListToNameGroupList(powerDomains);
        !childrenHaveUniqueNames(powerDomainsAsNameGroup))
    {
        return false;
    }

    for (auto const& child : *powerDomains)
    {
        if (!powerDomainValidator_->validate(child))
        {
            return false;
        }
    }

    return true;
}
