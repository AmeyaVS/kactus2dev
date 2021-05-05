//-----------------------------------------------------------------------------
// File: SVDGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#include "SVDGeneratorPlugin.h"
#include "SVDGenerator.h"

#include <Plugins/common/HDLParser/HDLParserCommon.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <Plugins/SVDGenerator/CPUSelectionDialog.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QDateTime>
#include <QFileDialog>

//----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::SVDGeneratorPlugin()
//-----------------------------------------------------------------------------
SVDGeneratorPlugin::SVDGeneratorPlugin(): QObject(0)
{
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getName() const
{
    return tr("SVD Generator");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getVersion() const
{
    return "0.1";
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getDescription() const
{
    return tr("Creates a CMSIS System View Description.");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString SVDGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* SVDGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* SVDGeneratorPlugin::getSettingsModel()
{
    return NULL;
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> SVDGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon SVDGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/SVDGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool SVDGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    if (!component || component->getImplementation() != KactusAttribute::HW)
    {
        return false;
    }

    return design || designConfiguration;
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void SVDGeneratorPlugin::runGenerator(IPluginUtility* utility,
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv());

    GenerationTuple controlTuple;
    controlTuple.component = component;
    controlTuple.design = design;
    controlTuple.designConfiguration = designConfiguration;

    QSharedPointer<QList<QSharedPointer<View> > > views = GenerationControl::findPossibleViews(controlTuple);
    QString activeView;
    if (!views->isEmpty())
    {
        activeView = views->first()->name();
    }

    CPUSelectionDialog selectionDialog(
        component, utility->getLibraryInterface(), views, utility->getParentWidget());
    if (selectionDialog.exec() == QDialog::Accepted)
    {
        QVector<ConnectivityGraphUtilities::interfaceRoutes> cpuRoutes = selectionDialog.getSelectedCPUs();
        if (!cpuRoutes.isEmpty())
        {
            bool blocksArePeripherals = selectionDialog.peripheralsAreBlocks();
            bool mapsArePeripherals = selectionDialog.peripheralsAreMaps();

            SVDGenerator generator(utility->getLibraryInterface());
            generator.generate(component, xmlFilePath, cpuRoutes, blocksArePeripherals, mapsArePeripherals);

            QVector<QString> generatedFiles = generator.getGeneratedFiles();

            utility->printInfo(tr("Generation complete."));
        }
        else
        {
            utility->printError(tr("Could not find CPU routes"));
        }
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGeneratorPlugin::saveToFileset()
//-----------------------------------------------------------------------------
/*
void SVDGeneratorPlugin::saveToFileset(QString const& targetFile, QSharedPointer<Component> component,
    IPluginUtility* utility)
{
    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv());

    QSharedPointer<Component> topComponent = component.dynamicCast<Component>();

    QString filesetName = "systemViewDescriptions";
    QSharedPointer<FileSet> targetFileset = topComponent->getFileSet(filesetName);
    if (!targetFileset)
    {
        targetFileset = QSharedPointer<FileSet>(new FileSet(filesetName));
        topComponent->getFileSets()->append(targetFileset);
    }

    QString relativeFilePath = General::getRelativePath(xmlFilePath, targetFile);

    QSharedPointer<File> file;
    foreach(QSharedPointer<File> filesetFile, *targetFileset->getFiles())
    {
        if (filesetFile->name().compare(relativeFilePath) == 0)
        {
            file = filesetFile;
        }
    }

    if (!file)
    {
        file = QSharedPointer<File>(new File(relativeFilePath));
        file->getFileTypes()->append(QStringLiteral("unknown"));

        targetFileset->addFile(file);
        utility->printInfo(tr("Added file %1 to top component file set %2.").arg(targetFile, filesetName));
    }

    file->setDescription(tr("Generated on %1 by Kactus2 %2 plugin version %3.").arg(
        QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), getName(), getVersion()));

    utility->getLibraryInterface()->writeModelToFile(component);
}
*/