FROM ubuntu:14.04
MAINTAINER Nick Andrew <nick@nick-andrew.net>

# Ubuntu trusty is not yet in old-releases.ubuntu.com
# RUN perl -p -i -e 's/archive/old-releases/' /etc/apt/sources.list
RUN apt-get update
RUN apt-get -y install make gcc
