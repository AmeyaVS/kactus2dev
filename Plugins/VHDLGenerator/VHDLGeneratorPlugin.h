//-----------------------------------------------------------------------------
// File: VHDLGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2017
//
// Description:
// Kactus2 plugin for VHDL file generation.
//-----------------------------------------------------------------------------

#ifndef VHDLGENERATORPLUGIN_H
#define VHDLGENERATORPLUGIN_H

#include "VHDLGenerator_global.h"

#include <Plugins/PluginSystem/CommandLineSupport.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>
#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

#include <QCommandLineParser>

class ViewSelection;
//-----------------------------------------------------------------------------
//! Kactus2 plugin for VHDL file generation.
//-----------------------------------------------------------------------------
class VHDLGENERATOR_EXPORT VHDLGeneratorPlugin : public QObject, public IGeneratorPlugin, public CommandLineSupport
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.VHDLGenerator" FILE "VHDLGenerator.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    //! The constructor.
    VHDLGeneratorPlugin();
    
    //! The destructor.
    ~VHDLGeneratorPlugin();

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
     *  Returns the holder of the licence of the plugin.
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
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const;
     
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
    virtual void runGenerator(IPluginUtility* utility, 
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration);

     //! Returns the external program requirements of the plugin.
     virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();
     
    /*!
     *  Gets the command required to run the plugin.
     *
     *      @return The command to run the plugin.
     */
     virtual QString getCommand() const;

    /*!
     *  Executes the plugin with the given arguments.
     *
     *      @param [in] arguments   The arguments for the execution.
     *      @param [in] utility     Utilities for enabling plugin execution.
     */
     virtual void process(QStringList const& arguments, IPluginUtility* utility);

private slots:
    
    //! Called when a user notification should be shown.
    void noticeMessage(QString const& message);

    //! Called when an error should be shown.
    void errorMessage(QString const& message);
   
private:
    //! The plugin utility provided by call runGenerator.
    IPluginUtility* utility_;

    //! Settings for the generation.
    GenerationSettings generationSettings_;

    /*!
     *  Finds the name of the component entity.
     *
     *      @param [in] component   The component to search.
     *      @param [in] viewName    The selected view to search.
     *
     *      @return The name of the VHDL entity for the component.
     */
    QString findEntityName(QSharedPointer<Component> component, QString const& viewName) const;

    /*!
     *  Runs the VHDL generation for the given component.
     *
     *      @param [in] component       The component to generate VHDL from.
     *      @param [in] filePath        The file path to save the VHDL file.
     *      @param [in] viewSettings    The view settings for the generation.
     */
    void generate(QSharedPointer<Component> component, QString const& filePath,
        QSharedPointer<ViewSelection> viewSettings);
};

#endif // VHDLGENERATORPLUGIN_H