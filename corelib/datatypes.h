#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
#include <map>
#include <memory>

/*!
 \defgroup ldapcore
 \brief namespace for groupping ldap core related functionality

 Namespace ldapeditor wrapes all classes and declarations, related to ldap core functionality
*/
namespace ldapcore
{

typedef struct _tConnectionOptions
{
	std::string host;
	int port{389};
	int version{3};
	std::string basedn;
	std::string username;
	std::string password;
	bool simpleAuth{true};
	bool useSSL{false};
	bool useSASL{false};
	bool useAnonymous{false};
	int  timeout{60}; // seconds

    bool useTLS{false};
    std::string cacertfile;
    std::string certfile;
    std::string keyfile;

} tConnectionOptions;

typedef struct _tSearchOptions
{
	int scope{1}; //LDAPConnection::SEARCH_ONE
	std::string basedn;
	std::string filter{"objectClass=*"};
	std::string  attributes;
} tSearchOptions;

}
#endif // DATATYPES_H
