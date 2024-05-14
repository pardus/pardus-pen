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

`apt install meson ninja-build qtbase5-dev qtchooser libglib2.0-dev`

For archlinux:

`pacman -Sy meson ninja qt5-base qt5-tools glib2`

For alpine:

`apk add qt5-qtbase-dev qt5-qttools-dev glib-dev`

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
