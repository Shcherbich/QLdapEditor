cmake_minimum_required(VERSION 3.1.0)

project(LdapEditorCore)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(/usr/include)
include_directories(../3rd_party/openldap/contrib/ldapcpp/src)

#include_directories(../common)
#include_directories(../3rd_party/openldap/contrib/ldapcpp/src)
#include_directories(../3rd_party/openldap/include)


add_compile_options(-DLDAPEDITORCORE_SO)
add_compile_options(-DLDAPEDITORCORE_SO_EXPORTS)
add_compile_options(-std=c++11)

FIND_PACKAGE(Qt5Core CONFIG REQUIRED)

set(LdapCore_SRCS
    qfunctional.h
    CLdapData.h
    CLdapData.cpp
    CLdapObject.h
    CLdapObject.cpp
    CLdapAttribute.h
    CLdapAttribute.cpp
)

file(GLOB OpenLDAPCpp_SRCS
    "../3rd_party/openldap/contrib/ldapcpp/src/*.cpp"
    "../3rd_party/openldap/contrib/ldapcpp/src/*.h"
)


#FIND_LIBRARY(LDAPCPP_LIBRARY NAMES ldapcpp PATHS ../3rd_party/openldap/lib)
#message(LDAPCPP_LIBRARY=${LDAPCPP_LIBRARY})
FIND_LIBRARY(LDAP_LIBRARY NAMES ldap PATHS /usr/lib64)


add_library(${PROJECT_NAME} SHARED ${LdapCore_SRCS} ${OpenLDAPCpp_SRCS})

qt5_use_modules(${PROJECT_NAME} Core)
target_link_libraries(${PROJECT_NAME} ${LDAP_LIBRARY} ) #${LDAPCPP_LIBRARY}


INSTALL(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
)
