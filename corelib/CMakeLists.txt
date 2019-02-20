cmake_minimum_required(VERSION 3.1.0)

project(LdapEditorCore)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(/usr/include)
include_directories(../3rd_party/openldap/contrib/ldapcpp/src)

add_compile_options(-DLDAPEDITORCORE_SO)
add_compile_options(-DLDAPEDITORCORE_SO_EXPORTS)
add_compile_options(-std=c++11)
add_compile_options(-Wall)
add_compile_options(-Wextra)

FIND_PACKAGE(Qt5Core CONFIG REQUIRED)
FIND_PACKAGE(Qt5Widgets CONFIG REQUIRED)

set(LdapCore_SRCS
    datatypes.h
    datatypesinternal.h 
    CLdapException.h
    CLdapException.cpp
    CLdapSchema.h
    CLdapSchema.cpp
    cldapserver.h
    cldapserver.cpp
    CLdapSchemaValidator.cpp
    qfunctional.h
    CLdapEntry.h
    CLdapEntry.cpp
    CLdapData.h
    CLdapData.cpp
    CLdapObject.h
    CLdapObject.cpp
    CLdapAttribute.h
    CLdapAttribute.cpp
)

if (APPLE)
    set(LdapCore_SRCS ${LdapCore_SRCS}  sasl_decode64.cpp)
endif(APPLE)

file(GLOB OpenLDAPCpp_SRCS
    "../3rd_party/openldap/contrib/ldapcpp/src/*.cpp"
    "../3rd_party/openldap/contrib/ldapcpp/src/*.h"
)


FIND_LIBRARY(LDAP_LIBRARY NAMES ldap PATHS /usr/lib64)


add_library(${PROJECT_NAME} SHARED ${LdapCore_SRCS} ${OpenLDAPCpp_SRCS})

qt5_use_modules(${PROJECT_NAME} Core Gui Widgets)
target_link_libraries(${PROJECT_NAME} ${LDAP_LIBRARY} )


INSTALL(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
)
