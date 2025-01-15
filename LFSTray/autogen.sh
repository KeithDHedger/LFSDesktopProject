#!/bin/bash -e

aclocal
autoheader
autoconf
automake --add-missing
./configure $@
