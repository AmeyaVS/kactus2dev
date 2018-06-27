//-----------------------------------------------------------------------------
// File: VHDLGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2017
//
// Description:
// Kactus2 plugin for VHDL file generation.
//-----------------------------------------------------------------------------

#include "VHDLGeneratorPlugin.h"

#include "vhdlgenerator2.h"
#include "VHDLGeneratorDialog.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <Plugins/PluginSystem/GeneratorPlugin/ViewSelection.h>
#include <Plugins/common/HDLParser/HDLCommandLineParser.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::VHDLGeneratorPlugin()
//-----------------------------------------------------------------------------
VHDLGeneratorPlugin::VHDLGeneratorPlugin() : QObject(0), utility_(0), generationSettings_()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::VHDLGeneratorPlugin()
//-----------------------------------------------------------------------------
VHDLGeneratorPlugin::~VHDLGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getName() const
{
    return "VHDL Generator";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getVersion() const
{
    return "1.2";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getDescription() const
{
    return "Generates a VHDL entity for a component or a structural description of a HW design.";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getVendor() const
{
    return tr("tut.fi");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VHDLGeneratorPlugin::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* VHDLGeneratorPlugin::getSettingsModel()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon VHDLGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/VHDLGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool VHDLGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> /*design*/,
    QSharedPointer<DesignConfiguration const> /*designConfiguration*/) const
{
    return component && component->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> /*design*/,
    QSharedPointer<DesignConfiguration> /*designConfiguration*/)
{
    utility_ = utility;

    QFileInfo targetInfo(utility_->getLibraryInterface()->getPath(component->getVlnv()));

    const bool saveToFileset = true;
    QSharedPointer<ViewSelection> viewSettings(new ViewSelection(QStringLiteral("vhdl"), saveToFileset,
        QString(), &generationSettings_, component->getViews(), component->getComponentInstantiations(),
        component->getFileSets()));

    QString suggestedPath = targetInfo.absolutePath();
    suggestedPath.append(QLatin1Char('/'));
    suggestedPath.append(findEntityName(component, viewSettings->getViewName()));
    suggestedPath.append(QStringLiteral(".vhd"));

    VHDLGeneratorDialog dialog(viewSettings, suggestedPath, utility->getParentWidget());

    if (dialog.exec() == QDialog::Accepted)
    {
        generate(component, dialog.getOutputPath(), viewSettings);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VHDLGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getCommand()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getCommand() const
{
    return QStringLiteral("generate_vhdl");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::process()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::process(QStringList const& arguments, IPluginUtility* utility)
{
    HDLCommandLineParser parser(getCommand());
    HDLCommandLineParser::ParseResults parseResult = parser.parseArguments(arguments);

    if (parseResult.cancelRun)
    {
        utility->printInfo(parseResult.message);
        return;
    }

    utility_ = utility;

    utility_->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    // Must have a component under any condition.
    QSharedPointer<Component> component =
        utility_->getLibraryInterface()->getModel(parseResult.vlnv).dynamicCast<Component>();
    if (!component)
    {
        utility_->printError(tr("Invalid component given as a parameter."));
        return;
    }

    utility_->printInfo(tr("Running generation for %1 and view '%2'.").arg(parseResult.vlnv.toString(), 
        parseResult.viewName));

    QDir targetDirectory;
    if (!targetDirectory.mkpath(parseResult.path))
    {
        utility_->printError(tr("Could not create target directory: %1").arg(parseResult.path));
        return;
    }

    utility_->printInfo(tr("Target directory: %1").arg(parseResult.path));

    const bool saveToFileset = false;
    QSharedPointer<ViewSelection> viewSettings(new ViewSelection(QStringLiteral("vhdl"), saveToFileset,
        QString(), &generationSettings_, component->getViews(), component->getComponentInstantiations(), 
        component->getFileSets()));

    QString filePath = parseResult.path;
    filePath.append(QLatin1Char('/'));
    filePath.append(findEntityName(component, parseResult.viewName));
    filePath.append(QStringLiteral(".vhd"));

    generate(component, filePath, viewSettings);
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::noticeMessage()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::noticeMessage(QString const& message)
{
    utility_->printInfo(message);
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::errorMessage()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::errorMessage(QString const& message)
{
    utility_->printError(message);
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::findEntityName()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::findEntityName(QSharedPointer<Component> component, QString const& viewName) const
{
    foreach (QSharedPointer<View> componentView, *component->getViews())
    {
        if (componentView->name().compare(viewName) == 0)
        {
            QString componentInstantiation = componentView->getComponentInstantiationRef();

            for (QList<QSharedPointer<ComponentInstantiation> >::const_iterator iter = 
                component->getComponentInstantiations()->cbegin();
                iter != component->getComponentInstantiations()->cend(); ++iter)
            {
                if ((*iter)->name().compare(componentInstantiation) == 0)
                {
                    return (*iter)->getModuleName();
                }
            }
        }
    }

    return component->getVlnv().getName();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::generate()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::generate(QSharedPointer<Component> component, QString const& filePath, 
    QSharedPointer<ViewSelection> viewSettings)
{
    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(finder));

    VhdlGenerator2 generator(expressionParser, utility_->getLibraryInterface(), this);

    connect(&generator, SIGNAL(errorMessage(const QString&)),
        this, SLOT(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&generator, SIGNAL(noticeMessage(const QString&)),
        this, SLOT(noticeMessage(const QString&)), Qt::UniqueConnection);

    // if errors are detected during parsing
    if (!generator.parse(component, viewSettings->getViewName()))
    {
        return;
    }

    // Generate the VHDL code.
    generator.generate(filePath);

    if (viewSettings->getSaveToFileset())
    {
        QString basePath = utility_->getLibraryInterface()->getPath(component->getVlnv());
        QString relativePath = General::getRelativePath(basePath, filePath);

        generator.addRTLView(viewSettings->getFileSetName(), filePath);
        utility_->getLibraryInterface()->writeModelToFile(component);
    }

    // Inform when done.
    utility_->printInfo( "VHDL generation complete." );
}
