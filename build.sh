#!/bin/bash
VERSION=0.0.1
EXTRA=yelp1

cd /build/src
./bootstrap
mkdir build
cd build
../configure --with-mesos=/usr/local/ --prefix=/usr
make
mkdir /dest
make install DESTDIR=/dest

fpm -s dir -t deb --deb-no-default-config-files -n mesos-hooks -v $VERSION --iteration $EXTRA -C /dest -p mesos-hooks_VERSION_ARCH.deb -d "mesos" usr/lib
cp mesos-hooks*.deb /dist/trusty/
