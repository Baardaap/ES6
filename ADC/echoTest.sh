#!/bin/bash
# A test script to continously test the ADC driver
echo "Starting echo test script"
while true; do
    cat /dev/ADC0
    cat /dev/ADC1
    cat /dev/ADC2
done