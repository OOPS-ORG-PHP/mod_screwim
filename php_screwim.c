/*
 * Copuright (c) 2016 JoungKyun.Kim <http://oops.org>. All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned long  ULong; /* 32 bits or more */

#include "php_screwim.h"

ZEND_DECLARE_MODULE_GLOBALS(screwim)

/* {{{ php function argument info
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_screwim_encrypt, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

#ifdef SCREWIM_DECRYPT
ZEND_BEGIN_ARG_INFO_EX(arginfo_screwim_decrypt, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, heder_len)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ +-- screwim_functions[]
 *
 * Every user visible function must have an entry in screwim_functions[].
 */
const zend_function_entry screwim_functions[] = {
	PHP_FE(screwim_encrypt, arginfo_screwim_encrypt)
#ifdef SCREWIM_DECRYPT
	PHP_FE(screwim_decrypt, arginfo_screwim_decrypt)
	PHP_FE(screwim_seed, NULL)
#endif
	{NULL, NULL, NULL}
};
/* }}} */

typedef struct screw_data {
	void * buf;
	size_t len;
} SCREWData;

/* {{{ +-- Internal APIs
 */

/* {{{ +-- static void php_screwim_init_globals(zend_screwim_globals *screwim_globals)
 */
static void php_screwim_init_globals(zend_screwim_globals *screwim_globals) {
	screwim_globals->enabled = 0;
}
/* }}} */

/* {{{ +-- SCREWData screwdata_init (void)
 */
SCREWData screwdata_init (void) {
	SCREWData data;

	data.buf = NULL;
	data.len = 0;

	return data;
}
/* }}} */

/* {{{ +-- SCREWData mcryptkey (char * key)
 */
SCREWData mcryptkey (char * key) {
	SCREWData sdata;
	short * buf;
	int i;
	short screwim_mycryptkey[] = {
		SCREWIM_ENC_DATA
	};

	if ( key != NULL && strlen (key) > 0 ) {
		sdata.len = strlen (key) / 4;
		if ( (strlen (key) % 4) != 0 )
			sdata.len++;

		buf = generate_key (key, sdata.len);
	} else {
		sdata.len = sizeof (screwim_mycryptkey) / sizeof (short);
		buf = (short *) emalloc (sizeof (screwim_mycryptkey));

		for ( i=0; i<sdata.len; i++ )
			buf[i] = screwim_mycryptkey[i];

	}
	sdata.buf = (short *) buf;

	return sdata;
}
/* }}} */

/* {{{ +-- SCREWData screwim_ext_buf (char * datap, ULong datalen, char * ukey)
 */
SCREWData screwim_ext_buf (char * datap, ULong datalen, char * ukey) {
	SCREWData   sdata;
	SCREWData   key;
	int         i;
	short     * keybuf;

	key = mcryptkey (ukey);
	keybuf = (short *) key.buf;

	for( i=0; i<datalen; i++ ) {
		datap[i] = (char) keybuf[(datalen - i) % key.len] ^ (~(datap[i]));
	}

	efree (key.buf);

	sdata = screwdata_init ();
	sdata.buf = (char *) zdecode (datap, datalen, &sdata.len);

	return sdata;
}
/* }}} */

/* {{{ +-- SCREWData screwim_ext_fopen (FILE * fp)
 */
SCREWData screwim_ext_fopen (FILE * fp) {
	struct      stat stat_buf;
	SCREWData   sdata;
	char      * datap = NULL;
	ULong       datalen;

	fstat (fileno (fp), &stat_buf);
	datalen = stat_buf.st_size - SCREWIM_LEN;
	datap = (char *) emalloc (datalen);
	memset (datap, 0, datalen);
	fread (datap, datalen, 1, fp);
	fclose (fp);

	sdata = screwim_ext_buf (datap, datalen, NULL);
	efree (datap);

	return sdata;
}
/* }}} */

#if PHP_VERSION_ID < 70400
/* {{{ +-- SCREWData screwim_ext_mmap (zend_file_handle * file_handle)
 */
SCREWData screwim_ext_mmap (zend_file_handle * file_handle) {
	SCREWData   sdata;
	char      * nbuf;
	char      * datap = NULL;
	ULong       datalen;

	nbuf = file_handle->handle.stream.mmap.buf + SCREWIM_LEN;
	datalen = file_handle->handle.stream.mmap.len - SCREWIM_LEN; 

	datap = (char *) emalloc (datalen);
	memset (datap, 0, datalen);
	memcpy (datap, nbuf, datalen);

	sdata = screwim_ext_buf (datap, datalen, NULL);
	efree (datap);

	return sdata;
}
/* }}} */
#endif

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle * file_handle, int type TSRMLS_DC);

