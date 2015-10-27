# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./bspbuildcommand.h \
    ./buildcommand.h \
    ./kactusExtensions/ComDefinition.h \
    ./VendorExtension.h \
    ./vlnv.h \
    ./writevalueconstraint.h \
    ./XmlUtils.h \
    ./generaldeclarations.h \
    ./GenericVendorExtension.h \
    ./ipxactmodels_global.h \
    ./librarycomponent.h \
    ./Component/MasterInterface.h \
    ./Component/MirroredSlaveInterface.h \
    ./Component/PortMap.h \
    ./Component/SlaveInterface.h \
    ./masterinterface.h \
    ./memoryblockdata.h \
    ./memorymapitem.h \
    ./mirroredslaveinterface.h \
    ./modelparameter.h \
    ./PortMap.h \
    ./PortRef.h \
    ./registermodel.h \
    ./reset.h \
    ./servicetypedef.h \
    ./slaveinterface.h \
    ./subspacemap.h \
    ./swbuildcommand.h \
    ./kactusExtensions/ApiDefinition.h \
    ./kactusExtensions/ApiFunction.h \
    ./kactusExtensions/ApiFunctionParameter.h \
    ./kactusExtensions/ApiInterconnection.h \
    ./kactusExtensions/ApiInterface.h \
    ./kactusExtensions/ColumnDesc.h \
    ./kactusExtensions/ComInterconnection.h \
    ./kactusExtensions/ComInterface.h \
    ./kactusExtensions/ComProperty.h \
    ./kactusExtensions/FileDependency.h \
    ./kactusExtensions/HierApiInterconnection.h \
    ./kactusExtensions/HierComInterconnection.h \
    ./kactusExtensions/Kactus2Array.h \
    ./kactusExtensions/Kactus2Group.h \
    ./kactusExtensions/Kactus2Placeholder.h \
    ./kactusExtensions/Kactus2Position.h \
    ./kactusExtensions/Kactus2Value.h \
    ./kactusExtensions/Kactus2Vector.h \
    ./kactusExtensions/KactusAttribute.h \
    ./kactusExtensions/SWInstance.h \
    ./kactusExtensions/SWView.h \
    ./kactusExtensions/SystemView.h \
    ./validators/ParameterValidator2014.h \
    ./validators/BinaryValidator.h \
    ./validators/namevalidator.h \
    ./validators/NMTokenValidator.h \
    ./common/Array.h \
    ./common/Assertion.h \
    ./common/BooleanValue.h \
    ./common/BuildModel.h \
    ./common/CellSpecification.h \
    ./common/ClockUnit.h \
    ./common/ConfigurableElementValue.h \
    ./common/ConfigurableVLNVReference.h \
    ./common/DirectionTypes.h \
    ./common/Document.h \
    ./common/Enumeration.h \
    ./common/Extendable.h \
    ./common/FileBuilder.h \
    ./common/FileTypes.h \
    ./common/ModuleParameter.h \
    ./common/NameGroup.h \
    ./common/NameValuePair.h \
    ./common/Parameter.h \
    ./common/PartSelect.h \
    ./common/Protocol.h \
    ./common/Qualifier.h \
    ./common/Range.h \
    ./common/TransactionalTypes.h \
    ./common/Vector.h \
    ./common/CommonItemsReader.h \
    ./common/CommonItemsWriter.h \
    ./common/DocumentReader.h \
    ./common/DocumentWriter.h \
    ./common/FileBuilderReader.h \
    ./common/FileBuilderWriter.h \
    ./common/ModuleParameterReader.h \
    ./common/ModuleParameterWriter.h \
    ./common/NameGroupReader.h \
    ./common/NameGroupWriter.h \
    ./common/ParameterReader.h \
    ./common/ParameterWriter.h \
    ./common/ProtocolReader.h \
    ./common/ProtocolWriter.h \
    ./BusDefinition/BusDefinition.h \
    ./BusDefinition/BusDefinitionReader.h \
    ./BusDefinition/BusDefinitionWriter.h \
    ./AbstractionDefinition/AbstractionDefinition.h \
    ./AbstractionDefinition/PortAbstraction.h \
    ./AbstractionDefinition/TimingConstraint.h \
    ./AbstractionDefinition/TransactionalAbstraction.h \
    ./AbstractionDefinition/TransactionalPort.h \
    ./AbstractionDefinition/WireAbstraction.h \
    ./AbstractionDefinition/WireAbstractionWriter.h \
    ./AbstractionDefinition/WirePort.h \
    ./AbstractionDefinition/AbstractionDefinitionReader.h \
    ./AbstractionDefinition/AbstractionDefinitionWriter.h \
    ./AbstractionDefinition/TransactionalAbstractionReader.h \
    ./AbstractionDefinition/TransactionalAbstractionWriter.h \
    ./AbstractionDefinition/WireAbstractionReader.h \
    ./designConfiguration/AbstractorInstance.h \
    ./designConfiguration/DesignConfiguration.h \
    ./designConfiguration/InterconnectionConfiguration.h \
    ./designConfiguration/InterfaceRef.h \
    ./designConfiguration/MultipleAbstractorInstances.h \
    ./designConfiguration/ViewConfiguration.h \
    ./designConfiguration/DesignConfigurationReader.h \
    ./designConfiguration/DesignConfigurationWriter.h \
    ./Design/ActiveInterface.h \
    ./Design/AdHocConnection.h \
    ./Design/ComponentInstance.h \
    ./Design/Design.h \
    ./Design/HierInterface.h \
    ./Design/Interconnection.h \
    ./Design/MonitorInterconnection.h \
    ./Design/MonitorInterface.h \
    ./Design/PortReference.h \
    ./Design/ComponentInstanceReader.h \
    ./Design/ComponentInstanceWriter.h \
    ./Design/DesignReader.h \
    ./Design/DesignWriter.h \
    ./Component/AddressBlock.h \
    ./Component/AddressSpace.h \
    ./Component/AlternateRegister.h \
    ./Component/BuildCommand.h \
    ./Component/BusInterface.h \
    ./Component/Channel.h \
    ./Component/Choice.h \
    ./Component/Component.h \
    ./Component/ComponentGenerator.h \
    ./Component/ComponentInstantiation.h \
    ./Component/Cpu.h \
    ./Component/DesignConfigurationInstantiation.h \
    ./Component/DesignInstantiation.h \
    ./Component/EnumeratedValue.h \
    ./Component/Field.h \
    ./Component/File.h \
    ./Component/FileSet.h \
    ./Component/Function.h \
    ./Component/MemoryBlockBase.h \
    ./Component/MemoryMap.h \
    ./Component/MemoryMapBase.h \
    ./Component/MemoryRemap.h \
    ./Component/Model.h \
    ./Component/AbstractionType.h \
    ./Component/OtherClockDriver.h \
    ./Component/Port.h \
    ./Component/Register.h \
    ./Component/RegisterBase.h \
    ./Component/RegisterDefinition.h \
    ./Component/RegisterFile.h \
    ./Component/RemapPort.h \
    ./Component/RemapState.h \
    ./Component/Segment.h \
    ./Component/Transactional.h \
    ./Component/View.h \
    ./Component/Wire.h \
    ./Component/WireTypeDef.h \
    ./Component/WriteValueConstraint.h \
    ./Component/AddressBlockReader.h \
    ./Component/AddressBlockWriter.h \
    ./Component/AddressSpaceReader.h \
    ./Component/AddressSpaceWriter.h \
    ./Component/BusInterfaceReader.h \
    ./Component/BusInterfaceWriter.h \
    ./Component/ChannelReader.h \
    ./Component/ChannelWriter.h \
    ./Component/ChoiceReader.h \
    ./Component/ChoiceWriter.h \
    ./Component/ComponentGeneratorReader.h \
    ./Component/ComponentGeneratorWriter.h \
    ./Component/ComponentReader.h \
    ./Component/ComponentWriter.h \
    ./Component/CPUReader.h \
    ./Component/CPUWriter.h \
    ./Component/EnumeratedValueReader.h \
    ./Component/EnumeratedValueWriter.h \
    ./Component/FieldReader.h \
    ./Component/FieldWriter.h \
    ./Component/FileReader.h \
    ./Component/FileSetReader.h \
    ./Component/FileSetWriter.h \
    ./Component/FileWriter.h \
    ./Component/InstantiationsReader.h \
    ./Component/InstantiationsWriter.h \
    ./Component/MemoryMapBaseReader.h \
    ./Component/MemoryMapBaseWriter.h \
    ./Component/MemoryMapReader.h \
    ./Component/MemoryMapWriter.h \
    ./Component/OtherClockDriverReader.h \
    ./Component/OtherClockDriverWriter.h \
    ./Component/PortReader.h \
    ./Component/PortWriter.h \
    ./Component/RegisterReader.h \
    ./Component/RegisterWriter.h \
    ./Component/RemapStateReader.h \
    ./Component/RemapStateWriter.h \
    ./Component/ViewReader.h \
    ./Component/ViewWriter.h
