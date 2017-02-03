//-----------------------------------------------------------------------------
// File: HDLComponentParser.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.08.2016
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLCOMPONENTPARSER_H
#define HDLCOMPONENTPARSER_H

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>

#include <Plugins/common/PortSorter/PortSorter.h>
#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/common/HDLParser/MetaInstance.h>

#include <QSharedPointer>
#include <QTextStream>
#include <QList>

class ExpressionFormatter;
class AddressBlock;
class LibraryInterface;

//-----------------------------------------------------------------------------
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------
class HDLComponentParser : public MetaInstance
{
public:
    
	/*!
	 *  The constructor.
     *
     *      @param [in] library             The library which is expected to contain the parsed documents.
     *      @param [in] messages            For message output.
	 *      @param [in] component           The instantiated component.
     *      @param [in] activeView          The active view for the component. May be null
	 */
    HDLComponentParser(LibraryInterface* library,
        MessagePasser* messages,
        QSharedPointer<Component> component,
        QSharedPointer<View> activeView);

	//! The destructor.
	~HDLComponentParser();

    /*!
     *  Sorts list of parameters based on their interdependencies.
     *
     *      @param [in] refParameters		    The list containing the parameters, to be used as a reference.
     *      @param [out] sortParameters         The list containing the same parameters as in refParameters, that will be sorted.
     */
	static void sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
        QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters);

    //! The parsed remap states.
    QList<QSharedPointer<FormattedRemapState> > remapStates_;

protected:
    /*!
    *  Parses the found parameter declarations.
    */
    virtual void formatParameters();
    
    /*!
    *  Culls and parses the ports of the component.
    */
    virtual void formatPorts();

private:
	// Disable copying.
	HDLComponentParser(HDLComponentParser const& rhs);
    HDLComponentParser& operator=(HDLComponentParser const& rhs);
    
    /*!
    *  Goes through the remap states, connects remaps to correct ports.
    */
    void parseRemapStates();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;
};

#endif // HDLCOMPONENTPARSER_H
