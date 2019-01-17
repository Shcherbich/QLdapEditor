/* Extracted fuction sasl_decode64 function from original cyrus-sasl project*/

/* saslutil.c
 * Rob Siemborski
 * Tim Martin
 */
/*
 * Copyright (c) 1998-2016 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The name "Carnegie Mellon University" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For permission or any other legal
 *    details, please contact
 *      Carnegie Mellon University
 *      Center for Technology Transfer and Enterprise Creation
 *      4615 Forbes Avenue
 *      Suite 302
 *      Pittsburgh, PA  15213
 *      (412) 268-7393, fax: (412) 268-7395
 *      innovation@andrew.cmu.edu
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef NULL
#define NULL 0L
#endif

/* SASL result codes: */
#define SASL_CONTINUE    1   /* another step is needed in authentication */
#define SASL_OK          0   /* successful result */
#define SASL_FAIL       -1   /* generic failure */
#define SASL_NOMEM      -2   /* memory shortage failure */
#define SASL_BUFOVER    -3   /* overflowed buffer */
#define SASL_NOMECH     -4   /* mechanism not supported */
#define SASL_BADPROT    -5   /* bad protocol / cancel */
#define SASL_NOTDONE    -6   /* can't request info until later in exchange */
#define SASL_BADPARAM   -7   /* invalid parameter supplied */
#define SASL_TRYAGAIN   -8   /* transient failure (e.g., weak key) */
#define SASL_BADMAC -9   /* integrity check failed */
#define SASL_NOTINIT    -12  /* SASL library not initialized */
/* -- client only codes -- */
#define SASL_INTERACT    2   /* needs user interaction */
#define SASL_BADSERV    -10  /* server failed mutual authentication step */
#define SASL_WRONGMECH  -11  /* mechanism doesn't support requested feature */
/* -- server only codes -- */
#define SASL_BADAUTH    -13  /* authentication failure */
#define SASL_NOAUTHZ    -14  /* authorization failure */
#define SASL_TOOWEAK    -15  /* mechanism too weak for this user */
#define SASL_ENCRYPT    -16  /* encryption needed to use mechanism */
#define SASL_TRANS      -17  /* One time use of a plaintext password will
                enable requested mechanism for user */
#define SASL_EXPIRED    -18  /* passphrase expired, has to be reset */
#define SASL_DISABLED   -19  /* account disabled */
#define SASL_NOUSER     -20  /* user not found */
#define SASL_BADVERS    -23  /* version mismatch with plug-in */
#define SASL_UNAVAIL    -24  /* remote authentication server unavailable */
#define SASL_NOVERIFY   -26  /* user exists, but no verifier for user */
/* -- codes for password setting -- */
#define SASL_PWLOCK     -21  /* passphrase locked */
#define SASL_NOCHANGE   -22  /* requested change was not needed */
#define SASL_WEAKPASS   -27  /* passphrase is too weak for security policy */
#define SASL_NOUSERPASS -28  /* user supplied passwords not permitted */
#define SASL_NEED_OLD_PASSWD -29 /* sasl_setpass needs old password in order
                    to perform password change */
#define SASL_CONSTRAINT_VIOLAT  -30 /* a property can't be stored,
                       because of some constrains/policy violation */

#define SASL_BADBINDING -32  /* channel binding failure */
#define SASL_CONFIGERR  -100 /* error when parsing configuration file */

/* max size of a sasl mechanism name */
#define SASL_MECHNAMEMAX 20


#define CHAR64(c)  (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])
//static char basis_64[] =
//   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????";

static char index_64[128] =
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9, 10, 11, 12, 13, 14,
	    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
    };

/* base64 decode
 *  in     -- input data
 *  inlen  -- length of input data
 *  out    -- output data (may be same as in, must have enough space)
 *  outmax  -- max size of output buffer
 * result:
 *  outlen -- actual output length
 *
 * returns:
 * SASL_BADPROT on bad base64,
 * SASL_BUFOVER if result won't fit,
 * SASL_CONTINUE on a partial block,
 * SASL_OK on success
 */

extern "C" int sasl_decode64(const char* in,
                             unsigned inlen,
                             char* out,
                             unsigned outmax,  /* size of the buffer, not counting the NUL */
                             unsigned* outlen)
{
	unsigned len = 0;
	unsigned j;
	int c[4];
	int saw_equal = 0;

	/* check parameters */
	if (out == NULL)
	{
		return SASL_FAIL;
	}

	if (inlen > 0 && *in == '\r')
	{
		return SASL_FAIL;
	}

	while (inlen > 3)
	{
		/* No data is valid after an '=' character */
		if (saw_equal)
		{
			return SASL_BADPROT;
		}

		for (j = 0; j < 4; j++)
		{
			c[j] = in[0];
			in++;
			inlen--;
		}

		if (CHAR64(c[0]) == -1 || CHAR64(c[1]) == -1)
		{
			return SASL_BADPROT;
		}
		if (c[2] != '=' && CHAR64(c[2]) == -1)
		{
			return SASL_BADPROT;
		}
		if (c[3] != '=' && CHAR64(c[3]) == -1)
		{
			return SASL_BADPROT;
		}
		/* No data is valid after a '=' character, unless it is another '=' */
		if (c[2] == '=' && c[3] != '=')
		{
			return SASL_BADPROT;
		}
		if (c[2] == '=' || c[3] == '=')
		{
			saw_equal = 1;
		}

		*out++ = (CHAR64(c[0]) << 2) | (CHAR64(c[1]) >> 4);
		if (++len >= outmax)
		{
			return SASL_BUFOVER;
		}
		if (c[2] != '=')
		{
			*out++ = ((CHAR64(c[1]) << 4) & 0xf0) | (CHAR64(c[2]) >> 2);
			if (++len >= outmax)
			{
				return SASL_BUFOVER;
			}
			if (c[3] != '=')
			{
				*out++ = ((CHAR64(c[2]) << 6) & 0xc0) | CHAR64(c[3]);
				if (++len >= outmax)
				{
					return SASL_BUFOVER;
				}
			}
		}
	}

	*out = '\0'; /* NUL terminate the output string */

	if (outlen)
	{
		*outlen = len;
	}

	if (inlen != 0)
	{
		if (saw_equal)
		{
			/* Unless there is CRLF at the end? */
			return SASL_BADPROT;
		}
		else
		{
			return (SASL_CONTINUE);
		}
	}

	return SASL_OK;
}
