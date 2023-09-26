//-----------------------------------------------------------------------------
// File: ModeValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Validator for ipxact:mode.
//-----------------------------------------------------------------------------

#include "ModeValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Mode.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ModeValidator::ModeValidator()
//-----------------------------------------------------------------------------
ModeValidator::ModeValidator(
	QSharedPointer<Component> component, 
	QSharedPointer<ExpressionParser> expressionParser):
	portValidator_(new PortSliceValidator(component, expressionParser)),
	fieldValidator_(new FieldSliceValidator(component, expressionParser)),
	component_(component),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: ModeValidator::~ModeValidator()
//-----------------------------------------------------------------------------
ModeValidator::~ModeValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ModeValidator::validate()
//-----------------------------------------------------------------------------
bool ModeValidator::validate(QSharedPointer<Mode> mode) const
{
	if (hasValidName(mode->name()) == false)
	{
		return false;
	}
	
	if (hasValidPortSlices(mode) == false)
	{
		return false;
	}

	if (hasValidFieldSlices(mode) == false)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ModeValidator::hasValidName(QString const& name) const
{
	return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::hasValidPortSlices()
//-----------------------------------------------------------------------------
bool ModeValidator::hasValidPortSlices(QSharedPointer<Mode> mode) const
{
	QStringList sliceNames;

	for (auto const& portSlice : *mode->getPortSlices())
    {
        if (sliceNames.contains(portSlice->name()))
        {
            return false;
        }

		if (portValidator_->validate(portSlice) == false)
		{
			return false;
		}

		sliceNames.append(portSlice->name());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::hasValidFieldSlices()
//-----------------------------------------------------------------------------
bool ModeValidator::hasValidFieldSlices(QSharedPointer<Mode> mode) const
{
    QStringList sliceNames;

    for (auto const& fieldSlice : *mode->getFieldSlices())
    {
        if (sliceNames.contains(fieldSlice->name()))
        {
            return false;
        }

        if (fieldValidator_->validate(fieldSlice) == false)
        {
            return false;
        }

        sliceNames.append(fieldSlice->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ModeValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Mode> mode,
    QString const& context) const
{
    if (!hasValidName(mode->name()))
    {
        errors.append(QObject::tr("Invalid name '%1' set for mode within %2.").arg(mode->name(), context));
    }

	QString modeContext(QStringLiteral("mode '%1'").arg(mode->name()));
	QStringList sliceNames;
	QStringList reportedNames;

    for (auto const& portSlice : *mode->getPortSlices())
    {
		if (sliceNames.contains(portSlice->name()) && reportedNames.contains(portSlice->name()) == false)
        {
            errors.append(QObject::tr("Port condition name '%1' is not unique within %2.").arg(
                portSlice->name(), modeContext));

			reportedNames.append(portSlice->name());
		}
        sliceNames.append(portSlice->name());

		portValidator_->findErrorsIn(errors, portSlice, modeContext);
    }

    for (auto const& fieldSlice : *mode->getFieldSlices())
    {
        if (sliceNames.contains(fieldSlice->name()) && reportedNames.contains(fieldSlice->name()) == false)
        {
            errors.append(QObject::tr("Field condition name '%1' is not unique within %2.").arg(
                fieldSlice->name(), modeContext));

            reportedNames.append(fieldSlice->name());
        }
        sliceNames.append(fieldSlice->name());

        fieldValidator_->findErrorsIn(errors, fieldSlice, modeContext);
    }
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::componentChange()
//-----------------------------------------------------------------------------
void ModeValidator::componentChange(QSharedPointer<Component> newComponent)
{
	component_ = newComponent;
	portValidator_->componentChange(newComponent);
    fieldValidator_->componentChange(newComponent);
}

//-----------------------------------------------------------------------------
// Function: ModeValidator::getPortSliceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortSliceValidator> ModeValidator::getPortSliceValidator() const
{
	return portValidator_;
}


//-----------------------------------------------------------------------------
// Function: ModeValidator::getFieldSliceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<FieldSliceValidator> ModeValidator::getFieldSliceValidator() const
{
    return fieldValidator_;
}
