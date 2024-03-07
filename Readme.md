# Pardus Pen
Simple qt based pen application

## How to build
```
meson setup build
ninja -C build
```

## How to make .deb package

To create a Debian package, you must install the following packages.

```
sudo apt install build-essential debhelper dh-make
```
Then you can follow the steps below;

```
meson setup build
ninja -C build
cd build/
dh_make --createorig
dpkg-buildpackage -us -uc
```

