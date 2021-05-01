PHP Screw Improved(ScrewIm) 확장 모듈
==
[![GitHub license](https://img.shields.io/badge/License-BSD%202--Clause-blue.svg)](https://raw.githubusercontent.com/OOPS-ORG-PHP/mod_screwim/master/LICENSE)
[![GitHub last release](https://img.shields.io/github/release/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/releases)
[![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/issues?q=is%3Aissue+is%3Aclosed)
[![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/OOPS-ORG-PHP/mod_screwim.svg)](https://github.com/OOPS-ORG-PHP/mod_screwim/pulls?q=is%3Apr+is%3Aclosed)

## 개요

___PHP Screw Imporved(ScrewIm)___ 는 PHP 스크립트 암호화 도구 입니다. PHP 개발 산출물의 지적 재산권을 보호하기 위하여 암호화 하여 배포할 수 있으며, 시스템의 개인 정보나 DB 접근 정보와 같은 설정 파일들을 암호화 하여 보호할 수 있습니다.

이 확장 모듈은 [PM9.com, Inc.](http://www.pm9.com)의 [___Kunimasa Noda___](mailto:kuni@pm9.com)가 만든 [PHP screw](http://www.pm9.com/newpm9/itbiz/php/phpscrew/) 확장 모듈을 기반으로 개선 및 기능 추가를 하였습니다.

원 PHP-screw와의 차이점은 다음과 같습니다:
 1. 복호화시에 복호화 된 소스를 tmpfile로 생성하던 로직을 memstream을 이용하도록 변경하여 성능을 개선.
    1. [신성욱님 개선 사항](http://enjoytools.net/xe/board_nfRq49/2018) 반영
    2. 원 코드에서 복호화를 할 때, include/require 를 처리할 경우에 해당 스크립트를 2번 open하는 문제를 수정하여 성능 개선.
    3. 메모리 누수 수정
 2. 큰 파일을 암호화 또는 복호화를 할 때 memory 재할당 로직을 변경하여 성능을 개선.
 3. 'screawim.enable' 옵션이 활성화 되었을 경우에만 동작하도록 개선
    * 일반적인 상황에서는 암호화된 파일 보다는 정상적인 파일을 다루는 경우가 많으므로, 이 경우를 위하여 동작 여부를 옵션으로 처리하여 성능 개선.
    * [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3) 참조
 4. 전역 변수 제거. 아마도 thread safe 할 것으로 예상 ??
 5. [php_unscrew](https://github.com/dehydr8/php_unscrew)로 decompile 가능한 문제를 좀 더 어렵게 수정
 6. runtime encrypt 함수 지원 ```screwim_encrypt()```
 7. runtime decrypt 함수 지원 ```screwim_decrypt(), screwim_seed()```
 8. 그 외 다수..

## 설명

___ScrewIm___ 는 암호화 도구로 PHP 스크립트를 암호화 합니다.

그리고, PHP 스크립트 실행 시, ___Zend Compiler___ API를 hooking하여 암호화된 코드를 복호화 한 후에 ___Zend Compiler___ 로 넘겨 주게 됩니다. 즉 암호화된 코드를 서비스 하기 위해서는 단순히 ___mod_screwim.so___ 를 php.ini에 등록하고, ___screwim.enable___ 옵션으로 구동여부를 결정하는 것이 전부입니다.

PHP 개발자는 코드의 암호화나 복호화에 신경을 쓸 필요가 전혀 없으며, 암호화된 스크립트 파일과 암호화 되지 않은 스크립트 파일을 혼용하는 것 또한 문제가 없습니다.

또한, 암호화 도구(screwim)의 암호화 로직과, 해독기(screwim.so)의 복호화 로직은 쉽게 원하는 대로 변경이 가능 합니다.

SEED 키를 변경 하여, 다른 사람이 빌드한 해독기(screwim.so)에서 동작을 하지 못하게 쉽게 변경을 할 수 있습니다.

## License

Copyright (c) 2021 JoungKyun.Kim

[BSD 2-clause](LICENSE)

## 요구사항

* ___PHP 5___ 이후 버전 (PHP 7 지원)
* PHP ___zlib___ 확장 모듈  
  다음 PHP 코드로 zlib가 포함되어 컴파일이 되었는지 확인 가능:
```php
    <?php gzopen(); ?>
```
* Unix 계열 OS

## 설치

### 1. 암호화 복호화 사용자 정의
  * ~~___my_screw.h___ 의 암호화 SEED키(```screwim_mcryptkey```)를 원하는 값으로 변경을 합니다.~~
  * ~~암호화 SEED 배열의 크기를 늘리면 암호화 강도를 더 높일 수 있습니다.~~
  * ~~암호화 SEED 배열의 크기는 복고화 처리 시간에 영향을 주지 않습니다.~~
  * 이제 ___configure___ 시에 암호화 SEED키는 5~8개의 배열로 자동 생성이 됩니다. 더이상 ___my_screw.h___ 를 사용하지 않습니다. (config.h의 ___SCREWIM_ENC_DATA___ 상수로 생성이 됩니다.)
  * 암호화된 스크립트는 파일의 처음 부분에 Magic key를 추가 합니다. 빌드를 하기 전, 이 magic key를 꼭 변경 하여야 합니다. ___php_screwim.h___ 에서 ___SCREWIM___ 과 ___SCREWIM_LEN___ 의 값을 변경하면 됩니다. ___SCREWIM_LEN___ 값은 ___SCREWIM___ 에 지정된 문자열의 길이와 같거나 작아야 합니다.
    * 2017.11 zend_compile_file API를 mod_screwim 보다 먼저 hooking 하여 암호화된 파일을 탈취하는 [php extension](https://github.com/jc-lab/php_screwhook) 이 나왔습니다. 이 모듈을 방지 하기 위하여, ___SCREWIM___ 과 ___SCREWIM_LEN___ 의 값은 반드시 변경이 되어야 하며, 길면 길수록 좋습니다.
    * 아무리 길게 변경을 하더라도 mod_screwim 이 사용하는 logic 상 100% 탈취를 막을 수는 없습니다. 암호화된 코드 배포용으로는 적합하지 않습니다.
    * Magic key를 변경할 경우, 기존의 암호화 되어진 파일과는 호환이 되지 않습니다. 새로 암호화를 하여야 합니다.


### 2. 빌드 및 설치
  ```bash
  [root@host mod_screwim]$ phpize
  [root@host mod_screwim]$ ./configure
  [root@host mod_screwim]$ make install
  ```

configure 시에, ___--enable-screwim-decrypt___ 옵션을 주면, 복호화 기능(```screwim_decrypt(), screwim_seed()```)이 추가 됩니다. 즉, ___암호화된 PHP 파일을 복호화 할 수 있다___ 는 의미입니다.

___--enable-screwim-decrypt___ 옵션은 배포용으로 빌드를 할 경우에는 절대 추가하면 안됩니다!


### 3. 테스트

PHP 는 빌드된 모듈이 잘 작동하는지 test 를 할 수 있습니다.

```bash
[root@host mod_screwim]$ make test PHP_EXECUTABLE=/usr/bin/php
```

또는, install 전에 빌드한 extension을 직접 테스트 하려면 다음과 같이 가능 합니다.

```bash
[root@host mod_screwim]$ php -d "extension_dir=./modules/" -d "extension=screwim.so" some.php
```

### 4. 설정
다음 라인을 ___php 설정 파일(php.ini 등)___ 에 추가를 합니다.

```ini
extension=screwim.so
screwim.enable = 1
```

기본적으로 ___screwim.enable___ 설정을 활성화 시키지 않으면, ___mod_screwim___ 은 복호화를 시도하지 않습니다. 이 의미는 ___screwim.enable___ 의 기본값이 0 이라는 의미 입니다. 이는 복호화를 할 필요가 없는 파일이 더 많은 환경을 위하여 선택적으로 복호화 로직을 처리할 수 있도록 설계가 되어 있습니다. 이에 대해서는 [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3) 이슈를 참고 하십시오.

설정에 대한 자세한 사항은 아래의 ___실행___ 항목을 참고 하십시오.


### 5. APIs

* ___(string) screwim_encrypt (string)___  
  * PHP 실행 중에, data를 암호화 합니다.
  * ___tools/screwim___ 명령어 대신 사용할 수 있습니다.
  * 암호화된 설정 파일을 생성하는 데 이용할 수 있습니다.
  * ___screwim.enable___ 옵션 여부의 영향을 받지 않습니다.

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
  * ___configure___ 시에 ___--enable-screwim-decrypt___ 옵션이 필요 합니다.
  * PHP 실행 중에, 암호화된 data를 복호화 합니다.
  * ___tools/screwim___ 명령어 대신 사용할 수 있습니다.
  * CLI 모드가 아니거나 root 권한이 아닐 경우, ___E_ERROR___ 처리 됩니다.
  * ___screwim.enable___ 옵션 여부의 영향을 받지 않습니다.

```php
  <?php
  $config = file_get_contents ('./config/config.php');
  echo screwim_decrypt ($config);
  ?>
```

* ___(object) screwim_seed (void)___
  * ___configure___ 시에 ___--enable-screwim-decrypt___ 옵션이 필요 합니다.
  * 현재 모듈의 encrypt seed key를 반환 합니다.
  * ___tools/screwim___ 명령어 대신 사용할 수 있습니다.
  * CLI 모드가 아니거나 root 권한이 아닐 경우, ___E_ERROR___ 처리 됩니다.
  * ___screwim.enable___ 옵션 여부의 영향을 받지 않습니다.

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


## 명령행 암호화 도구

암호화 도구는 소스코드의 ___tools___ 디렉토리에 있습니다.

### 1. 빌드

```bash
[root@host mod_screwim]$ cd tools
[root@host tools]$ ./autogen.sh
[root@host tools]$ ./configure --prefix=/usr
[root@host tools]$ make
[root@host tools]$ make install # 또는 원하는 위치에 screwim 파일을 복사 하십시오.
[root@host tools]$ /usr/bin/screwim -h
screwim 1.0.5 : encode or decode php file
Usage: screwim [OPTION] PHP_FILE
   -c VAL, --convert=VAL convert key byte to digits
   -d,     --decode   decrypt encrypted php script
   -h,     --help     this help messages
   -H VAL, --hlen=VAL length of magic key(SCREWIM_LEN or PM9SCREW_LEN). use with -d mode
   -k VAL, --key=VAL  key bytes. use with -d mode
   -v,     --view     print head length and key byte of this file
[root@host tools]$
```

### 2. 암호화

다음 명령으로 암호화된 스크립트를 생성할 수 있으며, 파일 이름은 ___파임이름.screw___ 로 생성이 됩니다.
```
   [root@host ~]$ /usr/bin/screwim test.php
   Success Crypting(test.php.screw)
   [root@host ~]$
```

### 3. 복호화

다음 명령으로 암호화된 스크립트를 복호화 할 수 있습니다. 파일 이름은 ___스크립트이름.discrew___ 로 생성이 됩니다.
```
   [root@host ~]$ /usr/bin/screwim -d test.php.screw
   Success Decrypting(test.php.screw.discrew)
   [root@host ~]$
```

## 실행

PHP 설정 파일(php.ini등)에 다음 설정을 추가 하십시오. 이 설정은 PHP에 ___ScrewIm___ 의 복호화 기능을 사용가능 하게 합니다.

```ini
extension=screwim.so
```

위의 모듈을 등록 하였다고 하여도, 기본적으로는 복호화 로직이 동작하지는 않습니다.

복호화 루틴은 다음의 과정으로 처리를 하게 됩니다.

1. 파일을 열어서 파일의 처음이 ___Magic key___ 로 시작하는지 확인 (ScrewIm 으로 암호화된 파일인지 체크)
2. 암호화 되지 않은 파일일 경우(Magic key가 없을 경우) 동작 하지 않고 ___zend compiler___ 로 복귀
3. ___Magic key___ 가 확인이 되면, 복호화 로직을 구동하여 복호화 후에 ___zend compile___ 로 복귀

분명히 암호화/복호화를 하는데는 성능을 떨어뜨리는 요소가 존재를 합니다. 그러므로 암호화 문서를 최대한 줄이는 것이 좋습니다. 하지만 암호화를 하지 않는다고 하여도 암호화가된 스크립트인지 확인을 하기 위하여 매번 file open을 해야 하는 것 또한 성능을 떨어뜨리는 요소임에 틀림이 없으며, 접속이 많은 경우에는 심각한 성능 저하를 유발할 수 있습니다.

___mod_screwim___ 은 이 문제를 해결하기 위하여, ___screwim.enable___ 옵션이 활성화가 되어 있지 않으면, 1번 사항 Magic key 체크를 하지 않고 바로 Zend compiler로 복귀를 하게 설계가 되어 있습니다.

___그러므로, 될 수 있는한 암호화는 최소화 하는 것이 좋으며, 특정 기능만 암호화 하여, php에서 include 하여 사용하는 것을 권장 합니다.___

다음은 ___screwim.enable___ 옵션을 사용하는 방법을 기술 합니다.:

### 1. PHP 설정 파일

```ini
screwim.enable = 1
```

PHP 설정 파일 (php.ini 등)에 위의 설정을 추가 합니다. 이 방법은 권장 하지 않습니다. 이 방법으로 설정을 했을 경우, 모든 PHP 스크립트에 대하여 암호화 파일 여부를 검사하게 되어 성능을 저하 시킬 수 있습니다. 접속이 그리 많지 않은 사이트이거나, 충분한 리소스가 보장이 되었다고 판단하는 경우에는 간단하게 이 방법으로 사용할 수 있습니다.

CLI 환경에서 설정 파일을 따로 사용할 수 있다면, CLI 환경에서는 이 방법을 권장 합니다.

### 2. mod_php (Apache module) 환경

___Apache 모듈___ 로서 PHP를 구동하는 환경에서는 ___apache 설정 파일___ 에서 특정 경로에만 구동이 되도록 설정을 할 수 있습니다.

```apache
<Directory /path>
    php_falg screwim.enable on
</Directory>
```

### 3. PHP Cli 환경

-d 명령행 옵션을 이용하여 구동할 수 있습니다.

```bash
[root@host ~]$ php -d screwim.enable=1 encrypted.php
```

CLI 환경의 경우에는 사용량 빈도가 시스템에 영향을 줄 정도가 아니기 때문에, apache 모듈 또는 FPM과 설정 파일을 따로 사용할 수 있다면 PHP 설정 파일에 이 옵션을 추가하여 사용하는 것을 권장 합니다.

### 4. PHP 코드에 포함 (권장)

```php
<?php
ini_set ('screwim.enable', true);
require_once 'encrypted.php';
ini_set ('screwim.enable', false);
require_once 'normal.php';

blah_blah();
?>
```
