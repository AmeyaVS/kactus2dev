//-----------------------------------------------------------------------------
// File: SingleAddressBlockEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.04.2015
//
// Description:
// Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------

#ifndef SINGLEADDRESSBLOCKEDITOR_H
#define SINGLEADDRESSBLOCKEDITOR_H

#include "addressblockeditor.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <QSharedPointer>

class ExpressionFormatter;
class ExpressionEditor;
class ExpressionParser;
class LibraryInterface;
class ParameterFinder;
class UsageComboBox;
class AccessComboBox;
class BooleanComboBox;
class RegisterFileEditor;
class AddressBlockValidator;
class RegisterInterface;

//-----------------------------------------------------------------------------
//! Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------
class SingleAddressBlockEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] registerInterface       Interface for registers.
	 *      @param [in] addressBlock            The address block being edited.
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] expressionParser        The expression parser.
     *      @param [in] addressBlockValidator   Validator used for address blocks.
	 *      @param [in] parent                  The parent of this editor.
	 */
    SingleAddressBlockEditor(RegisterInterface* registerInterface,
        QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<AddressBlockValidator> addressBlockValidator,
        QWidget* parent = 0);


	//! The destructor
    virtual ~SingleAddressBlockEditor();

	/*!
	 *  Reload all the contained editors.
	 */
	virtual void refresh();

signals:

    /*!
     *  Change the value for address unit bits in the model.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits value.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

protected:

    /*!
	 *  Shows the widget and informs of the required help document.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

private slots:
    
    /*!
     *  Sets the value in the base address editor.
     */
    void onBaseAddressChanged();

    /*!
     *  Sets the value in the range editor.
     */
    void onRangeChanged();

    /*!
     *  Sets the value in the width editor.
     */
    void onWidthChanged();

    //! Called when isPresent is changed.
    void onIsPresentEdited();

    /*!
     *  Sets the value in the usage editor.
     *
     *      @param [in] newUsage    The selected usage.
     */
    void onUsageSelected(QString const& newUsage);

    /*!
     *  Sets the value in the access editor.
     *
     *      @param [in] newAccess   The selected access.
     */
    void onAccessSelected(QString const& newAccess);

    /*!
     *  Sets the value in the volatile editor.
     *
     *      @param [in] newVolatileValue    The selected volatile status.
     */
    void onVolatileSelected(QString const& newVolatileValue);

private:
	
	//! No copying.
    SingleAddressBlockEditor(const SingleAddressBlockEditor& other);

	//! No assignment.
    SingleAddressBlockEditor& operator=(const SingleAddressBlockEditor& other);

    /*!
     *  Sets the layout for the editor.
     */
    void setupLayout();

    /*!
     *  Connects all the signals of the editors.
     */
    void connectSignals() const;

    /*!
     *  Change the status of signal blocking in all the expression editors.
     *
     *      @param [in] blockStatus     The new signal blocking status.
     */
    void changeExpressionEditorSignalBlockStatus(bool blockStatus);

    /*!
     *  Calculates the value of an expression.
     *
     *      @param [in] expression  The expression, whose value is to be calculated.
     *
     *      @return The calculated value of the given expression.
     */
    QString formattedValueFor(QString const& expression) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for containing the name group information of the address block.
    NameGroupEditor nameEditor_;

    //! Editor for selecting the usage of the address block.
    UsageComboBox* usageEditor_;

    //! Editor for the base address of the address block.
    ExpressionEditor* baseAddressEditor_;

    //! Editor for the range of the address block.
    ExpressionEditor* rangeEditor_;

    //! Editor for the width of the address block-
    ExpressionEditor* widthEditor_;

    //! Editor for the is present of the address block-
    ExpressionEditor* isPresentEditor_;

    //! Editor for selecting the access of the address block.
    AccessComboBox* accessEditor_;

    //! Editor for selecting the volatility of the address block.
    BooleanComboBox* volatileEditor_;

    //! Editor for containing the registers of the address block.
    AddressBlockEditor* registersEditor_;

    //! Editor for containing the register files of the address block.
    RegisterFileEditor* registerFilesEditor_;

    //! The current address block.
    QSharedPointer<AddressBlock> addressBlock_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SINGLEADDRESSBLOCKEDITOR_H
