//-----------------------------------------------------------------------------
// File: SVDGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#ifndef SVDGENERATORPLUGIN_H
#define SVDGENERATORPLUGIN_H

#include "svdgeneratorplugin_global.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>
// #include <Plugins/PluginSystem/CommandLineSupport.h>
// #include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

//-----------------------------------------------------------------------------
//! Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------
class SVDGENERATORPLUGIN_EXPORT SVDGeneratorPlugin : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.svdgeneratorplugin" FILE "svdgeneratorplugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:

	/*!
     *  The constructor.
     */
	SVDGeneratorPlugin();

	/*!
     *  The destructor.
     */
    virtual ~SVDGeneratorPlugin() = default;

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the licence holder of the plugin.
     */
    virtual QString getLicenceHolder() const;
    
    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel();

    /*!
     *  Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;
    
    /*!
     *  Checks whether the generator may run for the given component or design. 
     *
     *      @param [in] component	        The component for which to check support. If design is not null, component
	 *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design, QSharedPointer<DesignConfiguration const> designConfiguration) const;

    /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *      @param [in] utility			    The plugin utility interface.
     *      @param [in] component	        The component for which to check support. If design is not null, component
     *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     */
    virtual void runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
        QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration);
   
private:
	// Disable copying.
	SVDGeneratorPlugin(SVDGeneratorPlugin const& rhs);
    SVDGeneratorPlugin& operator=(SVDGeneratorPlugin const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    /*!
     *  Saves the generated file to top component fileset.
     *
     *      @param [in] targetFile      The generated file path.
     *      @param [in] component       The top level component.
     *      @param [in] utility         The plugin utility interface.
     *
     *      @return <Description>.
     */
//     void saveToFileset(QString const& targetFile, QSharedPointer<Component> component, IPluginUtility* utility);
};

#endif // SVDGENERATORPLUGIN_H