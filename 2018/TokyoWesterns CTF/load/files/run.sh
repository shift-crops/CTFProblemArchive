#!/bin/sh

exec socat - system:./load,pty,raw,echo=0
