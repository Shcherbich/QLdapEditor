CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
PROJECT(QLdapEditor)

SET(PRODUCT "LDAP Editor")
SET(PACKAGENAME "ldapeditor")
SET(COMPANY "Very Big Company Inc.")

SET(CMAKE_VERSION_MAJOR 0)
SET(CMAKE_VERSION_MINOR 5)
SET(CMAKE_VERSION_PATCH 52)


SET(VERSION "${CMAKE_VERSION_MAJOR}.${CMAKE_VERSION_MINOR}.${CMAKE_VERSION_PATCH}")

FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
"/*CMAKE Generated file, do not edit*/\n
\#define VERSION \"@VERSION@\"\n\
\#define PRODUCT \"@PRODUCT@\"\n\
\#define COMPANY \"@COMPANY@\"\n\
"
)
configure_file(${CMAKE_BINARY_DIR}/version.h.in ${CMAKE_BINARY_DIR}/version.h)



SET(APP_DEFAULT_LOCATION_DIR "/usr/local/bin/${PACKAGE_NAME}" )
SET(SO_DEFAULT_LOCATION_DIR "/usr/local/lib/${PACKAGE_NAME}" )
SET(PACKAGEGROUP "Utilities")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "Install path prefix, prepended onto install directories."  FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#include_directories(common)
add_subdirectory(corelib)
add_subdirectory(editor)


#INSTALL (DIRECTORY 3rd_party/openldap/lib/ DESTINATION lib)

#CPack options
SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_GENERATOR "RPM")
SET(CPACK_PACKAGE_NAME ${PACKAGENAME})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PRODUCT} tool for view/edit LDAP directories.")
SET(CPACK_RPM_PACKAGE_LICENSE "GPLv2")
SET(CPACK_RPM_PACKAGE_GROUP ${PACKAGEGROUP})
SET(CPACK_PACKAGE_RELEASE 1)
SET(CPACK_PACKAGE_CONTACT "Dmitry Markevich <d_markevich@bte.ae>")
SET(CPACK_PACKAGE_VENDOR ${COMPANY})
#SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

#important! fix for building RPM - removed dependences of install path from filesystem package CentOS
SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
SET(CPACK_PACKAGE_DEFAULT_LOCATION bin)
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} "LDAP Editor")
LIST(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST "/usr"
    "/usr/local"
    "/usr/local/bin"
    "/usr/local/lib"
    "/usr/share"
    "/usr/share/applications"
    "/usr/share/desktop-directories"
    "/usr/share/icons"
    "/opt"
    "/etc"
    "/etc/xdg"
    "/etc/xdg/menus"
    "/etc/xdg/menus/applications-merged"
)


INCLUDE(CPack)
