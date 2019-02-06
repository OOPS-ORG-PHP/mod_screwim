#!/bin/bash

source /usr/share/annyung-release/functions.d/bash/functions

errmsg () {
	echo "$*" > /dev/stderr
}

usage () {
	echo "Usage: $0 [clean|pack]"
	exit 1
}

optvalue () {
	local vn="$1"
	local vv
	shift
	vv=$(echo "$*" | sed -r "s/^'|'$//g")
	eval "$vn=\"\${vv}\""
}

opts=$(getopt -o h -l help -- "$@")
[ $? != 0 ] && usage

set -- ${opts}
for i
do
	case "$i" in
		-h|--help)
			usage
			shift
			;;
		--)
			shift
			break
			;;
	esac
done

optvalue mode $1

case "${mode}" in
	clean)
		[ -f Makefile ] && make distclean
		rm -rf include autom4te* build modules
		rm -f Makefile.g* Makefile.f* Makefile.o* Makefile
		rm -f config.log config.nice config.g* config.s* config.h*
		rm -f ac* configure* mkinstalldirs .deps missing ltmain.sh
		rm -f run-tests.php install* libtool tools/screw

		[ -f tools/Makefile ] && make -C tools distclean
		rm -rf tools/build
		rm -f tools/ac* tools/config.h* tools/{configure,install-sh,/missing}

		rm -f package.xml
		;;
	pack)
		cp -af package.xml.tmpl package.xml
		list=$(grep "md5sum" ./package.xml | sed -r 's/.*"@|@".*//g')

		for i in ${list}
		do
			md5s=$(md5sum "$i" | awk '{print $1}')
			perl -pi -e "s!\@${i}\@!${md5s}!g" ./package.xml
		done

		curdate=$(date +'%Y-%m-%d')
		curtime=$(date +'%H:%M:%S')

		perl -pi -e "s!\@curdate\@!${curdate}!g" ./package.xml
		perl -pi -e "s!\@curtime\@!${curtime}!g" ./package.xml

		pecl package
		rm -f package.xml
		;;
	*)
		errmsg "Unsupport mode '${1}'"
		exit 1
esac

exit 0
