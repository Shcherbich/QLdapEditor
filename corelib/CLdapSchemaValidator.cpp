#include <algorithm>
#include <functional>
#include <string>
#include <regex>
#include <tuple>
#include "CLdapSchema.h"
#include "datatypesinternal.h"



// These (DirectoryServer389) functions are copied from 389 Directory Server
// https://releases.pagure.org/389-ds-base/
// https://releases.pagure.org/389-ds-base/389-ds-base-1.4.1.1.tar.bz2
namespace DirectoryServer389
{
char* strnchr(const char* s, int c, size_t count)
{
    for (; count-- && *s != '\0'; ++s)
        if (*s == (char)c)
        {
            return (char*)s;
        }
    return NULL;
}

/* The following are derived from RFC 4512, section 1.4. */
#define IS_LEADKEYCHAR(c) (isalpha(c))
#define IS_KEYCHAR(c)     (isalnum(c) || (c == '-'))
#define IS_SPACE(c)       ((c == ' '))
#define IS_LDIGIT(c)      ((c != '0') && isdigit(c))
#define IS_SHARP(c)       ((c == '#'))
#define IS_DOLLAR(c)      ((c == '$'))
#define IS_SQUOTE(c)      ((c == '\''))
#define IS_ESC(c)         ((c == '\\'))
#define IS_LPAREN(c)      ((c == '('))
#define IS_RPAREN(c)      ((c == ')'))
#define IS_COLON(c)       ((c == ':'))
#define IS_UTF0(c) (((unsigned char)(c) >= (unsigned char)'\x80') && ((unsigned char)(c) <= (unsigned char)'\xBF'))
#define IS_UTF1(c) (!((unsigned char)(c)&128))
/* These are only checking the first byte of the multibyte character.  They
 * do not verify that the entire multibyte character is correct. */
#define IS_UTF2(c)  (((unsigned char)(c) >= (unsigned char)'\xC2') && ((unsigned char)(c) <= (unsigned char)'\xDF'))
#define IS_UTF3(c)  (((unsigned char)(c) >= (unsigned char)'\xE0') && ((unsigned char)(c) <= (unsigned char)'\xEF'))
#define IS_UTF4(c)  (((unsigned char)(c) >= (unsigned char)'\xF0') && ((unsigned char)(c) <= (unsigned char)'\xF4'))
#define IS_UTFMB(c) (IS_UTF2(c) || IS_UTF3(c) || IS_UTF4(c))
#define IS_UTF8(c)  (IS_UTF1(c) || IS_UTFMB(c))

/* The following are derived from RFC 4514, section 3. */
#define IS_ESCAPED(c) ((c == '"') || (c == '+') || (c == ',') || \
                       (c == ';') || (c == '<') || (c == '>'))
#define IS_SPECIAL(c) (IS_ESCAPED(c) || IS_SPACE(c) || \
                       IS_SHARP(c) || (c == '='))
#define IS_LUTF1(c) (IS_UTF1(c) && !IS_ESCAPED(c) && !IS_SPACE(c) && \
                     !IS_SHARP(c) && !IS_ESC(c))
#define IS_TUTF1(c) (IS_UTF1(c) && !IS_ESCAPED(c) && !IS_SPACE(c) && \
                     !IS_ESC(c))
#define IS_SUTF1(c) (IS_UTF1(c) && !IS_ESCAPED(c) && !IS_ESC(c))


/* Helper function for processing a 'keystring'.
 *
 * Returns 0 is the value between begin and end is a valid 'keystring'.
 * Returns non-zero if the value is not a valide 'keystring'.
 */
int
keystring_validate(
    const char* begin,
    const char* end)
{
    int rc = 0; /* assume the value is valid */
    const char* p = begin;

    if ((begin == NULL) || (end == NULL))
    {
        rc = 1;
        goto exit;
    }

    /* Per RFC4512:
     *
     *   keystring = leadkeychar *keychar
     */
    if (IS_LEADKEYCHAR(*p))
    {
        for (p++; p <= end; p++)
        {
            if (!IS_KEYCHAR(*p))
            {
                rc = 1;
                goto exit;
            }
        }
    }
    else
    {
        rc = 1;
        goto exit;
    }

exit:
    return (rc);
}

