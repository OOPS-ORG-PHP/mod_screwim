/*
 * Copyright 2022. JoungKyun.Kim All rights reserved.
 *
 * This file is part of mod_screwim
 *
 * This program is forked from PHP screw who made by Kunimasa Noda in
 * PM9.com, Inc. and, his information is follows:
 *   http://www.pm9.com
 *   kuni@pm9.com
 *   https://sourceforge.net/projects/php-screw/
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PHP_SCREWIM_H
#define PHP_SCREWIM_H

#define SCREWIM     "\tSCREWIM DATA\t"
#define SCREWIM_LEN 14

/* If build PHP module {{{
 */
#ifndef NO_PHP_MOD

//extern zend_module_entry screwim_module_entry;
//#define phpext_screwim_ptr &screwim_module_entry

PHP_MINIT_FUNCTION (screwim);
PHP_MSHUTDOWN_FUNCTION (screwim);
PHP_MINFO_FUNCTION (screwim);

PHP_FUNCTION(screwim_encrypt);
#ifdef SCREWIM_DECRYPT
PHP_FUNCTION(screwim_decrypt);
PHP_FUNCTION(screwim_seed);
#endif

ZEND_BEGIN_MODULE_GLOBALS(screwim)
	zend_bool enabled;
ZEND_END_MODULE_GLOBALS(screwim)

#ifdef ZTS
# define SCREWIM_G(v) TSRMG(screwim_globals_id, zend_screwim_globals *, v)
# ifdef COMPILE_DL_SCREWIM
	ZEND_TSRMLS_CACHE_EXTERN()
# endif
#else
# define SCREWIM_G(v) (screwim_globals.v)
#endif

/* for <= PHP 5 {{{ */
#if PHP_VERSION_ID < 60000
	typedef struct _zend_string {
		size_t   len;
		char   * val;
	} zend_string;
	typedef long zend_long;

	#define ZSTR_VAL(zstr) (zstr)->val
	#define ZSTR_LEN(zstr) (zstr)->len
	#define RETURN_STR(str) RETURN_STRINGL(ZSTR_VAL(str),ZSTR_LEN(str),1)
	#define RETVAL_STR(str) RETVAL_STRINGL(ZSTR_VAL(str),ZSTR_LEN(str),1)
	#define RETVAL_SCREWDATA(z) RETVAL_STRINGL(z.buf, z.len, 1)
	#define screwim_add_property_string(rvalue, str, strlength) add_property_string(rvalue, str, strlength, 1)
#else
	#define RETVAL_SCREWDATA(z) RETVAL_STRINGL(z.buf, z.len)
	#define screwim_add_property_string add_property_string
#endif /* }}} */

/* for >= PHP8 {{{
 *
 * Since PHP 8, TSRMLS_DC, TSRMLS_CC, and TSRMLS_C constants have been removed.
 */
#if PHP_VERSION_ID > 79999
	#define TSRMLS_DC
	#define TSRMLS_CC
	#define TSRMLS_C
#endif /* }}} */

#endif // }}} end of build PHP module

char * zdecode (char * inbuf, ULong inbuf_len, ULong * resultbuf_len);
char * zencode (char * inbuf, ULong inbuf_len, ULong * resultbuf_len);
unsigned short revert_endian (size_t x);
short * generate_key (char * p, int no);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
