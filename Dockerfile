FROM ubuntu:10.04
MAINTAINER Nick Andrew <nick@nick-andrew.net>

RUN perl -p -i -e 's/archive/old-releases/' /etc/apt/sources.list
RUN apt-get update
RUN apt-get -y install make gcc
