PHP Screw Improved(ScrewIm) extension
==
[![GitHub license](https://img.shields.io/badge/License-BSD%202--Clause-blue.svg)](https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_screwim/master/LICENSE)
![GitHub download](https://img.shields.io/github/downloads/OOPS-ORG-PHP/mod_screwim/total.svg)
[![GitHub last release](https://img.shields.io/github/release/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/releases)
[![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/issues?q=is%3Aissue+is%3Aclosed)
[![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/pulls?q=is%3Apr+is%3Aclosed)

## Information

If you can read korean, see also [README.ko.md](README.ko.md)

## Abstract

___PHP Screw Imporved(ScrewIm)___ is a PHP script encryption tool. When you are developing a commercial package using PHP, the script can be distributed as encrypted up until just before execution. This preserves your intellectual property.

This extension is based from [PHP screw](http://www.pm9.com/newpm9/itbiz/php/phpscrew/) who made by [___Kunimasa Noda___](mailto:kuni@pm9.com) in [PM9.com, Inc.](http://www.pm9.com)

The differences from the original PHP-screw are as follows:
 1. Improved performance by processing in memory rather than creating temporary files during decoding.
    1. Applied [Sungwook-Shin's improved patch](https://github.com/edp1096)
    2. Improved performance by fixing duplicated file open (issue #4)
    3. Fixed memory leaks.
 2. Improved performance by changing memory reallocation logic when encoding or decoding large files.
 3. Only works if 'screwim.enable' option is on.
    * Improved performance by don't check magic key under normal environment(regular php script).
    * See also [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3)
 4. Fixed memory leaks.
 5. Remove global variable. Maybe thread safe.
 6. Preventing problems that can be decompiled with [php_unscrew](https://github.com/dehydr8/php_unscrew)
 7. support runtime encrypt function ```screwim_encrypt()```
 8. support runtime decrypt function ```screwim_decrypt(), screwim_seed()```
 9. And so on..

## Description

___ScrewIm___ is encipher a PHP script with the encryption tool.

And, at the time of execution of a PHP script, the decryptor screwim.so is executed as PHP-Extension, just before the PHP script is handed to the Zend-Compiler.

In fact what is necessary is just to add the description about php.screw to php.ini. A PHP script programmer does not need to be conscious of decrypting process. Moreover it is possible for you to intermingle an enciphered script file and an unenciphered one.

The encryption logic in the encryption tool(screwim) and the decryption logic in the decryptor(screwim.so), can be customized easily.

The normal purpose code and decryption logic included, can be customized by only changing the encryption SEED key. Although it is easy to cusomize the encryption, by the encryption SEED, it does NOT mean, that the PHP scripts can be decrypted by others easily.

## License

Copyright (c) 2021 JoungKyun.Kim

[BSD 2-clause](LICENSE)

## Requirement

* ___PHP 5___ and after (Also support ___PHP 7___)
* PHP ___zlib___ extension.  
  Check that PHP has zlib compiled in it with the PHP script:
```php
    <?php gzopen(); ?>
```
* Unix like OS.

## Installation

### 1. Customize encrytion / decryption  
  * ~~change the encryption SEED key (___screwim_mcryptkey___) in ___my_screw.h___ into the values according to what you like.~~
  * ~~The encryption will be harder to break, if you add more values to the encryption SEED array.~~
  * ~~However, the size of the SEED is unrelated to the time of the decrypt processing.~~
  * The encryption SEED key is now automatically generated from 5 to 8 arrays at configure time. Don't use ___my_screw.h___ any more. (craeted with the ___SCREWIM_ENC_DATA___ constant in config.h)
  * Encrypted scripts get a stamp added to the beginning of the file. Before building, you must change this stamp defined by ___SCREWIM___ and ___SCREWIM_LEN___ in ___php_screwim.h___. ___SCREWIM_LEN___ must be less than or equal to the size of ___SCREWIM___.
    * In November 2017, [a php extension](https://github.com/jc-lab/php_screwhook) has been introduced that hooks up the ___zend_compile_file___ API before ___mod_screwim___ to take an encrypted file. To prevent this module, the values of ___SCREWIM___ and ___SCREWIM_LEN___ must be changed, and longer is better.
    * No matter how long you change to a string, you can not defend 100% in mod_screwim logic. It is not suitable for encrypted code distribution.
    * If you change the Magic key, it is not compatible with existing encrypted files. New encryption is required.


### 2. Build and install  
  ```bash
  [root@host mod_screwim]$ phpize
  [root@host mod_screwim]$ ./configure
  [root@host mod_screwim]$ make install
  ```
On configure, the ___--enable-screwim-decrypt___ option adds decrypt functions ( ___screwim_decrypt(), screwim_seed()___ ). This means that ___you can decrypt an encrypted PHP file___.

If you are building ___for distribution___, never add the --enable-screwim-decrypt option!


### 3. Test

PHP can test to see if the built module works.

```bash
[root@host mod_screwim]$ make test PHP_EXECUTABLE=/usr/bin/php
```

Or, if you want to test the extension you built before installing, you can do the following:

```bash
[root@host mod_screwim]$ php -d "extension_dir=./modules/" -d "extension=screwim.so" some.php
```

### 4. Configuration
Add next line to php configuration file (php.ini and so on)

```ini
extension=screwim.so
screwim.enable = 1
```

By default, decryption does not work, so the performance of regular PHP files is better than the original PHP Screw. The screwim.enable option must be turned on for decryption to work. See also https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3

For detail on the settings, refer to the ___Execution___ item below.


### 5. APIs

* ___(string) screwim_encrypt (string)___
  * Support runtime encryption.
  * Can be used instead of ___tools/screwim___ command
  * This API is not affected by the ___screwim.enable___ option.

```php
  <?php
  $code = <<<EOF
  <?php
  $conf['url'] = 'http://domain.com/register';
  $conf['pass'] = 'blah blah';
  ?>
  EOF;

  $data = screwim_encrypt ($code);
  file_put_contents ('./config/config.php', $data);
  ?>
```

* ___(string) screwim_decrypt (string, (optional) key, (optional) magickey_len)___  
  * The ___--enable-screwim-decrypt___ option must be given at build time.
  * Support runtime decryption.
  * Can be used instead of ___tools/screwim___ command
  * When call in an environment other than ___CLI mode___, ___E_ERROR___ occurs.
  * When not running as ___root privileges___, ___E_ERROR___ occurs.
  * This API is not affected by the ___screwim.enable___ option.

```php
  <?php
  $config = file_get_contents ('./config/config.php');
  echo screwim_decrypt ($config);
  ?>
```

* ___(object) screwim_seed (void)___
  * The ___--enable-screwim-decrypt___ option must be given at build time.
  * Returns ___encrypt seed key___ of current ___mod_screwim.so___
  * Can be used instead of ___tools/screwim___ command
  * When call in an environment other than ___CLI mode___, ___E_ERROR___ occurs.
  * When not running as ___root privileges___, ___E_ERROR___ occurs.
  * This API is not affected by the ___screwim.enable___ option.

```php
  <?php
  // returns follow
  // stdClass Object
  // (
  //     [keybyte] => 6b22886a0f4faa5f37783d36944d7823e707
  //     [keystr] => 8811, 27272, 20239, 24490, 30775, 13885, 19860, 9080, 2023
  //     [headerlen] => 14
  // )
  print_r (screwim_seed ());
  ?>
```


## Command line Encryption Tool

The encription tool is located in ___mod_screwim/tools/___ .

### 1. Build

```bash
[root@host mod_screwim]$ cd tools
[root@host tools]$ ./autogen.sh
[root@host tools]$ ./configure --prefix=/usr
[root@host tools]$ make
[root@host tools]$ make install # Or copy the screwim file into an appropriate directory.
[root@host tools]$ /usr/bin/screwim -h
screwim 1.0.3 : encode or decode php file
Usage: screwim [OPTION] PHP_FILE
   -c VAL, --convert=VAL convert key byte to digits
   -d,     --decode   decrypt encrypted php script
   -h,     --help     this help messages
   -H VAL, --hlen=VAL length of magic key(SCREWIM_LEN or PM9SCREW_LEN). use with -d mode
   -k VAL, --key=VAL  key bytes. use with -d mode
   -v,     --view     print head length and key byte of this file
[root@host tools]$
```

### 2. Encryptioin

The follow command creates the script file enciphered by the name of ___script file name .screw___.
```
   [root@host ~]$ /usr/bin/screwim test.php
   Success Crypting(test.php.screw)
   [root@host ~]$
```

### 3. Decryption

The follow command creates the script file enciphered by the name of ___script file name .discrew___.
```
   [root@host ~]$ /usr/bin/screwim -d test.php.screw
   Success Decrypting(test.php.screw.discrew)
   [root@host ~]$
```

## Execution

Add next line to php configuration file (php.ini and so on)

```ini
extension=screwim.so
```

The decryption does not work by default, when loading module.

The decryption process is as follows:
  1. Open the file and verify that the beginning of the file starts with the Magic key. (Check if the file is encrypted with ScrewIm)
  2. If the file is unencrypted(If the Magic key is missing or different), return to the zend compiler without working.
  3. Once the Magic key is confirmed, return to zend compile after decoding.

Obviously encryption and decryption will degrade performance. Therefore, it is recommended to reduce the number of encrypted documents as much as possible. However, even if you do not encrypt it, opening all files to check for encryption also causes performance degradation. In particular, a large number of connections can cause serious performance degradation.

To solve this problem, ___mod_screwim___ will return to the Zend compiler without checking the Magic key if the screwim.enable option is not enabled.

___Therefore, it is best to minimize the encryption as much as possible, and it is recommended to include it in php after encrypting only certain functions___.

Here is how to use the screwim.enable option:

### 1. PHP configuration file

```ini
screwim.enable = 1
```

Add the above settings to the PHP configuration file(___php.ini___ and so on). It is not recommended because it causes ___performance degradation___ when processing unencrypted php scripts.

Use this setting if there are not many connections or if there is sufficient resources.

In the CLI environment, if you can use the CLI configuration file separately from the apache module or FPM, it is recommended to add the above settings to php.ini.

### 2. mod_php (Apache module) envionment

Use the <directory> block to make the decryptor work on the desired path.

```apache
<Directory /path>
    php_falg screwim.enable on
</Directory>
```

### 3. PHP Cli environments

use -d option.

```bash
[root@host ~]$ php -d screwim.enable=1 encrypted.php
```

In the CLI environment, resource utilization is not high. If the configuration file is separate from the apache module or FPM, it is recommended that you add this option to your PHP configuration file.

### 4. embeded php code (Recommand)

```php
<?php
ini_set ('screwim.enable', true);
require_once 'encrypted.php';
ini_set ('screwim.enable', false);
require_once 'normal.php';

blah_blah();
?>
```
