                    [ PHP Screw Improved - User's Manual ]

1. What's PHP SCREW Imporved (ScrewIm)?

 PHP Screw Imporved is a PHP script encryption tool. When you are developing a
 commercial package using PHP, the script can be distributed as encrypted up
 until just before execution. This preserves your intellectual property.

 This extension is forked from PHP screw who made by Kunimasa Noda in
 PM9.com, Inc. and, his information is follows:
   http://www.pm9.com
   kuni@pm9.com
   https://sourceforge.net/projects/php-screw/

2. Different original PHP SCREW

 * change the method of decryption processing from creating temp file to
   memstream api.

 * improved memory reallocation logic.
 
 * Only works if 'screwim.enable' option is on.
   - Improved performance by don't check magic key under normal environment.
   - see also https://github.com/OOPS-ORG-PHP/mod_screwim/issues/3

 * maybe thread safe


2. Outline.

 The outline of ScrewIm is as follows.

 a. Encipher a PHP script with the encryption tool (screwim).

 b. At the time of execution of a PHP script, the decryptor screwim.so is
    executed as PHP-Extension, just before the PHP script is handed to the
    Zend-Compiler.

   * In fact what is necessary is just to add the description about php.screw
     to php.ini. A PHP script programmer does not need to be conscious of
     decrypting process.
     Moreover it is possible for you to intermingle an enciphered script file
     and an unenciphered one.

   * The encryption logic in the encryption tool (screwim) and the decryption
     logic in the decryptor (ScrewIm), can be cutomized easily.

   * The normal purpose code and decryption logic included, can be customized
     by only changing the encryption SEED key.
     Although it is easy to cusomize the encryption, by the encryption SEED, it
     does NOT mean, that the PHP scripts can be decrypted by others easily.


3. Requirement

 (1) PHP 5.x and after (Also support PHP 7)
     NOTICE: PHP must be compiled with the "--with-zlib" option.
     Check that PHP has zlib compiled in it with the PHP script:
     "<?php gzopen(); ?>"
     If PHP knows about the function you can happily proceed.

 (2) UNIX like OS (LINUX, FreeBSD, etc. are included) 


4. Installation.

 (1) Unpack mod_screwim-{VERSION}.tar.gz.

 (2) Execute the following command with directory mod_screwim-{VERSION}

       phpize
       ./configure

 (3) Customize encryption / decryption

       vi my_screw.h

       -- Please change the encryption SEED key (screwim_mycryptkey) into the
          values according to what you like.
          The encryption will be harder to break, if you add more values to the
	  encryption SEED array. However, the size of the SEED is unrelated to
	  the time of the decrypt processing.

       *  If you can read and understand the source code, to modify an original
          encryption logic will be possible. But in general, this should not
	  be necessary.

       OPTIONAL: Encrypted scripts get a stamp added to the beginning of the
		 file. If you like, you may change this stamp defined by
		 SCREWIM and SCREWIM_LEN in php_screwim.h. SCREWIM_LEN must
		 be less than or equal to the size of SCREWIM.

 (4) Compile the decryptor

       make

 (5) Copy modules/screwim.so into an extension directory.

       ex)
           make install

           or

           cp modules/screwim.so $(php-config --extension-dir)/

           or

           cp modules/screwim.so $(php -i | grep extension_dir | awk '{print $5}')/


 (6) Add the next line to php.ini.

       extension=screwim.so

 (7) Restart the httpd daemon or FPM daemon.

 (8) Compile the encryption tool

       cd tools
       make

 (9) Copy the encryptor ("screwim") into an appropriate directory.


5. User's manual.

 (1) Encrypting a PHP script:
     Please execute the following command.

        screwim "Path to the PHP script to be encrypted"

     The above line creates the script file enciphered by the name of script
     file name .screw.

 (2) Executing a PHP script.
     If the installation of screwim.so went OK, encrypted PHP scripts can now
     be copied to an appropriate directory and executed as if it was
     unencrypted. That is: The encryption is transparent for the
     user.


7. Copyright
 (C) 2016 JoungKyun.Kim, http://oops.org/
 contact:  https://joungkyun.github.io
