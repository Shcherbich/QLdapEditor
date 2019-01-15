#ifndef CLDAP_EXCEPTION_H
#define CLDAP_EXCEPTION_H

#include <exception>
#include <string>

namespace ldapcore
{

class CLdapNameMissedException : public std::exception
{
public:
	CLdapNameMissedException(char const* lpText);
    virtual char const* what() const _GLIBCXX_USE_NOEXCEPT;

private:
	std::string m_what;
};



class CLdapMatchRuleException : public std::exception
{
public:
	CLdapMatchRuleException(char const* lpText);
    virtual char const* what() const _GLIBCXX_USE_NOEXCEPT;

private:
	std::string m_what;
};

}


#endif // CLDAP_EXCEPTION_H

