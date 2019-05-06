#!/bin/bash
./judgement | stdbuf -o 0 head -c 1024
