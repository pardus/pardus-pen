#!/bin/bash
set -e
xgettext -o po/pardus-pen.pot --from-code="utf-8" \
    --language=C++ -v --keyword=_\
    `find src -type f -iname "*.cpp"`\
    `find src -type f -iname "*.c"`
for file in `ls po/*.po`; do
    msgmerge $file po/pardus-pen.pot -o $file.new
    echo POT: $file
    rm -f $file
    mv $file.new $file
done
