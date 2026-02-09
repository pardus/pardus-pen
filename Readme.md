# Pardus Pen
Simple qt based pen application

## Features
* Pen, marker, eraser tools
* Line spline, circle drawing
* Color selection
* Thickness selection
* Pen preview
* White, black, transparent backgrounds
* Square, line, none overlays
* Multiple pages
* Undo, redo buttons
* Screenshot
* Clear, exit confirm dialogs
* Minimize button
* Multi touch support
* Gfx tablet support
* Movable floating toolbox

## How to build
### Installing Dependencies
For debian:

`apt install meson ninja-build libarchive-dev pkgconf qt6-base-dev libqt6svg6-dev libpoppler-qt6-dev`

For archlinux:

`pacman -Syu meson ninja pkgconf libarchive qt6-base qt6-tools poppler-qt6`

For alpine:

`apk add meson ninja-build qt6-qtbase-dev qt6-qttools-dev libarchive-dev poppler-qt6`

### Building
```
meson setup build
ninja -C build
```

### Installing
```
ninja -C build install
```

## How to create deb package
### Installing Dependencies
```
apt install -yq meson devscripts
```
### Create Deb Package
```
# run as root
mk-build-deps -ir
# run as user
dpkg-buildpackage -us -uc
```
