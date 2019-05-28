FROM python:2.7

MAINTAINER ShiftCrops <simiyu@shift-crops.net>

RUN pip install pwntools && \
	wget -P /opt/sc_expwn/ https://raw.githubusercontent.com/shift-crops/sc_expwn/master/sc_expwn.py

ADD exploit.py /root
ADD files/babyheap /root
ADD files/libc-2.27.so /root

ENV PYTHONPATH "${PYTHONPATH}:/opt/sc_expwn"
ENV TERM "linux"

WORKDIR /root
CMD ./exploit.py
