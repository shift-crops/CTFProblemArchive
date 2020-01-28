FROM ubuntu:18.04

MAINTAINER ShiftCrops <simiyu@shift-crops.net>

RUN apt-get update && apt-get -y upgrade \
 && apt install -y socat curl \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*
RUN groupadd user && useradd -g user user

COPY send_flag /send_flag
RUN chmod 755 /send_flag

COPY --chown=root:user memo /home/user/memo

WORKDIR /home/user/
USER user

EXPOSE 4296
CMD ["socat", "TCP-LISTEN:4296,reuseaddr", "EXEC:/home/user/memo"]
