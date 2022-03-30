#!/bin/sh

MODULE="ch3_mod_201912698_dev"
MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)

mknod /dev/$MODULE c $MAJOR 0
