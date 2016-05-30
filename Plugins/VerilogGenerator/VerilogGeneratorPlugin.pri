# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ../PluginSystem/GeneratorPlugin/GeneratorConfiguration.h \
    ./veriloggeneratorplugin_global.h \
    ../PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.h \
    ./VerilogGeneratorPlugin.h \
    ./VerilogGenerator/VerilogGenerator.h \
    ../common/NameGenerationPolicy.h \
    ./common/Writer.h \
    ./common/WriterGroup.h \
    ./ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h \
    ./ComponentVerilogWriter/ComponentVerilogWriter.h \
    ./ModelParameterVerilogWriter/ModelParameterVerilogWriter.h \
    ./PortSorter/InterfaceDirectionNameSorter.h \
    ./PortSorter/PortSorter.h \
    ./PortVerilogWriter/PortVerilogWriter.h \
    ./VerilogHeaderWriter/VerilogHeaderWriter.h \
    ./VerilogWireWriter/VerilogWireWriter.h \
    ./CommentWriter/CommentWriter.h \
    ../../editors/ComponentEditor/common/ComponentParameterFinder.h \
    ../../editors/ComponentEditor/common/ExpressionFormatter.h \
    ../../editors/ComponentEditor/common/IPXactSystemVerilogParser.h \
    ../../editors/ComponentEditor/common/ParameterFinder.h \
    ../../editors/ComponentEditor/common/SystemVerilogExpressionParser.h \
    ../../designEditors/common/TopComponentParameterFinder.h \
    ./VerilogTiedValueWriter/VerilogTiedValueWriter.h \
    ../../editors/ComponentEditor/common/MultipleParameterFinder.h
SOURCES += ../PluginSystem/GeneratorPlugin/GeneratorConfiguration.cpp \
    ../PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.cpp \
    ./VerilogGeneratorPlugin.cpp \
    ./VerilogGenerator/VerilogGenerator.cpp \
    ./ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.cpp \
    ./ComponentVerilogWriter/ComponentVerilogWriter.cpp \
    ./ModelParameterVerilogWriter/ModelParameterVerilogWriter.cpp \
    ./PortSorter/InterfaceDirectionNameSorter.cpp \
    ./PortVerilogWriter/PortVerilogWriter.cpp \
    ./VerilogHeaderWriter/VerilogHeaderWriter.cpp \
    ./VerilogWireWriter/VerilogWireWriter.cpp \
    ./CommentWriter/CommentWriter.cpp \
    ../common/NameGenerationPolicy.cpp \
    ./common/WriterGroup.cpp \
    ../../editors/ComponentEditor/common/ComponentParameterFinder.cpp \
    ../../editors/ComponentEditor/common/ExpressionFormatter.cpp \
    ../../editors/ComponentEditor/common/IPXactSystemVerilogParser.cpp \
    ../../editors/ComponentEditor/common/SystemVerilogExpressionParser.cpp \
    ../../designEditors/common/TopComponentParameterFinder.cpp \
    ./VerilogTiedValueWriter/VerilogTiedValueWriter.cpp \
    ../../editors/ComponentEditor/common/MultipleParameterFinder.cpp
RESOURCES += VerilogGenerator.qrc
