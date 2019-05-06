#! /bin/bash

cd /home/mboard
ulimit -m 262144
ulimit -t 2
ulimit -u 8
exec ./mvees_sandbox --replicas=1 --level=2 --out-limit=8192 --deny=11 ./mboard 2>&1
