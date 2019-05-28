FROM python:2.7

MAINTAINER ShiftCrops <simiyu@shift-crops.net>

RUN apt update && \
	apt install -y netcat && \
	apt clean && \
	rm -rf /var/lib/apt/lists/*

ADD exploit.sh /root
ADD files/memo /root

WORKDIR /root
CMD ./exploit.sh
