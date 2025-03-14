//-----------------------------------------------------------------------------
// File: RenodeMemoriesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2023
//
// Description:
// The model to manage the Renode memory details.
//-----------------------------------------------------------------------------

#ifndef RENODEMEMORIESMODEL_H
#define RENODEMEMORIESMODEL_H

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The model to manage the Renode memory details.
//-----------------------------------------------------------------------------
class RenodeMemoriesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] parent  Pointer to the owner of this model.
	 */
    RenodeMemoriesModel(QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~RenodeMemoriesModel() = default;

    /*!
     *  Setup the selected memory items.
     *
     *    @param [in] newMemories     The selected memory items.
     */
    void setupMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories> > newMemories);

	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index   Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section         The section specifies the row/column number for the header.
	 *    @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *    @param [in] role            Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is requested.
	 *    @param [in] role    The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *    @param [in] index   The model index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving happened successfully.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    //! No copying. No assignment.
    RenodeMemoriesModel(const RenodeMemoriesModel& other) = delete;
    RenodeMemoriesModel& operator=(const RenodeMemoriesModel& other) = delete;

private:
	
    /*!
     *  Gets the value for the given index.
     *
     *    @param [in] index   The index of the target data.
     *
     *    @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the tooltip value for the given index.
     *
     *    @param [in] index   The index of the target data.
     *
     *    @return The tooltip data in the given index.
     */
    QVariant tooltipForIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The memory items being edited.
    QVector<QSharedPointer<RenodeStructs::cpuMemories> > memories_;
};

#endif // RENODEMEMORIESMODEL_H