/* Helper function for processing a 'numericoid'.
 *
 * Returns 0 is the value between begin and end is a valid 'numericoid'.
 * Returns non-zero if the value is not a valide 'numericoid'.
 */
int numericoid_validate(const char* begin, const char* end)
{
    int rc = 0; /* assume the value is valid */
    int found_separator = 0;
    const char* p = NULL;

    if ((begin == NULL) || (end == NULL))
    {
        rc = 1;
        goto exit;
    }

    /* Per RFC 4512:
     *
     *   numericoid = number 1*( DOT number )
     */

    /* one pass of this loop should process one element of the oid (number DOT) */
    for (p = begin; p <= end; p++)
    {
        if (IS_LDIGIT(*p))
        {
            /* loop until we get to a separator char */
            while (*p != '.')
            {
                p++;
                if (p > end)
                {
                    /* ensure we got at least 2 elements */
                    if (!found_separator)
                    {
                        rc = 1;
                        goto exit;
                    }
                    else
                    {
                        /* looks like a valid numericoid */
                        goto exit;
                    }
                }
                else if (*p == '.')
                {
                    /* we can not end with a '.' */
                    if (p == end)
                    {
                        rc = 1;
                        goto exit;
                    }
                    else
                    {
                        found_separator = 1;
                    }
                }
                else if (!isdigit(*p))
                {
                    rc = 1;
                    goto exit;
                }
            }
        }
        else if (*p == '0')
        {
            p++;
            if (p > end)
            {
                /* ensure we got at least 2 elements */
                if (!found_separator)
                {
                    rc = 1;
                    goto exit;
                }
                else
                {
                    /* looks like a valid numericoid */
                    goto exit;
                }
            }
            else if (*p != '.')
            {
                /* a leading 0 is not allowed unless the entire element is simply 0 */
                rc = 1;
                goto exit;
            }

            /* At this point, *p is '.'.  We can not end with a '.' */
            if (p == end)
            {
                rc = 1;
                goto exit;
            }
            else
            {
                found_separator = 1;
            }
        }
        else
        {
            rc = 1;
            goto exit;
        }
    }

exit:
    return (rc);
}


/* Helper to validate a single UTF-8 character.
 * It is assumed that the first byte of the character
 * is pointed to by begin.  This function will not read
 * past the byte pointed to by the end parameter.  The
 * last pointer will be filled in the the address of
 * the last byte of the validated character if the
 * character is valid, or the last byte processed
 * in the invalid case.
 *
 * Returns 0 if it is valid and non-zero otherwise. */
