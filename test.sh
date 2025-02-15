#!/bin/sh
echo "starting on /dev/input/event$1"
intercept -g "/dev/input/event$1" | ./1806 | uinput -d "/dev/input/event$1"
