// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define LDAPEDITORCORE_SO_HELPER_IMPORT __declspec(dllimport)
  #define LDAPEDITORCORE_SO_HELPER_EXPORT __declspec(dllexport)
  #define LDAPEDITORCORE_SO_HELPER_LOCAL
#else
  #if __GNUC__ >= 4
    #define LDAPEDITORCORE_SO_HELPER_IMPORT __LDAPEDITORCORE_SO_HELPER_IMPORT __attribute__ ((visibility ("default")))
    #define LDAPEDITORCORE_SO_HELPER_EXPORT __LDAPEDITORCORE_SO_HELPER_EXPORT __attribute__ ((visibility ("default")))
    #define LDAPEDITORCORE_SO_HELPER_LOCAL  __LDAPEDITORCORE_SO_HELPER_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define LDAPEDITORCORE_SO_HELPERLDAPEDITORCORE_SO_HELPER_IMPORT
    #define LDAPEDITORCORE_SO_HELPER_EXPORT
    #define LDAPEDITORCORE_SO_HELPER_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define FOX_API and FOX_LOCAL.
// FOX_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// FOX_LOCAL is used for non-api symbols.

#ifdef LDAPEDITORCORE_SO // defined if FOX is compiled as a DLL
  #ifdef LDAPEDITORCORE_SO_EXPORTS // defined if we are building the FOX DLL (instead of using it)
    #define LDAPEDITORCORE_SO_API LDAPEDITORCORE_SO_HELPER_EXPORT
  #else
    #define LDAPEDITORCORE_SO_API LDAPEDITORCORE_SO_HELPER_IMPORT
  #endif // LDAPEDITORCORE_SO_EXPORTS
  #define LDAPEDITORCORE_SO_LOCAL LDAPEDITORCORE_SO_HELPER_LOCAL
#else // FOX_DLL is not defined: this means FOX is a static lib.
  #define LDAPEDITORCORE_SO_API
  #define LDAPEDITORCORE_SO_LOCAL
#endif // LDAPEDITORCORE_SO