int
utf8char_validate(
    const char* begin,
    const char* end,
    const char** last)
{
    int rc = 0; /* Assume char is valid */
    const char* p = begin;

    if ((begin == NULL) || (end == NULL))
    {
        rc = 1;
        goto exit;
    }

    /* Per RFC 4512:
     *
     *   UTF8  = UTF1 / UTFMB
     *   UTFMB = UTF2 / UTF3 / UTF4
     *   UTF0  = %x80-BF
     *   UTF1  = %x00-7F
     *   UTF2  = %xC2-DF UTF0
     *   UTF3  = %xE0 %xA0-BF UTF0 / %xE1-EC 2(UTF0) /
     *           %xED %x80-9F UTF0 / %xEE-EF 2(UTF0)
     *   UTF4  = %xF0 %x90-BF 2(UTF0) / %xF1-F3 3(UTF0) /
     *           %xF4 %x80-8F 2(UTF0)
     */

    /* If we have a single byte (ASCII) character, we
     * don't really have any work to do. */
    if (IS_UTF1(*p))
    {
        goto exit;
    }
    else if (IS_UTF2(*p))
    {
        /* Ensure that there is another byte
         * and that is is 'UTF0'. */
        if ((p == end) || !IS_UTF0(*(p + 1)))
        {
            rc = 1;
            goto exit;
        }

        /* Advance p so last is set correctly */
        p++;
    }
    else if (IS_UTF3(*p))
    {
        /* Ensure that there are at least 2 more bytes. */
        if (end - p < 2)
        {
            rc = 1;
            goto exit;
        }

        /* The first byte determines what is legal for
         * the second byte. */
        if (*p == '\xE0')
        {
            /* The next byte must be %xA0-BF. */
            p++;
            if (((unsigned char)*p < (unsigned char)'\xA0') || ((unsigned char)*p > (unsigned char)'\xBF'))
            {
                rc = 1;
                goto exit;
            }
        }
        else if (*p == '\xED')
        {
            /* The next byte must be %x80-9F. */
            p++;
            if (((unsigned char)*p < (unsigned char)'\x80') || ((unsigned char)*p > (unsigned char)'\x9F'))
            {
                rc = 1;
                goto exit;
            }
        }
        else
        {
            /* The next byte must each be 'UTF0'. */
            p++;
            if (!IS_UTF0(*p))
            {
                rc = 1;
                goto exit;
            }
        }

        /* The last byte must be 'UTF0'. */
        p++;
        if (!IS_UTF0(*p))
        {
            rc = 1;
            goto exit;
        }
    }
    else if (IS_UTF4(*p))
    {
        /* Ensure that there are at least 3 more bytes. */
        if (end - p < 3)
        {
            rc = 1;
            goto exit;
        }

        /* The first byte determines what is legal for
         * the second byte. */
        if (*p == '\xF0')
        {
            /* The next byte must be %x90-BF. */
            p++;
            if (((unsigned char)*p < (unsigned char)'\x90') || ((unsigned char)*p > (unsigned char)'\xBF'))
            {
                rc = 1;
                goto exit;
            }
        }
        else if (*p == '\xF4')
        {
            /* The next byte must be %x80-BF. */
            p++;
            if (((unsigned char)*p < (unsigned char)'\x80') || ((unsigned char)*p > (unsigned char)'\xBF'))
            {
                rc = 1;
                goto exit;
            }
        }
        else
        {
            /* The next byte must each be 'UTF0'. */
            p++;
            if (!IS_UTF0(*p))
            {
                rc = 1;
                goto exit;
            }
        }

        /* The last 2 bytes must be 'UTF0'. */
        p++;
        if (!IS_UTF0(*p) || !IS_UTF0(*(p + 1)))
        {
            rc = 1;
            goto exit;
        }

        /* Advance the pointer so last is set correctly
         * when we return. */
        p++;
    }
    else
    {
        /* We found an illegal first byte. */
        rc = 1;
        goto exit;
    }

exit:
    if (last)
    {
        *last = (const char*)p;
    }
    return (rc);
}


/*
 * Helper function for validating a DN.  This function will validate
 * a single RDN.  If the RDN is valid, 0 will be returned, otherwise
 * non-zero will be returned. A pointer to the last character processed
 * will be set in the "last parameter.  This will be the end of the RDN
 * in the valid case, and the illegal character in the invalid case.
 */
