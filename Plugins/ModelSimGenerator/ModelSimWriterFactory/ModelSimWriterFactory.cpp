//-----------------------------------------------------------------------------
// File: ModelSimWriterFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Creates writers for generating do files.
//-----------------------------------------------------------------------------

#include "ModelSimWriterFactory.h"
#include "ModelSimWriter.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::ModelSimWriterFactory()
//-----------------------------------------------------------------------------
ModelSimWriterFactory::ModelSimWriterFactory(LibraryInterface* library,
    MessageMediator* messages, GenerationSettings* settings,
    QString const& kactusVersion, QString const& generatorVersion) :
    library_(library),
    messages_(messages),
    settings_(settings),    
    generatorVersion_(generatorVersion),
    kactusVersion_(kactusVersion)
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::prepareComponent()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationOutput> ModelSimWriterFactory::prepareComponent(QString const& /*outputPath*/,
    QSharedPointer<MetaComponent> /*component*/)
{
    return QSharedPointer<ModelSimDocument>();
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::prepareDesign()
//-----------------------------------------------------------------------------
QList<QSharedPointer<GenerationOutput> > ModelSimWriterFactory::prepareDesign(QList<QSharedPointer<MetaDesign> >& designs)
{
    QSharedPointer<ModelSimDocument> document = 
        QSharedPointer<ModelSimDocument>(new ModelSimDocument(QSharedPointer<ModelSimWriter>(new ModelSimWriter)));

    QList<QSharedPointer<MetaInstance> > components;

    // Instead of including every file in the hierarchy, include only files from the file sets of the top level design component,
    // and any leaf components. Any components in between in the hierarchy shouldn't ideally have any manually created 
    // rtl files, only kactus2-generated structural rtl which should be generated from the topmost component.

    QList<QSharedPointer<GenerationOutput> > retval;
    if (designs.isEmpty()) return retval;
    
    QSharedPointer<ModelSimWriter> writer(new ModelSimWriter);
    document->writer_ = writer;
    document->fileName_ = designs.first()->getTopInstance()->getModuleName() + ".do";
    document->vlnv_ = designs.first()->getTopInstance()->getComponent()->getVlnv().toString();
    document->metaDesign_ = designs.first();

    for (auto const& design : designs)
    {
        // Always add files of topmost design component
        if (design == designs.first())
        {
            components.append(design->getTopInstance());
        }

        // Add design instances if they contain no design themselves
        for (auto const& instance : *design->getInstances())
        {
            // Figure out through the active view if the component instance is hierarchical
            auto view = instance->getActiveView();

            if (view != nullptr && instance->getComponent()->getHierRef(view->name()).isValid() == false)
            {
                components.append(instance);
            }
        }
    }

    // Finally add the files of the selected components to be included
    for (QSharedPointer<MetaComponent> mComponent : components)
    {
        QString basePath = library_->getPath(mComponent->getComponent()->getVlnv());

        for (QSharedPointer<FileSet> fileSet : *mComponent->getFileSets())
        {
            for (QSharedPointer<File> file : *fileSet->getFiles())
            {
                // Fetch the absolute path to the file
                QString absolutePath = General::getAbsolutePath(basePath, file->name());

                writer->addPath(absolutePath);
            }
        }
    }

    retval.append(document);
    return retval;
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getLanguage()
//-----------------------------------------------------------------------------
QString ModelSimWriterFactory::getLanguage() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getSaveToFileset()
//-----------------------------------------------------------------------------
bool ModelSimWriterFactory::getSaveToFileset() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriterFactory::getGroupIdentifier()
//-----------------------------------------------------------------------------
QString ModelSimWriterFactory::getGroupIdentifier() const
{
    return QStringLiteral("simulation");
}
