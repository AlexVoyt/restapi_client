#!/bin/bash
INCLUDE_DIR="external/"
g++ -I$INCLUDE_DIR -g -Wall -lcurl main.cpp -o main
