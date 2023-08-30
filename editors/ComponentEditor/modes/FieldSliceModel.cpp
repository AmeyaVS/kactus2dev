//-----------------------------------------------------------------------------
// File: FieldSliceModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Model for Mode elements within a component.
//-----------------------------------------------------------------------------

#include "FieldSliceModel.h"

#include "FieldSliceColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <QStringList>
#include <QRegularExpression>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::FieldSliceModel()
//-----------------------------------------------------------------------------
FieldSliceModel::FieldSliceModel(QSharedPointer <Mode> mode,
 //   QSharedPointer<FieldSliceValidator> validator,
    QObject* parent): 
    QAbstractTableModel(parent),
 //   validator_(validator),
    fieldSlices_(mode->getFieldSlices())
{
    
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::rowCount()
//-----------------------------------------------------------------------------
int FieldSliceModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return fieldSlices_->count();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::columnCount()
//-----------------------------------------------------------------------------
int FieldSliceModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return FieldSliceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FieldSliceModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::headerData()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == FieldSliceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == FieldSliceColumns::FIELD_REF)
    {
        return tr("Field");
    }
    else if (section == FieldSliceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::data()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size())
    {
        return QVariant();
    }

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());
    const int column = index.column(); 

    if (role == Qt::DisplayRole)
    {
        return valueForIndex(index).toString();
    }

    else if (role == Qt::EditRole)
    {
        return valueForIndex(index);
    }

    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }


    else if (role == Qt::BackgroundRole)
    {
        if (column == FieldSliceColumns::NAME)
        {
            return KactusColors::MANDATORY_FIELD;
        }
    }


    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::setData()
//-----------------------------------------------------------------------------
bool FieldSliceModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size() || role != Qt::EditRole)
    {
        return false;
    }

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());
    const int column = index.column();

    if (column == FieldSliceColumns::NAME)
    {
        slice->setName(value.toString());
    }

    else if (column == FieldSliceColumns::DESCRIPTION)
    {
        fieldSlices_->at(index.row())->setDescription(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    emit contentChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::onAddItem()
//-----------------------------------------------------------------------------
void FieldSliceModel::onAddItem(QModelIndex const& index)
{
	int row = fieldSlices_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fieldSlices_->insert(row, QSharedPointer<FieldSlice>(new FieldSlice()));
	endInsertRows();


	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::onRemoveItem()
//-----------------------------------------------------------------------------
void FieldSliceModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= fieldSlices_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fieldSlices_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}


//-----------------------------------------------------------------------------
// Function: FieldSliceModel::validateIndex()
//-----------------------------------------------------------------------------
bool FieldSliceModel::validateIndex(QModelIndex const& index) const
{
    const int column = index.column();
    auto FieldSlice = fieldSlices_->at(index.row());
// 
//     if (column == FieldSliceColumns::NAME)
//     {
//         return validator_->hasValidName(FieldSlice->name());
//     }


    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldSliceModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant FieldSliceModel::valueForIndex(QModelIndex const& index) const
{
    const int column = index.column();

    QSharedPointer<FieldSlice> slice = fieldSlices_->at(index.row());

    if (column == FieldSliceColumns::NAME)
    {
        return slice->name();
    }
    else if (column == FieldSliceColumns::FIELD_REF)
    {
        auto ref = slice->getReference(FieldReference::FIELD);
        if (ref.isNull() == false)
        {
            return ref->reference_;
        }
    }
    else if (column == FieldSliceColumns::DESCRIPTION)
    {
        return slice->description();
    }

    return QVariant();
}