int
rdn_validate(const char* begin, const char* end, const char** last)
{
    int rc = 0; /* Assume RDN is valid */
    int numericform = 0;
    char* separator = NULL;
    const char* p = begin;

    /* Find the '=', then use the helpers for descr and numericoid */
    if ((separator = strnchr(p, '=', end - begin + 1)) == NULL)
    {
        rc = 1;
        goto exit;
    }

    /* Process an attribute type. The 'descr'
     * form must start with a 'leadkeychar'. */
    if (IS_LEADKEYCHAR(*p))
    {
        if ((rc = keystring_validate(p, separator - 1)))
        {
            goto exit;
        }
        /* See if the 'numericoid' form is being used */
    }
    else if (isdigit(*p))
    {
        numericform = 1;
        if ((rc = numericoid_validate(p, separator - 1)))
        {
            goto exit;
        }
    }
    else
    {
        rc = 1;
        goto exit;
    }

    /* Advance the pointer past the '=' and make sure
     * we're not past the end of the string. */
    p = separator + 1;
    if (p > end)
    {
        rc = 1;
        goto exit;
    }

    /* The value must be a 'hexstring' if the 'numericoid'
     * form of 'attributeType' is used.  Per RFC 4514:
     *
     *   hexstring = SHARP 1*hexpair
     *   hexpair = HEX HEX
     */
    if (numericform)
    {
        if ((p == end) || !IS_SHARP(*p))
        {
            rc = 1;
            goto exit;
        }
        p++;
        /* The value must be a 'string' when the 'descr' form
        * of 'attributeType' is used.  Per RFC 4514:
        *
        *   string = [ ( leadchar / pair ) [ *( stringchar / pair )
        *      ( trailchar / pair ) ] ]
        *
        *   leadchar   = LUTF1 / UTFMB
        *   trailchar  = TUTF1 / UTFMB
        *   stringchar = SUTF1 / UTFMB
        *
        *   pair = ESC (ESC / special / hexpair )
        *   special = escaped / SPACE / SHARP / EQUALS
        *   escaped = DQUOTE / PLUS / COMMA / SEMI / LANGLE / RANGLE
        *   hexpair = HEX HEX
        */
    }
    else
    {
        /* Check the leadchar to see if anything illegal
         * is there.  We need to allow a 'pair' to get
         * through, so we'll assume that a '\' is the
         * start of a 'pair' for now. */
        if (IS_UTF1(*p) && !IS_ESC(*p) && !IS_LUTF1(*p))
        {
            rc = 1;
            goto exit;
        }
    }

    /* Loop through string until we find the ',' separator, a '+'
     * char indicating a multi-value RDN, or we reach the end.  */
    while ((p <= end) && (*p != ',') && (*p != '+'))
    {
        if (numericform)
        {
            /* Process a single 'hexpair' */
            if ((p == end) || !isxdigit(*p) || !isxdigit(*p + 1))
            {
                rc = 1;
                goto exit;
            }
            p = p + 2;
        }
        else
        {
            /* Check for a valid 'stringchar'.  We handle
             * multi-byte characters separately. */
            if (IS_UTF1(*p))
            {
                /* If we're at the end, check if we have
                 * a valid 'trailchar'. */
                if ((p == end) && !IS_TUTF1(*p))
                {
                    rc = 1;
                    goto exit;
                    /* Check for a 'pair'. */
                }
                else if (IS_ESC(*p))
                {
                    /* We're guaranteed to still have at
                     * least one more character, so lets
                     * take a look at it. */
                    p++;
                    if (!IS_ESC(*p) && !IS_SPECIAL(*p))
                    {
                        /* The only thing valid now
                         * is a 'hexpair'. */
                        if ((p == end) || !isxdigit(*p) || !isxdigit(*p + 1))
                        {
                            rc = 1;
                            goto exit;
                        }
                        p++;
                    }
                    /* Only allow 'SUTF1' chars now. */
                }
                else if (!IS_SUTF1(*p))
                {
                    rc = 1;
                    goto exit;
                }

                p++;
            }
            else
            {
                /* Validate a single 'UTFMB' (multi-byte) character. */
                if (utf8char_validate(p, end, &p) != 0)
                {
                    rc = 1;
                    goto exit;
                }

                /* Advance the pointer past the multi-byte char. */
                p++;
            }
        }
    }

    /* We'll end up either at the comma, a '+', or one past end.
     * If we are processing a multi-valued RDN, we recurse to
     * process the next 'attributeTypeAndValue'. */
    if ((p <= end) && (*p == '+'))
    {
        /* Make sure that there is something after the '+'. */
        if (p == end)
        {
            rc = 1;
            goto exit;
        }
        p++;

        /* Recurse to process the next value.  We need to reset p to
         * ensure that last is set correctly for the original caller. */
        rc = rdn_validate(p, end, last);
        p = *last + 1;
    }

exit:
    *last = p - 1;
    return rc;
}

/*
 * Validates a distinguishedName as degined in RFC 4514.  Returns
 * 0 if the value from begin to end is a valid distinguishedName.
 * Returns 1 otherwise.
 */