// {{{ +-- ZEND_API zend_op_array * screwim_compile_file (zend_file_handle *, int TSRMLS_DC)
ZEND_API zend_op_array * screwim_compile_file (zend_file_handle * file_handle, int type TSRMLS_DC)
{
	FILE      * fp;
	char        buf[SCREWIM_LEN + 1] = { 0, };
	char        fname[32] = { 0, };
	SCREWData   sdata, tmp;

	if ( ! SCREWIM_G (enabled) )
		return org_compile_file (file_handle, type TSRMLS_CC);

#if PHP_VERSION_ID < 70400
	// If file_handle->type is ZEND_HANDLE_MAPPED, handle.stream.mmap.buf has already
	// contents data. This case is include or require. when file is directly opened,
	// handle.stream.mmap.buf has NULL.
	if ( file_handle->type == ZEND_HANDLE_MAPPED ) {
		if ( file_handle->handle.stream.mmap.len > 0 ) {
			memcpy (buf, file_handle->handle.stream.mmap.buf, SCREWIM_LEN);
			if ( memcmp (buf, SCREWIM, SCREWIM_LEN) != 0 )
				return org_compile_file (file_handle, type TSRMLS_CC);
		} else
			return org_compile_file (file_handle, type TSRMLS_CC);
	}
#endif

	if ( zend_is_executing (TSRMLS_C) ) {
		if ( get_active_function_name (TSRMLS_C) ) {
			strncpy (fname, get_active_function_name (TSRMLS_C), sizeof (fname - 2));
		}
	}
	if ( fname[0] ) {
		if ( strcasecmp (fname, "show_source") == 0 || strcasecmp (fname, "highlight_file") == 0) {
			return NULL;
		}
	}

#if PHP_VERSION_ID < 70400
	if ( file_handle->type == ZEND_HANDLE_MAPPED ) {
		sdata = screwim_ext_mmap (file_handle);
	} else {
#endif
		// When file is opened directly (type is ZEND_HANDLE_FP), check here.

		fp = fopen (file_handle->filename, "rb");
		if ( ! fp ) {
			return org_compile_file (file_handle, type TSRMLS_CC);
		}

		fread (buf, sizeof (char), SCREWIM_LEN, fp);

		if ( memcmp (buf, SCREWIM, SCREWIM_LEN) != 0 ) {
			fclose (fp);
			return org_compile_file (file_handle, type TSRMLS_CC);
		}

		sdata = screwim_ext_fopen (fp);
#if PHP_VERSION_ID < 70400
	}
#endif

	tmp = screwdata_init ();

	if ( sdata.buf == NULL )
		return NULL;

	if ( zend_stream_fixup (file_handle, (char **) &tmp.buf, &tmp.len TSRMLS_CC) == FAILURE )
		return NULL;

#if PHP_VERSION_ID < 70400
	file_handle->handle.stream.mmap.buf = sdata.buf;
	file_handle->handle.stream.mmap.len = sdata.len;
#else
	file_handle->buf = sdata.buf;
	file_handle->len = sdata.len;
#endif
	file_handle->handle.stream.closer = NULL;

	return org_compile_file (file_handle, type TSRMLS_CC);
}
/* }}} */

/* {{{ for PHP 5 APIs
 */

#if PHP_VERSION_ID < 60000
/* {{{ +-- zend_string * zend_string_alloc (size_t len, int persis)
 */
zend_string * zend_string_alloc (size_t len, int persis) {
	zend_string * buf;

	buf = (zend_string *) emalloc (sizeof (zend_string));
	buf->val = (char *) emalloc (sizeof (char) * len + 1);
	memset (buf->val, 0, sizeof (char) * len + 1);
	buf->len = len;

	return buf;
}
/* }}} */

/* {{{ +-- void zend_string_free (zend_string * buf)
 */
void zend_string_free (zend_string * buf) {
	if ( buf == NULL )
		return;

	if ( buf->val != NULL ) {
		efree (buf->val);
		buf->val = NULL;
	}

	efree (buf);
}
/* }}} */
#endif

/* End of PHP 5 APIs }}} */

/* end of internal APIs }}} */

/* {{{ +-- PHP_FUNCTION (string) screwim_encrypt (string)
 */
