#!/bin/bash
./greeting | stdbuf -o 0 head -c 131072
