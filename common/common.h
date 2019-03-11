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

/*!
 * @ingroup common
 * @brief Auxiliary class for handling switchine boolean flag
 */
class CBooleanAutoFlag
{
    bool& m_flag;
    bool m_initial;
public:
    /*!
     * \brief Constructor CBooleanAutoFlag
     * \param flag reference to boolean flag
     * \param initial initial state
     */
    CBooleanAutoFlag(bool& flag, bool initial):m_flag{flag}, m_initial{initial}
    {
        m_flag = initial;
    }
    /*!
     * \brief Destructor CBooleanAutoFlag
     *
     * Sets flag in !initial state
    */
    ~CBooleanAutoFlag()
    {
        m_flag = !m_initial;
    }
};
} //namespace common

#endif //#define COMMON_H
