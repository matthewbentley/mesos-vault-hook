FROM ubuntu:trusty

RUN sudo apt-key adv --keyserver keyserver.ubuntu.com --recv E56151BF
RUN echo "deb http://repos.mesosphere.io/ubuntu trusty main" > /etc/apt/sources.list.d/mesosphere.list

RUN export DEBIAN_FRONTEND=noninteractive && apt-get update && apt-get install -y libgoogle-glog-dev libboost-dev libsvn-dev libsasl2-dev libcurl4-openssl-dev libtool build-essential autoconf libprotobuf-dev ruby-dev git && gem install fpm

RUN export DEBIAN_FRONTEND=noninteractive && apt-get install -y mesos=0.28.2-2.0.27.ubuntu1404
