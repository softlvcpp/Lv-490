#ifndef EXPORTCONFIG_H
#define EXPORTCONFIG_H

// Header that just defines export macros

#define EXTERN_C extern "C"

#ifdef _WIN32
// define for .dll
#ifdef LOGGERLIBRARY_EXPORTS
// I use VS to create dynamic library .dll file
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

#else
// define for .so
#define PCH_PRCHEADER
#define INCLUDE_CHECK

#ifdef LOGGERLIBRARY_LIBRARY
// I use qt to create shared library .so file
#include "QtGlobal"
#define LOGGER_API Q_DECL_EXPORT
#else
#define LOGGER_API Q_DECL_IMPORT
#endif

#endif // Export/import defines




#endif // EXPORTCONFIG_H