PHP_FUNCTION (screwim_encrypt) {
	zend_string * text;
	zend_string * ndata;
	char        * datap;
	ULong         datalen;
	int           i;

	SCREWData     key;
	short       * keybuf;

#if PHP_VERSION_ID < 60000
	text = (zend_string *) emalloc (sizeof (zend_string));
	ZSTR_VAL(text) = NULL;
	ZSTR_LEN(text) = 0;
	if ( zend_parse_parameters (ZEND_NUM_ARGS(), "s", &ZSTR_VAL(text), &ZSTR_LEN(text)) == FAILURE )
#else
	if ( zend_parse_parameters (ZEND_NUM_ARGS(), "S", &text) == FAILURE )
#endif
	{
#if PHP_VERSION_ID < 60000
		zend_string_free (text);
#endif
		return;
	}

	if ( ZSTR_LEN (text) == 0 ) {
#if PHP_VERSION_ID < 60000
		zend_string_free (text);
#endif
		RETURN_NULL ();
	}

	if ( memcmp (ZSTR_VAL (text), SCREWIM, SCREWIM_LEN) == 0 ) {
		php_error (E_WARNING, "given data already crypted");
		RETVAL_STR (text);
#if PHP_VERSION_ID < 60000
		efree (text);
#endif
	}

	datap = zencode (ZSTR_VAL (text), ZSTR_LEN (text), (ULong *) &datalen);

#if PHP_VERSION_ID < 60000
	zend_string_free (text);
#endif

	key = mcryptkey (NULL);
	keybuf = (short *) key.buf;

	for( i=0; i<datalen; i++ ) {
		datap[i] = (char) keybuf[(datalen - i) % key.len] ^ (~(datap[i]));
	}

	efree (key.buf);

	ndata = zend_string_alloc (datalen + SCREWIM_LEN, 0);
	memcpy (ZSTR_VAL(ndata), SCREWIM, SCREWIM_LEN);
	memcpy (ZSTR_VAL(ndata) + SCREWIM_LEN, datap, datalen);
	datalen += SCREWIM_LEN;
	ZSTR_VAL(ndata)[datalen] = '\0';
	efree (datap);

	RETVAL_STR (ndata);
	zend_string_free (ndata);
}
/* end of PHP_FUNCTION (screwim_encrypt) }}} */

/* {{{ +-- PHP_FUNCTION (string) screwim_decrypt (string, (optional) key, (optional) magickey_len)
 * return strings
 */
#ifdef SCREWIM_DECRYPT
PHP_FUNCTION (screwim_decrypt) {
	zend_string * text;
	zend_string * key = NULL;
	zend_long     header_len = -1;

	SCREWData     newdata;
	int           hlen = 0;
	size_t        datalen = 0;
	char        * datap;

	// only execute on cli mode
	if ( strcmp (sapi_module.name, "cli") != 0 )
		php_error (E_ERROR, "screwim_decrypt() can only be executed in CLI mode");
	else {
		int uid = getuid ();
		if ( uid != 0 ) {
			php_error (E_ERROR, "screwim_decrypt() can only be executed by root privileges");
		}
	}

#if PHP_VERSION_ID < 60000
	text = (zend_string *) emalloc (sizeof (zend_string));
	ZSTR_VAL(text) = NULL;
	ZSTR_LEN(text) = 0;
	key  = (zend_string *) emalloc (sizeof (zend_string));
	ZSTR_VAL(key)  = NULL;
	ZSTR_LEN(key)  = 0;

	if ( zend_parse_parameters (ZEND_NUM_ARGS(), "s|sl", &ZSTR_VAL(text), &ZSTR_LEN(text), &ZSTR_VAL(key), &ZSTR_LEN(key), &header_len) == FAILURE )
#else
	if ( zend_parse_parameters (ZEND_NUM_ARGS(), "S|Sl", &text, &key, &header_len) == FAILURE )
#endif
	{
#if PHP_VERSION_ID < 60000
		zend_string_free (text);
		zend_string_free (key);
#endif
		return;
	}

#if PHP_VERSION_ID < 60000
	if ( ZEND_NUM_ARGS() == 1 ) {
		zend_string_free (key);
		key = NULL;
	}
#endif

	if ( header_len < 0 ) {
		if ( memcmp (ZSTR_VAL (text), SCREWIM, SCREWIM_LEN) != 0 ) {
			php_error (E_WARNING, "no Crypted data");
			RETVAL_STR (text);
#if PHP_VERSION_ID < 60000
			zend_string_free (text);
			zend_string_free (key);
#endif
		}
	}

	hlen = (header_len < 0) ? SCREWIM_LEN : header_len;
	datalen = ZSTR_LEN (text) - hlen;

	datap = (char *) emalloc (datalen + 1);
	memset (datap, 0, datalen + 1);
	memcpy (datap, ZSTR_VAL (text) + hlen, datalen);

	newdata = screwim_ext_buf (
		datap, datalen, (key != NULL) ? ZSTR_VAL (key) : NULL
	);

#if PHP_VERSION_ID < 60000
	zend_string_free (text);
	zend_string_free (key);
#endif
	efree (datap);

	RETVAL_SCREWDATA (newdata);
	efree (newdata.buf);
} 
#endif
/* end of PHP_FUNCTION(screwim_decrypt) }}} */

