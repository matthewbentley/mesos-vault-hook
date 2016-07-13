#!/bin/bash
VERSION=0.1.4
EXTRA=yelp1

cd /src/src
./bootstrap
mkdir /build
cd /build
/src/src/configure --with-mesos=/usr/local/ --prefix=/usr
make
mkdir /dest
make install DESTDIR=/dest

fpm -s dir -t deb --deb-no-default-config-files -n mesos-vault-hook -v $VERSION --iteration $EXTRA -C /dest -p mesos-vault-hook_VERSION_ARCH.deb -d "mesos" usr/lib
cp mesos-vault-hook*.deb /dist/trusty/
