#ifndef CLDAP_EXCEPTION_H
#define CLDAP_EXCEPTION_H

#include <exception>
#include <string>

namespace ldapcore
{

/*!
 * @ingroup ldapcore
 * @brief The ldap name missed exception
 *
 * The CLdapNameMissedException is represented exception, which thrown when validation of attribute has failed
 */
class CLdapNameMissedException : public std::exception
{
public:

    /*!
     * \brief Constructor CLdapNameMissedException by default
     * @param lpText The text of exception
     */
	CLdapNameMissedException(char const* lpText);

    /*!
     * \brief Method gets text of exception
     * @return char const* text of exception
     */
	virtual char const* what() const _GLIBCXX_USE_NOEXCEPT;

private:
    std::string m_what;///< text of exception
};


/*!
 * @ingroup ldapcore
 * @brief The ldap match rule exception
 *
 * The CLdapMatchRuleException is represented exception, which thrown when match rule validation of attribute has failed
 */
class CLdapMatchRuleException : public std::exception
{
public:

    /*!
     * \brief Constructor CLdapMatchRuleException by default
     * @param lpText The text of exception
     */
	CLdapMatchRuleException(char const* lpText);

    /*!
     * \brief Method gets text of exception
     * @return char const* text of exception
     */
    virtual char const* what() const _GLIBCXX_USE_NOEXCEPT;

private:
    std::string m_what;///< text of exception
};

/*!
 * @ingroup ldapcore
 * @brief The ldap server exception
 *
 * The CLdapServerException is represented exception, which thrown when ldap server has failed
 */
class CLdapServerException : public std::exception
{
public:

    /*!
     * \brief Constructor CLdapServerException by default
     * @param lpText The text of exception
     */
    CLdapServerException(char const* lpText);

    /*!
     * \brief Method gets text of exception
     * @return char const* text of exception
     */
    virtual char const* what() const _GLIBCXX_USE_NOEXCEPT;

private:
    std::string m_what;///< text of exception
};


}


#endif // CLDAP_EXCEPTION_H

