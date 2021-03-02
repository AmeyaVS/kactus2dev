//-----------------------------------------------------------------------------
// File: ScriptingConsole.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.09.2019
//
// Description:
// Widget for scripting.
//-----------------------------------------------------------------------------

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ScriptingConsole.h"
#include "ScriptingTextEditor.h"
#include "ScriptingHistory.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>

#include <PythonAPI/PythonInterpreter.h>

#include <PythonAPI/ChannelRelay.h>

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::ScriptingConsole()
//-----------------------------------------------------------------------------
ScriptingConsole::ScriptingConsole(QWidget* parent):
    QWidget(parent),
    outputChannel_(new ChannelRelay(this)),
    errorChannel_(new ChannelRelay(this)),
    interpreter_(new PythonInterpreter(outputChannel_, errorChannel_, this)),
    history_(new ScriptingHistory(this)),
    scriptEditor_(new ScriptingTextEditor(interpreter_, history_, this)),
    historyListing_(new QListWidget(this)),
    toolBar_(new QToolBar(this))
{    
    connect(outputChannel_, SIGNAL(data(QString const&)),
        scriptEditor_, SLOT(print(QString const&)), Qt::UniqueConnection);
    connect(errorChannel_, SIGNAL(data(QString const&)),
        scriptEditor_, SLOT(printError(QString const&)), Qt::UniqueConnection);

    interpreter_->initialize();

    QAction* historyAction = toolBar_->addAction(QIcon(":/icons/common/graphics/history.png"), QString());
    historyAction->setToolTip(QStringLiteral("Show history"));
    historyAction->setCheckable(true);
    
    connect(historyAction, SIGNAL(toggled(bool)), historyListing_, SLOT(setVisible(bool)), Qt::UniqueConnection);

    QAction* saveAction = toolBar_->addAction(QIcon(":/icons/common/graphics/edit.png"), QString(), this, SLOT(onSaveAction()));
    saveAction->setToolTip(QStringLiteral("Save script"));

    QAction* runAction = toolBar_->addAction(QIcon(":/icons/common/graphics/runProgram.png"), QString());
    runAction->setToolTip(QStringLiteral("Run file (Disabled)"));
    runAction->setEnabled(false);

    connect(history_, SIGNAL(commandAdded(QString const&)), 
        this, SLOT(onCommandInput(QString const&)), Qt::UniqueConnection);
    connect(historyListing_, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(onHistoryItemClicked(QListWidgetItem*)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::applySettings()
//-----------------------------------------------------------------------------
void ScriptingConsole::applySettings()
{
    scriptEditor_->applySettings();
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onHistoryAction()
//-----------------------------------------------------------------------------
void ScriptingConsole::onSaveAction()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("Python Script (*.py)"));
    if (fileName.isEmpty() == false)
    {
        QFile outputFile(fileName);
        outputFile.open(QFile::WriteOnly | QFile::Text);

        QTextStream output(&outputFile);
        for (int i = 0; i < historyListing_->count(); ++i)
        {
            output << historyListing_->item(i)->text() << "\n";
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onCommandInput()
//-----------------------------------------------------------------------------
void ScriptingConsole::onCommandInput(QString const& command)
{
    historyListing_->addItem(command);
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::onHistoryItemClicked()
//-----------------------------------------------------------------------------
void ScriptingConsole::onHistoryItemClicked(QListWidgetItem* item)
{
    scriptEditor_->insertInput(item->text());
}

//-----------------------------------------------------------------------------
// Function: ScriptingConsole::setupLayout()
//-----------------------------------------------------------------------------
void ScriptingConsole::setupLayout()
{
    toolBar_->setOrientation(Qt::Vertical);

    historyListing_->hide();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(scriptEditor_);
    layout->addWidget(historyListing_);
    layout->addWidget(toolBar_);
    layout->setContentsMargins(0, 0, 0, 2 );
}
