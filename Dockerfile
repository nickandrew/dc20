FROM ubuntu:16.04
MAINTAINER Nick Andrew <nick@nick-andrew.net>

RUN apt-get update
RUN apt-get -y install make gcc
