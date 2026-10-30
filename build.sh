#!/bin/bash

echo "Starting build"
g++ main.cpp packet_parser.cpp ./npcap-sdk-1.16/Lib/x64/wpcap.lib -o main.exe -I./libpcap1 -lws2_32
echo "Build finished successfully!"



