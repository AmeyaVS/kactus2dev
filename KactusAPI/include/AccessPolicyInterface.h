//-----------------------------------------------------------------------------
// File: AccessPolicyInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 20.9.2023
//
// Description:
// Interface for editing access policies.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICYINTERFACE_H
#define ACCESSPOLICYINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <IPXACTmodels/common/AccessTypes.h>

#include <QList>
#include <QSharedPointer>

class AccessPolicy;

//-----------------------------------------------------------------------------
//! Interface for editing access policies.
//-----------------------------------------------------------------------------
class KACTUS2_API AccessPolicyInterface
{
public:

    AccessPolicyInterface() = default;
    virtual ~AccessPolicyInterface() = default;

    //! No copying.
    AccessPolicyInterface(AccessPolicyInterface const& other) = delete;
    AccessPolicyInterface& operator=(AccessPolicyInterface const& other) = delete;

    /*!
     *	Set available access policies.
     *  
     *      @param [in] accessPolicies     Access policies to set.
     */
    void setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies);

    /*!
     *	Get the access policy count.
     *	    
     * 	    @return The number of available access policies.
     */
    int getAccessPolicyCount() const;

    /*!
     *	Add a new access policy.
     */
    void addAccessPolicy();

    /*!
     *	Remove an access policy given by index.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy to remove.
     *	    
     * 	    @return True, if the removal was successful, otherwise false.
     */
    bool removeAccessPolicy(int accessPolicyIndex);

    /*!
     *	Clear and set the mode references of an access policy.
     *  
     *      @param [in] newModeRefs             The mode references to set.
     *      @param [in] accessPolicyIndex       The index of the access policy whose mode refs are set.
     *	    
     * 	    @return True, if the operation was successful, otherwise false.
     */
    bool setModeRefs(std::vector<std::pair<std::string, int> > const& newModeRefs, int accessPolicyIndex);

    /*!
     *	Get the mode references of an access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy whose mode references will be get.
     *	    
     * 	    @return A list of mode references as string-int pair vector.
     */
    std::vector<std::pair<std::string, int> > getModeRefs(int accessPolicyIndex) const;

    /*!
     *	Validates the available access policies.
     *  
     * 	    @return True, if the access policies are valid, otherwise false.
     */
    bool hasValidAccessPolicies() const;

    /*!
     *	Set the access value for a given accesspolicy.
     *  
     *      @param [in] newAccess             The new access value to set.
     *      @param [in] accessPolicyIndex     The index of the access policy to set the access for.
     *	    
     * 	    @return True, if the access value was successfully set, otherwise false.
     */
    bool setAccess(std::string const& newAccess, int accessPolicyIndex);

    /*!
     *	Get the access value as a string of a given access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy.
     *
     * 	    @return The access value as a string.
     */
    std::string getAccessString(int accessPolicyIndex) const;

    /*!
     *	Get the access value of a given access policy.
     *  
     *      @param [in] accessPolicyIndex     The index of the access policy.
     *	    
     * 	    @return The access value.
     */
    AccessTypes::Access getAccess(int accessPolicyIndex) const;

private:

    //! Available access policies.
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_ =
        QSharedPointer<QList<QSharedPointer<AccessPolicy> > >(new QList<QSharedPointer<AccessPolicy> >());
};


#endif // ACCESSPOLICYINTERFACE_H
