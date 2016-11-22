# PHP Screw Improved(ScrewIm) 확장 모듈

## 개요

***PHP Screw Imporved(ScrewIm)***는 PHP 스크립트 암호화 도구 입니다. PHP 개발 산출물의 지적 재산권을 보호하기 위하여 암호화 하여 배포할 수 있으며, 시스템의 개인 정보나 DB 접근 정보와 같은 설정 파일들을 암호화 하여 보호할 수 있습니다.

이 확장 모듈은 [PM9.com, Inc.](http://www.pm9.com)의 [***Kunimasa Noda***](mailto:kuni@pm9.com)가 만든 [PHP screw](http://www.pm9.com/newpm9/itbiz/php/phpscrew/) 확장 모듈을 기반으로 개선 및 기능 추가를 하였습니다.

원 PHP-screw와의 차이점은 다음과 같습니다:
 1. 복호화시에 복호화 된 소스를 tmpfile로 생성하던 로직을 memstream을 이용하도록 변경하여 성능을 개선.
 2. 큰 파일을 암호화 또는 복호화를 할 때 memory 재할당 로직을 변경하여 성능을 개선.
 3. 복호화를 할 때, include/require 를 처리할 경우에 해당 스크립트를 2번 open하는 문제를 수정하여 성능 개선.
 4. 'screawim.enable' 옵션이 활성화 되었을 경우에만 동작하도록 개선
  * 일반적인 상황에서는 암호화된 파일 보다는 정상적인 파일을 다루는 경우가 많으므로, 이 경우를 위하여 동작 여부를 옵션으로 처리하여 성능 개선.
  * [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3) 참조
 4. 메모리 누수 수정
 5. 아마도 thread safe 할 것으로 예상 ??
 6. [php_unscrew](https://github.com/dehydr8/php_unscrew)로 decompile 가능한 문제를 좀 더 어렵게 수정
 7. 그 외 다수..

## 설명

***ScrewIm***는 암호화 도구로 PHP 스크립트를 암호화 합니다.

그리고, PHP 스크립트 실행 시, ***Zend Compiler*** API를 hooking하여 암호화된 코드를 복호화 한 후에 ***Zend Compiler***로 넘겨 주게 됩니다. 즉 암호화된 코드를 서비스 하기 위해서는 단순히 ***mod_screwim.so***를 php.ini에 등록하고, ***screwim.enable*** 옵션으로 구동여부를 결정하는 것이 전부입니다.

PHP 개발자는 코드의 암호화나 복호화에 신경을 쓸 필요가 전혀 없으며, 암호화된 스크립트 파일과 암호화 되지 않은 스크립트 파일을 혼용하는 것 또한 문제가 없습니다.

또한, 암호화 도구(screwim)의 암호화 로직과, 해독기(screwim.so)의 복호화 로직은 쉽게 원하는 대로 변경이 가능 합니다.

SEED 키를 변경 하여, 다른 사람이 빌드한 해독기(screwim.so)에서 동작을 하지 못하게 쉽게 변경을 할 수 있습니다.

## License

Copyright (c) 2016 JoungKyun.Kim

[BSD 2-clause](LICENSE)

## 요구사항

* ***PHP 5*** 이후 버전 (PHP 7 지원)
* PHP ***zlib*** 확장 모듈  
  다음 PHP 코드로 zlib가 포함되어 컴파일이 되었는지 확인 가능:
```php
    <?php gzopen(); ?>
```
* Unix 계열 OS

## 설치

### 1. 암호화 복호화 사용자 정의
  * ***my_screw.h***의 암호화 SEED키(***screwim_mcryptkey***)를 원하는 값으로 변경을 합니다.
  * 암호화 SEED 배열의 크기를 늘리면 암호화 강도를 더 높일 수 있습니다.
  * 암호화 SEED 배열의 크기는 복고화 처리 시간에 영향을 주지 않습니다.
  * (***부가적으로***) 암호화된 스크립트는 파일의 처음 부분에 Magic key를 추가 한다. 이 magic key를 변경하고 싶을 경우, ***php_screwim.h***에서 ***SCREWIM***과 ***SCREWIM_LEN***의 값을 변경하면 됩니다. ***SCREWIM_LEN*** 값은 ***SCREWIM***에 지정된 문자열의 길이와 같거나 작아야 합니다.

### 2. 빌드 및 설치
  ```bash
  [root@host mod_screwim]$ phpize
  [root@host mod_screwim]$ ./configure
  [root@host mod_screwim]$ make install
  ```

### 3. 설정
다음 라인을 ***php 설정 파일(php.ini 등)***에 추가를 합니다.

```ini
extension=screwim.so
screwim.enable = 1
```

기본적으로 ***screwim.enable*** 설정을 활성화 시키지 않으면, ***mod_screwim***은 복호화를 시도하지 않습니다. 이 의미는 ***screwim.enable***의 기본값이 0 이라는 의미 입니다. 이는 복호화를 할 필요가 없는 파일이 더 많은 환경을 위하여 선택적으로 복호화 로직을 처리할 수 있도록 설계가 되어 있습니다. 이에 대해서는 [#3 add screwim.enable ini option issue](https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3) 이슈를 참고 하십시오.

## 암호화 도구

The encription tool is located in ***mod_screwim/tools/***.
암호화 도구는 소스코드의 ***tools*** 디렉토리에 있습니다.

### 1. 빌드

```bash
[root@host mod_screwim]$ cd tools
[root@host tools]$ ./autogen.sh
[root@host tools]$ ./configure --prefix=/usr
[root@host tools]$ make
[root@host tools]$ make install # 또는 원하는 위치에 screwim 파일을 복사 하십시오.
[root@host tools]$ /usr/bin/screwim -h
screwim 1.0.0 : encode or decode php file
Usage: screwim [OPTION] PHP_FILE
   -d,     --decode   decrypt encrypted php script
   -h,     --help     this help messages
   -H VAL, --hlen=VAL length of magic key(SCREWIM_LEN or PM9SCREW_LEN). use with -d mode
   -k VAL, --key=VAL  key bytes. use with -d mode
   -v,     --view     print head length and key byte of this file
[root@host tools]$
```

### 2. 암호화

다음 명령으로 암호화된 스크립트를 생성할 수 있으며, 파일 이름은 ***파임이름.screw***로 생성이 됩니다.
```
   [root@host ~]$ /usr/bin/screwim test.php
   Success Crypting(test.php.screw)
   [root@host ~]$
```

### 3. 복호화

다음 명령으로 암호화된 스크립트를 복호화 할 수 있습니다. 파일 이름은 ***스크립트이름.discrew***로 생성이 됩니다.
```
   [root@host ~]$ /usr/bin/screwim -d test.php.screw
   Success Decrypting(test.php.screw.discrew)
   [root@host ~]$
```

### 4. 실행

PHP 설정 파일(php.ini등)에 다음 설정을 추가 하십시오. 이 설정은 PHP에 ***ScrewIm***의 복호화 기능을 사용가능 하게 합니다.

```ini
extension=screwim.so
```

위의 모듈을 등록 하였다고 하여도, 기본적으로는 복호화 로직이 동작하지는 않습니다.

복호화 루틴은 다음의 과정으로 처리를 하게 됩니다.

1. 파일을 열어서 파일의 처음이 ***Magic key***로 시작하는지 확인 (ScrewIm 으로 암호화된 파일인지 체크)
2. 암호화 되지 않은 파일일 경우(Magic key가 없을 경우) 동작 하지 않고 ***zend compiler***로 복귀
3. ***Magic key***가 확인이 되면, 복호화 로직을 구동하여 복호화 후에 ***zend compile***로 복귀

분명히 암호화/복호화를 하는데는 성능을 떨어뜨리는 요소가 존재를 합니다. 그러므로 암호화 문서를 최대한 줄이는 것이 좋습니다. 하지만 암호화를 하지 않는다고 하여도 암호화가된 스크립트인지 확인을 하기 위하여 매번 file open을 해야 하는 것 또한 성능을 떨어뜨리는 요소임에 틀림이 없으며, 접속이 많은 경우에는 심각한 성능 저햐를 유발할 수 있습니다.

***ScrewIm***은 이 문제를 해결하기 위하여, ***screwim.enable*** 옵션이 활성화가 되어 있지 않으면, 1번 사항 Magic key 체크를 하지 않고 바로 Zend compiler로 복귀를 하게 설계가 되어 있습니다.

다음은 ***screwim.enable*** 옵션을 사용하는 방법을 기술 합니다.:

#### 4.1. PHP 설정

```ini
screwim.enable = 1
```

PHP 설정 파일 (php.ini 등)에 위의 설정을 추가 합니다. 이 방법은 권장 하지 않습니다. 이 방법으로 설정을 했을 경우, 모든 PHP 스크립트에 대하여 암호화 파일 여부를 검사하게 되어 성능을 저하 시킬 수 있습니다. 접속이 그리 많지 않은 사이트이거나, 충분한 리소스가 보장이 되었다고 판단하는 경우에는 간단하게 이 방법으로 사용할 수 있습니다.

CLI 환경에서 설정 파일을 따로 사용할 수 있다면, CLI 환경에서는 이 방법을 권장 합니다.

#### 4.2. mod_php (Apache module) 환경

***Apache 모듈***로서 PHP를 구동하는 환경에서는 ***apache 설정 파일***에서 특정 경로에만 구동이 되도록 설정을 할 수 있습니다.

```apache
<Directory /path>
    php_falg screwim.enable on
</Directory>
```

#### 4.3. PHP Cli 환경

-d 명령행 옵션을 이용하여 구동할 수 있습니다.

```bash
[root@host ~]$ php -d screwim.enable=1 encrypted.php
```

CLI 환경의 경우에는 사용량 빈도가 시스템에 영향을 줄 정도가 아니기 때문에, apache 모듈 또는 FPM과 설정 파일을 따로 사용할 수 있다면 PHP 설정 파일에 이 옵션을 추가하여 사용하는 것을 권장 합니다.

#### 4.4. PHP 코드에 포함 (권장)

```php
<?php
ini_set ('screwim.enable', true);
require_once 'encrypted.php';
ini_set ('screwim.enable', false);
require_once 'normal.php';

blah_blah();
?>
```
