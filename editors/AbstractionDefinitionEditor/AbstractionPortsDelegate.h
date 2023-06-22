//-----------------------------------------------------------------------------
// File: AbstractionPortsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.06.2018
//
// Description:
// Master delegate for abstraction definition wire and transactional ports.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONPORTSDELEGATE_H
#define ABSTRACTIONPORTSDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

#include <IPXACTmodels/common/Document.h>

#include <QStyledItemDelegate>

class BusDefinition;
class LibraryInterface;
class PortAbstractionInterface;
class QualifierEditor;

//-----------------------------------------------------------------------------
//! Master delegate for abstraction definition wire and transactional ports.
//-----------------------------------------------------------------------------
class AbstractionPortsDelegate : public EnumerationEditorConstructorDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] stdRevision     The revision of the standard used by the abstraction definition.
	 *      @param [in] parent          The owner of this instance
     */
    AbstractionPortsDelegate(LibraryInterface* libraryAcces, Document::Revision stdRevision, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~AbstractionPortsDelegate() = default;

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor The editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;
        
	/*!
     *  Set the bus definition referenced by the edited abstraction definition.
	 *
	 *      @param [in] busDefinition  The referenced bus definition.
     */
    void setBusDef(QSharedPointer<const BusDefinition> busDefinition);

    /*!
     *	Modifies the editor depending on document standard revision.
     *
     *      @param [in] revision	The revision of the document standard.
     */
    void setRevision(Document::Revision revision);

    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

    void onEditingCanceled();

protected:

    /*!
     *  Get the list of available qualifier values.
     *
     *      @return The list of available qualifier values.
     */
    virtual QStringList getQualifierList() const;

    /*!
     *  Check if the editor used in the selected index column is a line editor.
     *
     *      @param [in] indexColumn     The selected index column.
     *
     *      @return True, if the editor used in the selected column is a line editor, false otherwise.
     */
    virtual bool editorIsLineEditor(int indexColumn) const;

    /*!
     *  Check if the editor used in the selected index column is a combo box editor.
     *
     *      @param [in] indexColumn     The selected index column.
     *
     *      @return True, if the editor used in the selected column is a combo box editor, false otherwise.
     */
    virtual bool editorIsComboBox(int indexColumn) const;

    Document::Revision stdRevision_;

private:
	//! No copying. No assignment.
    AbstractionPortsDelegate(const AbstractionPortsDelegate& other);
    AbstractionPortsDelegate& operator=(const AbstractionPortsDelegate& other);

    /*!
     *  Check if the column is used for enumerations.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the column is used for editing enumerations, false otherwise.
     */
    virtual bool isEnumerationEditorColumn(QModelIndex const& index) const override final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return List of currently selected enumerations.
     */
    virtual QStringList getCurrentSelection(QModelIndex const& index) const override final;

    /*!
     *  Get the list of the available enumerations.
     *
     *      @return List of the available enumerations.
     */
    virtual QStringList getAvailableItems() const override final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *      @param [in] index           Model index identifying the item that's data is to be saved.
     *      @param [in] model           Model that contains the data structure where data is to be saved to.
     *      @param [in] selectedItems   List of the selected enumerations.
     */
    virtual void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const override final;

    void setupQualifierEditorQualifiers(QModelIndex const& index, QualifierEditor* qualifierEditor, QStringList const& attributeItems) const;

    void saveQualifierData(QualifierEditor* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface to the library.
    LibraryInterface* libraryAccess_;

    //! The bus definition referenced by the edited abstraction definition.
    QSharedPointer<const BusDefinition> busDefinition_;

    QStringList modeOptions_ = { "initiator", "target", "system" };
};

#endif // ABSTRACTIONPORTSDELEGATE_H
