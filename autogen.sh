#!/bin/bash

if [ -e ./remakesourcelist ];then
	./remakesourcelist
fi

aclocal
autoheader
touch NEWS README AUTHORS ChangeLog
automake --add-missing --copy
autoconf
./configure $@