int
distinguishedname_validate(
    const char* begin,
    const char* end)
{
    int rc = 0; /* Assume value is valid */
    const char* p = begin;
    const char* last = NULL;

    /* Per RFC 4514:
     *
     * distinguishedName = [ relativeDistinguishedName
     *     *( COMMA relativeDistinguishedName ) ]
     * relativeDistinguishedName = attributeTypeAndValue
     *     *( PLUS attributeTypeAndValue )
     * attributeTypeAndValue = attribyteType EQUALS attributeValue
     * attributeType = descr / numericoid
     * attributeValue = string / hexstring
     */

    /* Validate one RDN at a time in a loop. */
    while (p <= end)
    {
        if ((rc = rdn_validate(p, end, &last)) != 0)
        {
            goto exit;
        }
        p = last + 1;

        /* p should be pointing at a comma, or one past
         * the end of the entire dn value.  If we have
         * not reached the end, ensure that the next
         * character is a comma and that there is at
         * least another character after the comma. */
        if ((p <= end) && ((p == end) || (*p != ',')))
        {
            rc = 1;
            goto exit;
        }

        /* Advance the pointer past the comma so it
         * points at the beginning of the next RDN
         * (if there is one). */
        p++;
    }

exit:
    return rc;
}
}

namespace ldapcore
{

struct CLdapSchemaImpl;

using namespace std;
using namespace std::placeholders;



std::string CheckBitString(std::string value)
{
    if (value.size() < 3)
    {
        return "The value is very small";
    }
    if (value[0] != '\'' ||
        value[value.size() - 2] != '\'' ||
        value[value.size() - 1] != 'B')
    {
        return "Incorrect syntax. Must be as '0110'B";
    }
    for (size_t i = 0; i < value.size() - 2; i++)
	{
		auto ch = value.at(i);
		bool b1 = ch == '0';
		bool b2 = ch == '1';
		if (!b1 && !b2)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}

std::string CheckBool(std::string value)
{
	bool b1 = value == "TRUE";
	bool b2 = value == "FALSE";
    return (b1 || b2) ? std::string("") : std::string("The value '")  + value + "' is invalid";;
}


static std::string validSetOfCountry  = "'()+ -.,/:?";
static std::string validSetOfCountry2 = "'()+ -.,/:?$";

std::string CheckCountryStringImpl(std::string value, std::string extendedSet, bool checkSizeOn2)
{
    if (checkSizeOn2 && value.size() != 2)
    {
        return "Incorrect syntax. Length must be equal 2";
    }
	for (size_t i = 0; i < value.size(); i++)
	{
		auto ch = value.at(i);
		bool b1 = isalpha(ch);
		bool b2 = isdigit(ch);
		bool b3 = extendedSet.find(ch) != std::string::npos;
		if (!b1 && !b2 && !b3)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}


std::string CheckOctet(std::string value)
{
	static std::string validSet1 =  "abcdef";
	static std::string validSet2 =  "ABCDEF";

	for (size_t i = 0; i < value.size(); i++)
	{
		auto ch = value.at(i);
		bool b1 = isdigit(ch);
		bool b2 = validSet1.find(ch) != std::string::npos;
		bool b3 = validSet2.find(ch) != std::string::npos;
		if (!b1 && !b2 && !b3)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}

std::string CheckUTF8(std::string value)
{
    QString temp = QString::fromUtf8(value.c_str());
    return temp.length() == (int)value.size() ? "" : "Invalid input string";
}

std::string CheckGeneralizedTime(std::string value)
{
    /*
     *Valid formats
     *
        yyyymmddhhmmss.ffffff (local time)
        yyyymmddhhmmss.ffffffZ (GMT)
        yyyymmddhhmmss.ffffff-hhmm (Time zone west)
        yyyymmddhhmmss.ffffff+hhmm (Time zone east)
        The seconds (ss) and microseconds (ffffff) can be omitted and defaults to 0.
    */

    QRegExp timeFormat("^[0-9]{14}\\.([0-9]{1,6}|[0-9]{1,6}Z|[0-9]{1,6}\\-[0-9]{4}|[0-9]{1,6}\\+[0-9]{4})$");
    timeFormat.indexIn(value.c_str());
    QStringList listName = timeFormat.capturedTexts();
    return listName.length() > 1 ? "" : "There is not a generalized time";
}

std::string CheckUTCTime(std::string value)
{
    /*
     *Valid formats
     *
        yymmddhhmmss.ffffff (local time)
        yymmddhhmmss.ffffffZ (GMT)
        yymmddhhmmss.ffffff-hhmm (Time zone west)
        yymmddhhmmss.ffffff+hhmm (Time zone east)
        The seconds (ss) and microseconds (ffffff) can be omitted and defaults to 0.
    */

    QRegExp timeFormat("^[0-9]{12}\\.([0-9]{1,6}|[0-9]{1,6}Z|[0-9]{1,6}\\-[0-9]{4}|[0-9]{1,6}\\+[0-9]{4})$");
    timeFormat.indexIn(value.c_str());
    QStringList listName = timeFormat.capturedTexts();
    return listName.length() > 1 ? "" : "There is not a generalized time";
}


#define LDAP_ASCII(c)       (!((c) & 0x80))

std::string CheckIA5String(std::string value)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        auto ch = value.at(i);
        bool b1 = LDAP_ASCII(ch);
        if (!b1)
        {
            return std::string("The symbol '") + ch + "' is invalid";
        }
    }
    return std::string("");
}

std::string Check62Integer(std::string value)
{
    bool ok;
    QString(value.c_str()).toLong(&ok, 10);
    return ok ? "" : "There is not a +/- 62 digit integer";
}

std::string CheckNumericString(std::string value)
{
    QStringList list = QString(value.c_str()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (auto item : list)
    {
        bool ok;
        item.toLong(&ok, 10);
        if (!ok)
        {
            return QString("%1 is not a +/- 62 digit integer").arg(item).toStdString();
        }
    }
    return "";
}

std::string CheckDnString(std::string value)
{
    if (!value.size())
    {
        return "Too small string";
    }
    auto ret = DirectoryServer389::distinguishedname_validate(value.c_str(), value.c_str() + value.size());
    return ret == 0 ? "" : "There is not a distinguished name";
}

struct validateInfo
{
    std::string type;
    std::function< std::string(std::string)> funcToValidate;
};

static std::map<std::string, validateInfo > syntaxNumber2toCheckFunction
{
    {"1.3.6.1.4.1.1466.115.121.1.5",  {"Binary", CheckOctet}},
    {"1.3.6.1.4.1.1466.115.121.1.6",  {"Bit String", CheckBitString}},
    {"1.3.6.1.4.1.1466.115.121.1.7",  {"Boolean", CheckBool}},
    {"1.3.6.1.4.1.1466.115.121.1.11", {"Country String", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, true)}},
    {"1.3.6.1.4.1.1466.115.121.1.12", {"Distinguished Name", CheckDnString}},
    {"1.3.6.1.4.1.1466.115.121.1.14", {"Delivery Method", CheckUTF8}},
    {"1.3.6.1.4.1.1466.115.121.1.15", {"Directory String", CheckUTF8}},
    {"1.3.6.1.4.1.1466.115.121.1.22", {"Facsimile Telephone Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry2, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.24", {"Generalized Time	", CheckGeneralizedTime}},
    {"1.3.6.1.4.1.1466.115.121.1.26", {"IA5 String", CheckIA5String}},
    {"1.3.6.1.4.1.1466.115.121.1.27", {"Integer", Check62Integer}},
    {"1.3.6.1.4.1.1466.115.121.1.36", {"Numeric String", CheckNumericString}},
    {"1.3.6.1.4.1.1466.115.121.1.40", {"Octet String", CheckOctet}},
    {"1.3.6.1.4.1.1466.115.121.1.44", {"Printable String", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.50", {"Telephone Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.52", {"Telex Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry2, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.53", {"UTC Time", CheckUTCTime}},
};


void CLdapSchema::validateAttributeByName(std::string attributeName, std::string value) noexcept(false)
{
	auto info = m_impl->attr2info.find(attributeName);
	if (info == m_impl->attr2info.end())
	{
		return;
	}
	auto f = syntaxNumber2toCheckFunction.find(std::get<2>(info->second));
	if (f != syntaxNumber2toCheckFunction.end())
	{
        auto checkReturn = f->second.funcToValidate(value);
		if (!checkReturn.empty())
		{
            auto err = QString("The '%1' is not '%2' of '%3'.\nDescription: %4")
                       .arg(value.c_str()).arg(f->second.type.c_str()).arg(attributeName.c_str()).arg(checkReturn.c_str());
            throw CLdapMatchRuleException(err.toStdString().c_str());
		}
	}
}

}

