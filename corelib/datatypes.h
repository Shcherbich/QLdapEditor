#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
#include <map>
#include <memory>

/*!
 \defgroup ldapcore Core API
 \brief namespace for grouping ldap core related functionality

 Namespace ldapeditor wrapes all classes and declarations, related to ldap core functionality
*/
namespace ldapcore
{

/*!
  @brief type for storing connection options
*/
typedef struct _tConnectionOptions
{
    std::string host;   ///< host(ip) string
    int port{389};      ///< port number
    int version{3};     ///< LDAP protocol version
    std::string basedn;     ///< Base DN
    std::string username;   ///< user name used for authentification
    std::string password;   ///< password uused for authentification
    bool simpleAuth{true};  ///< Will simple authentification be used
    bool useSSL{false};     ///< Will SSL be used
    bool useSASL{false};    ///< will SASL be used
    bool useAnonymous{false}; ///< Will anonymous connection be used
    int  timeout{60};        ///< connection timeout, seconds

    bool useTLS{false};     ///< Will TLS be used
    std::string cacertfile; ///< CA certificate file path
    std::string certfile;   ///< Certificate file path
    std::string keyfile;    ///< Key file path

} tConnectionOptions;

/*!
  @brief type for storing search options
*/
typedef struct _tSearchOptions
{
    //LDAPConnection::SEARCH_ONE
    int scope{1};           ///< search scope, 0 = LDAPConnection::SEARCH_BASE, 1 = LDAPConnection::SEARCH_ONE, 2 = LDAPConnection::SEARCH_SUB
    std::string basedn;     ///< Base DN string
    std::string filter{"objectClass=*"};    ///< filter string
    std::string  attributes;    ///< list of attributes to return
} tSearchOptions;

}
#endif // DATATYPES_H