/* {{{ +-- PHP_FUNCTION (object) screwim_seed (void)
 * return strings
 */
#ifdef SCREWIM_DECRYPT
PHP_FUNCTION (screwim_seed) {
	SCREWData   key;
	int         i, j, buflen;
	short     * keybuf;
	char      * tmp;
	char      * keybyte;
	char      * keystr;

	// only execute on cli mode
	if ( strcmp (sapi_module.name, "cli") != 0 )
		php_error (E_ERROR, "screwim_seed() can only be executed in CLI mode");
	else {
		int uid = getuid ();
		if ( uid != 0 ) {
			php_error (E_ERROR, "screwim_seed() can only be executed by root privileges");
		}
	}

	if ( ZEND_NUM_ARGS () > 0 ) {
		php_error (E_WARNING, "screwim_seed() expects exactly 0 parameter, %d given", ZEND_NUM_ARGS ());
		return;
	}
	
	key = mcryptkey (NULL);
	keybuf = (short *) key.buf;
	buflen = key.len * 4;

	keybyte = emalloc (buflen + 1);
	memset (keybyte, 0, buflen + 1);

	for ( i=0; i<key.len; i++ ) {
		j = i * 4;
		sprintf (keybyte + j, "%04x", revert_endian (keybuf[i]));
	}

	buflen = key.len * 7;
	keystr = emalloc (buflen + 1);
	memset (keystr, 0, buflen + 1);

	for ( i=0; i<key.len; i++ ) {
		j = strlen (keystr);
		sprintf (keystr + j, "%d, ", keybuf[i]);
	}

	tmp = strrchr (keystr, ',');
	if ( tmp != NULL )
		*tmp = 0;

	if ( object_init (return_value) == FAILURE ) {
		php_error(E_WARNING, "screwim_seed() failure object initialize");
		efree (keybyte);
		efree (keystr);
		RETURN_NULL ();
	}

	screwim_add_property_string (return_value, "keybyte", keybyte);
	screwim_add_property_string (return_value, "keystr", keystr);
	add_property_long (return_value, "headerlen", SCREWIM_LEN);
	efree (keybyte);
	efree (keystr);
}
#endif
/* end of PHP_FUNCTION(screwim_seed) }}} */

/* {{{ +-- PHP Module resgistration
 */

zend_module_entry screwim_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"screwim",
	screwim_functions,
	PHP_MINIT(screwim),
	PHP_MSHUTDOWN(screwim),
	NULL,
	NULL,
	PHP_MINFO(screwim),
#if ZEND_MODULE_API_NO >= 20010901
	SCREWIM_VERSION, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SCREWIM
ZEND_GET_MODULE (screwim);
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("screwim.enable",  "0", PHP_INI_ALL, OnUpdateBool, enabled, zend_screwim_globals, screwim_globals)
PHP_INI_END()

PHP_MINFO_FUNCTION (screwim)
{
	php_info_print_table_start ();
	php_info_print_table_colspan_header (2, "PHP SCREW Imporved support");
	php_info_print_table_row (2, "Summary", "PHP script encryption tool");
	php_info_print_table_row (2, "URL", "http://github.com/OOPS-ORG-PHP/mod_screwim");
	php_info_print_table_row (2, "Build version", SCREWIM_VERSION);
	php_info_print_table_end ();
}

PHP_MINIT_FUNCTION (screwim)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	ZEND_INIT_MODULE_GLOBALS(screwim, php_screwim_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	org_compile_file  = zend_compile_file;
	zend_compile_file = screwim_compile_file;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(screwim)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	zend_compile_file = org_compile_file;
	return SUCCESS;
}

/* end of PHP Module registration }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
