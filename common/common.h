/*!
\file
\brief Header for common utility functions

File contains  declarations for common utility functions
*/

#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>

namespace common 
{
/*!
 * \brief utility function splits string on parts according delimiter
 * \param str source string
 * \param delim delimiter string
 * \return vector of strings
 */
std::vector<std::string> splitString(const std::string& str, const std::string& delim);
} //namespace common

#endif //#define COMMON_H
