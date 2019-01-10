CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
PROJECT(LdapEditorApp)

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
SET(CMAKE_AUTOMOC ON)

# Create code from a list of Qt designer ui files
SET(CMAKE_AUTOUIC ON)

# Instruct CMake to run rcc automatically when needed
SET(CMAKE_AUTORCC ON)

# Find the QtWidgets library
FIND_PACKAGE(Qt5Widgets CONFIG REQUIRED)

#include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

include_directories(../corelib)

# Populate a CMake variable with the sources
SET(ldapeditor_SOURCES
    resources.qrc
    datetimeeditdelegate.cpp
    ldapsettings.cpp
    simplecrypt.cpp
    ldaptreemodel.cpp
    ldapattributesmodel.cpp
    ldapsearchdialog.cpp
    ldapsearchdialog.ui
    connectiondialog.cpp
    connectiondialog.ui
    ldaptableview.cpp
    ldaptreeview.cpp
    mainwindow.cpp
    main.cpp
)
SET(ldapeditor_HEADERS
    datetimeeditdelegate.h
    ldapsettings.h
    simplecrypt.h
    ldaptreemodel.h
    ldapattributesmodel.h
    ldapsearchdialog.h
    connectiondialog.h
    ldaptableview.h
    ldaptreeview.h
    mainwindow.h
    ldapeditordefines.h
)

SET(ldapeditor_OTHER
    install/LdapEditorApp.desktop
    install/LdapEditorApp.directory
    install/LdapEditorApp.menu
)

# Tell CMake to create the helloworld executable
ADD_EXECUTABLE(${PROJECT_NAME} ${ldapeditor_SOURCES} ${ldapeditor_HEADERS} ${ldapeditor_OTHER})
target_link_libraries(${PROJECT_NAME} LdapEditorCore)
set_target_properties(${PROJECT_NAME} PROPERTIES
                      INSTALL_RPATH /usr/local/lib
                      INSTALL_RPATH_USE_LINK_PATH TRUE)


# Copy the ldap-editor pixmap
SET(LDAP_EDITOR_DESKTOP_DIR     "/usr/share/applications")
SET(LDAP_EDITOR_PIXMAPS_DIR     "/usr/share/icons")
SET(LDAP_EDITOR_DIRECTORY_DIR   "/usr/share/desktop-directories")
SET(LDAP_EDITOR_MENU_MERGED_DIR "/etc/xdg/menus/application-merged")

INSTALL(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
INSTALL (FILES install/LdapEditorApp.png DESTINATION ${LDAP_EDITOR_PIXMAPS_DIR})
INSTALL (FILES install/LdapEditorApp.desktop DESTINATION ${LDAP_EDITOR_DESKTOP_DIR})
INSTALL (FILES install/LdapEditorApp.menu DESTINATION ${LDAP_EDITOR_MENU_MERGED_DIR})
INSTALL (FILES install/LdapEditorApp.directory DESTINATION ${LDAP_EDITOR_DIRECTORY_DIR})

# try to set up the menu system
#find_program(XDG-MIME_EXECUTABLE xdg-mime)
#FIND_PROGRAM(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)

#INSTALL(CODE "
#  EXECUTE_PROCESS(COMMAND ${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor ${LDAP_EDITOR_DESKTOP_DIR}/ldapeditor.desktop)
#  "
#)

# Set used Qt5 modules
QT5_USE_MODULES(${PROJECT_NAME} Core Gui Widgets)
set(CPACK_CREATE_DESKTOP_LINKS  ${PROJECT_NAME} )