SOURCES += ./bspbuildcommand.cpp \
    ./generaldeclarations.cpp \
    ./GenericVendorExtension.cpp \
    ./librarycomponent.cpp \
    ./Component/MirroredSlaveInterface.cpp \
    ./Component/PortMap.cpp \
    ./Component/SlaveInterface.cpp \
    ./swbuildcommand.cpp \
    ./vlnv.cpp \
    ./XmlUtils.cpp \
    ./kactusExtensions/ApiDefinition.cpp \
    ./kactusExtensions/ApiFunction.cpp \
    ./kactusExtensions/ApiFunctionParameter.cpp \
    ./kactusExtensions/ApiInterconnection.cpp \
    ./kactusExtensions/ApiInterface.cpp \
    ./kactusExtensions/ColumnDesc.cpp \
    ./kactusExtensions/ComInterconnection.cpp \
    ./kactusExtensions/ComInterface.cpp \
    ./kactusExtensions/ComProperty.cpp \
    ./kactusExtensions/FileDependency.cpp \
    ./kactusExtensions/HierApiInterconnection.cpp \
    ./kactusExtensions/HierComInterconnection.cpp \
    ./kactusExtensions/Kactus2Array.cpp \
    ./kactusExtensions/Kactus2Group.cpp \
    ./kactusExtensions/Kactus2Placeholder.cpp \
    ./kactusExtensions/Kactus2Position.cpp \
    ./kactusExtensions/Kactus2Value.cpp \
    ./kactusExtensions/Kactus2Vector.cpp \
    ./kactusExtensions/KactusAttribute.cpp \
    ./kactusExtensions/SWInstance.cpp \
    ./kactusExtensions/SWView.cpp \
    ./kactusExtensions/SystemView.cpp \
    ./validators/BinaryValidator.cpp \
    ./validators/namevalidator.cpp \
    ./validators/NMTokenValidator.cpp \
    ./validators/ParameterValidator2014.cpp \
    ./common/Array.cpp \
    ./common/Assertion.cpp \
    ./common/BooleanValue.cpp \
    ./common/BuildModel.cpp \
    ./common/CellSpecification.cpp \
    ./common/ClockUnit.cpp \
    ./common/CommonItemsReader.cpp \
    ./common/CommonItemsWriter.cpp \
    ./common/ConfigurableElementValue.cpp \
    ./common/ConfigurableVLNVReference.cpp \
    ./common/DirectionTypes.cpp \
    ./common/Document.cpp \
    ./common/DocumentReader.cpp \
    ./common/DocumentWriter.cpp \
    ./common/Enumeration.cpp \
    ./common/Extendable.cpp \
    ./common/FileBuilder.cpp \
    ./common/FileBuilderReader.cpp \
    ./common/FileBuilderWriter.cpp \
    ./common/FileTypes.cpp \
    ./common/ModuleParameter.cpp \
    ./common/ModuleParameterReader.cpp \
    ./common/ModuleParameterWriter.cpp \
    ./common/NameGroup.cpp \
    ./common/NameGroupReader.cpp \
    ./common/NameGroupWriter.cpp \
    ./common/NameValuePair.cpp \
    ./common/Parameter.cpp \
    ./common/ParameterReader.cpp \
    ./common/ParameterWriter.cpp \
    ./common/PartSelect.cpp \
    ./common/Protocol.cpp \
    ./common/ProtocolReader.cpp \
    ./common/ProtocolWriter.cpp \
    ./common/Qualifier.cpp \
    ./common/Range.cpp \
    ./common/TransactionalTypes.cpp \
    ./common/Vector.cpp \
    ./BusDefinition/BusDefinition.cpp \
    ./BusDefinition/BusDefinitionReader.cpp \
    ./BusDefinition/BusDefinitionWriter.cpp \
    ./AbstractionDefinition/AbstractionDefinition.cpp \
    ./AbstractionDefinition/AbstractionDefinitionReader.cpp \
    ./AbstractionDefinition/AbstractionDefinitionWriter.cpp \
    ./AbstractionDefinition/PortAbstraction.cpp \
    ./AbstractionDefinition/TimingConstraint.cpp \
    ./AbstractionDefinition/TransactionalAbstraction.cpp \
    ./AbstractionDefinition/TransactionalAbstractionReader.cpp \
    ./AbstractionDefinition/TransactionalAbstractionWriter.cpp \
    ./AbstractionDefinition/TransactionalPort.cpp \
    ./AbstractionDefinition/WireAbstraction.cpp \
    ./AbstractionDefinition/WireAbstractionReader.cpp \
    ./AbstractionDefinition/WireAbstractionWriter.cpp \
    ./AbstractionDefinition/WirePort.cpp \
    ./designConfiguration/AbstractorInstance.cpp \
    ./designConfiguration/DesignConfiguration.cpp \
    ./designConfiguration/DesignConfigurationReader.cpp \
    ./designConfiguration/DesignConfigurationWriter.cpp \
    ./designConfiguration/InterconnectionConfiguration.cpp \
    ./designConfiguration/InterfaceRef.cpp \
    ./designConfiguration/MultipleAbstractorInstances.cpp \
    ./designConfiguration/ViewConfiguration.cpp \
    ./Design/ActiveInterface.cpp \
    ./Design/AdHocConnection.cpp \
    ./Design/ComponentInstance.cpp \
    ./Design/ComponentInstanceReader.cpp \
    ./Design/ComponentInstanceWriter.cpp \
    ./Design/Design.cpp \
    ./Design/DesignReader.cpp \
    ./Design/DesignWriter.cpp \
    ./Design/HierInterface.cpp \
    ./Design/Interconnection.cpp \
    ./Design/MonitorInterconnection.cpp \
    ./Design/MonitorInterface.cpp \
    ./Design/PortReference.cpp \
    ./Component/AddressBlock.cpp \
    ./Component/AddressBlockReader.cpp \
    ./Component/AddressBlockWriter.cpp \
    ./Component/AddressSpace.cpp \
    ./Component/AddressSpaceReader.cpp \
    ./Component/AddressSpaceWriter.cpp \
    ./Component/AlternateRegister.cpp \
    ./Component/BuildCommand.cpp \
    ./Component/BusInterface.cpp \
    ./Component/BusInterfaceReader.cpp \
    ./Component/BusInterfaceWriter.cpp \
    ./Component/Channel.cpp \
    ./Component/ChannelReader.cpp \
    ./Component/ChannelWriter.cpp \
    ./Component/Choice.cpp \
    ./Component/ChoiceReader.cpp \
    ./Component/ChoiceWriter.cpp \
    ./Component/Component.cpp \
    ./Component/ComponentGenerator.cpp \
    ./Component/ComponentGeneratorReader.cpp \
    ./Component/ComponentGeneratorWriter.cpp \
    ./Component/ComponentInstantiation.cpp \
    ./Component/ComponentReader.cpp \
    ./Component/ComponentWriter.cpp \
    ./Component/Cpu.cpp \
    ./Component/CPUReader.cpp \
    ./Component/CPUWriter.cpp \
    ./Component/DesignConfigurationInstantiation.cpp \
    ./Component/DesignInstantiation.cpp \
    ./Component/EnumeratedValue.cpp \
    ./Component/EnumeratedValueReader.cpp \
    ./Component/EnumeratedValueWriter.cpp \
    ./Component/Field.cpp \
    ./Component/FieldReader.cpp \
    ./Component/FieldWriter.cpp \
    ./Component/File.cpp \
    ./Component/FileReader.cpp \
    ./Component/FileSet.cpp \
    ./Component/FileSetReader.cpp \
    ./Component/FileSetWriter.cpp \
    ./Component/FileWriter.cpp \
    ./Component/Function.cpp \
    ./Component/InstantiationsReader.cpp \
    ./Component/InstantiationsWriter.cpp \
    ./Component/MemoryBlockBase.cpp \
    ./Component/MemoryMap.cpp \
    ./Component/MemoryMapBase.cpp \
    ./Component/MemoryMapBaseReader.cpp \
    ./Component/MemoryMapBaseWriter.cpp \
    ./Component/MemoryMapReader.cpp \
    ./Component/MemoryMapWriter.cpp \
    ./Component/MemoryRemap.cpp \
    ./Component/Model.cpp \
    ./Component/AbstractionType.cpp \
    ./Component/OtherClockDriver.cpp \
    ./Component/OtherClockDriverReader.cpp \
    ./Component/OtherClockDriverWriter.cpp \
    ./Component/Port.cpp \
    ./Component/PortReader.cpp \
    ./Component/PortWriter.cpp \
    ./Component/Register.cpp \
    ./Component/RegisterBase.cpp \
    ./Component/RegisterDefinition.cpp \
    ./Component/RegisterFile.cpp \
    ./Component/RegisterReader.cpp \
    ./Component/RegisterWriter.cpp \
    ./Component/RemapPort.cpp \
    ./Component/RemapState.cpp \
    ./Component/RemapStateReader.cpp \
    ./Component/RemapStateWriter.cpp \
    ./Component/Segment.cpp \
    ./Component/Transactional.cpp \
    ./Component/View.cpp \
    ./Component/ViewReader.cpp \
    ./Component/ViewWriter.cpp \
    ./Component/Wire.cpp \
    ./Component/WireTypeDef.cpp \
    ./Component/WriteValueConstraint.cpp \
    ./Component/MasterInterface.cpp
    ./kactusExtensions/ComDefinition.cpp
