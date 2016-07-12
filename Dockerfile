FROM docker-dev.yelpcorp.com/trusty_yelp

RUN export DEBIAN_FRONTEND=noninteractive && apt-get update && apt-get install -y libgoogle-glog-dev libboost-dev libsvn-dev libsasl2-dev libcurl4-openssl-dev libtool build-essential autoconf mesos libprotobuf-dev ruby-dev git && gem install fpm
