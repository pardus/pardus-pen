#!/bin/bash
set -e
if [ "$QT_PATH" == "" ] ; then
    export QT_PATH="/usr/lib64/qt5"
fi
# cleanup
rm -rf build || true
# build
meson setup build --prefix=/usr -Dresources=true "$@"
ninja -C build -j`nproc`
# create dir
mkdir -p build/appimage/{lib,plugins}
# install base files
install build/pardus-pen build/appimage/pardus-pen
install appimage/AppRun build/appimage/AppRun
install data/tr.org.pardus.pen.svg build/appimage/
install data/tr.org.pardus.pen.desktop build/appimage/
# install interpreter
interpreter=$(ldd build/pardus-pen | grep /ld- | cut -f1 -d" " | tr -d "\t")
install "$interpreter" build/appimage/$(basename "$interpreter")
sed -i "s/@interpreter@/$(basename $interpreter)/g" build/appimage/AppRun
# install qt
cp -prvf "${QT_PATH}"/plugins/* build/appimage/plugins
# install libs
ldd $(find build/appimage -type f) build/pardus-pen | cut -f3 -d" " | sort -V | while read line ; do
    if [ -f "$line" ] ; then
        install "$line" build/appimage/lib/$(basename "$line")
    fi
done
# create appimage
wget https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-$(uname -m).AppImage -O build/appimagetool
chmod 755 build/appimagetool
export ARCH=$(uname -m)
cd build ; ./appimagetool ./appimage
