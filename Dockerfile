FROM ubuntu:20.04
MAINTAINER Nick Andrew <nick@nick-andrew.net>

RUN apt-get update
RUN apt-get -y install make gcc
