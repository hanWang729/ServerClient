FROM ubuntu:20.04
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y iproute2
RUN apt-get install -y iputils-ping
RUN apt-get update
COPY output/server /usr/bin/server
CMD /usr/bin/server