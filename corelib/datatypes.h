#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
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
    bool useTLS{false};
    bool useSASL{false};
    bool useAnonymous{false};
}tConnectionOptions;

typedef struct _tSearchOptions
{
    int scope{1}; //LDAPConnection::SEARCH_ONE
     std::string basedn;
     std::string filter{"objectClass=*"};
     std::string  attributes;
}tSearchOptions;

}
#endif // DATATYPES_H
