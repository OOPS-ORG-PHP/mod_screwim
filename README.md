# PHP Screw Improved(ScrewIm) extension

## Abstract

***PHP Screw Imporved(ScrewIm)*** is a PHP script encryption tool. When you are developing a commercial package using PHP, the script can be distributed as encrypted up until just before execution. This preserves your intellectual property.

This extension is based from [PHP screw](http://www.pm9.com/newpm9/itbiz/php/phpscrew/) who made by [***Kunimasa Noda***](mailto:kuni@pm9.com) in [PM9.com, Inc.](http://www.pm9.com)

The differences from the original PHP-screw are as follows:
 1. Improved performance by processing in memory rather than creating temporary files during decoding.
 2. Improved performance by changing memory reallocation logic when encoding or decoding large files.
 3. Improved performance by fixing duplicated file open (issue #4)
 4. Only works if 'screwim.enable' option is on.
  * Improved performance by don't check magic key under normal environment(regular php script).
  * See also [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3)
 5. Fixed memory leaks.
 6. Maybe thread safe.
 7. Preventing problems that can be decompiled with [php_unscrew](https://github.com/dehydr8/php_unscrew)
 8. support runtime encrypt function (***screwim_encrypt()***)
 9. support runtime decrypt function (***screwim_decrypt(), screwim_seed()***)
 10. And so on..

## Description

***ScrewIm*** is encipher a PHP script with the encryption tool.

And, at the time of execution of a PHP script, the decryptor screwim.so is executed as PHP-Extension, just before the PHP script is handed to the Zend-Compiler.

In fact what is necessary is just to add the description about php.screw to php.ini. A PHP script programmer does not need to be conscious of decrypting process. Moreover it is possible for you to intermingle an enciphered script file and an unenciphered one.

The encryption logic in the encryption tool(screwim) and the decryption logic in the decryptor(screwim.so), can be customized easily.

The normal purpose code and decryption logic included, can be customized by only changing the encryption SEED key. Although it is easy to cusomize the encryption, by the encryption SEED, it does NOT mean, that the PHP scripts can be decrypted by others easily.

## License

Copyright (c) 2016 JoungKyun.Kim

[BSD 2-clause](LICENSE)

## Requirement

* ***PHP 5*** and after (Also support ***PHP 7***)
* PHP ***zlib*** extension.  
  Check that PHP has zlib compiled in it with the PHP script:
```php
    <?php gzopen(); ?>
```
* Unix like OS.

## Installation

### 1. Customize encrytion / decryption  
  * ~~change the encryption SEED key (***screwim_mcryptkey***) in ***my_screw.h*** into the values according to what you like.~~
  * ~~The encryption will be harder to break, if you add more values to the encryption SEED array.~~
  * ~~However, the size of the SEED is unrelated to the time of the decrypt processing.~~
  * The encryption SEED key is now automatically generated from 5 to 8 arrays at configure time. Don't use ***my_screw.h*** any more. (craeted with the ***SCREWIM_ENC_DATA*** constant in config.h)
  * (***Optional***) Encrypted scripts get a stamp added to the beginning of the file. If you like, you may change this stamp defined by ***SCREWIM*** and ***SCREWIM_LEN*** in ***php_screwim.h***. ***SCREWIM_LEN*** must be less than or equal to the size of ***SCREWIM***.

### 2. Build and install  
  ```bash
  [root@host mod_screwim]$ phpize
  [root@host mod_screwim]$ ./configure
  [root@host mod_screwim]$ make install
  ```
On configure, the ***--enable-screwim-decrypt*** option adds decrypt functions(***screwim_decrypt(), screwim_seed()***). This means that ***you can decrypt an encrypted PHP file***.

If you are building ***for distribution***, never add the --enable-screwim-decrypt option!


### 3. Configuration
Add next line to php configuration file (php.ini and so on)

```ini
extension=screwim.so
screwim.enable = 1
```

By default, decryption does not work, so the performance of regular PHP files is better than the original PHP Screw. The screwim.enable option must be turned on for decryption to work. See also https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3

For detail on the settings, refer to the ***Execution*** item below.

### 4. APIs

* ***(string) screwim_encrypt (string)***
 * Support runtime encryption.
 * Can be used instead of ***tools/screwim*** command
 * This API is not affected by the ***screwim.enable*** option.

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

* ***(string) screwim_decrypt (string, (optional) key, (optional) magickey_len)***  
 * The ***--enable-screwim-decrypt*** option must be given at build time.
 * Support runtime decryption.
 * Can be used instead of ***tools/screwim*** command
 * When call in an environment other than ***CLI mode***, ***E_ERROR*** occurs.
 * When not running as ***root privileges***, ***E_ERROR*** occurs.
 * This API is not affected by the ***screwim.enable*** option.

```php
  <?php
  $config = file_get_contents ('./config/config.php');
  echo screwim_decrypt ($config);
  ?>
```

* ***(object) screwim_seed (void)***
 * The ***--enable-screwim-decrypt*** option must be given at build time.
 * Returns ***encrypt seed key*** of current ***mod_screwim.so***
 * Can be used instead of ***tools/screwim*** command
 * When call in an environment other than ***CLI mode***, ***E_ERROR*** occurs.
 * When not running as ***root privileges***, ***E_ERROR*** occurs.
 * This API is not affected by the ***screwim.enable*** option.

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

The encription tool is located in ***mod_screwim/tools/***.

### 1. Build

```bash
[root@host mod_screwim]$ cd tools
[root@host tools]$ ./autogen.sh
[root@host tools]$ ./configure --prefix=/usr
[root@host tools]$ nmake
[root@host tools]$ make install # Or copy the screwim file into an appropriate directory.
[root@host tools]$ /usr/bin/screwim -h
screwim 1.0.0 : encode or decode php file
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

The follow command creates the script file enciphered by the name of ***script file name .screw***.
```
   [root@host ~]$ /usr/bin/screwim test.php
   Success Crypting(test.php.screw)
   [root@host ~]$
```

### 3. Decryption

The follow command creates the script file enciphered by the name of ***script file name .discrew***.
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

To solve this problem, ***mod_screwim*** will return to the Zend compiler without checking the Magic key if the screwim.enable option is not enabled.

***Therefore, it is best to minimize the encryption as much as possible, and it is recommended to include it in php after encrypting only certain functions***.

Here is how to use the screwim.enable option:

### 1. PHP configuration file

```ini
screwim.enable = 1
```

Add the above settings to the PHP configuration file(***php.ini*** and so on). It is not recommended because it causes ***performance degradation*** when processing unencrypted php scripts.

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
