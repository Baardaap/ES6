#!/bin/sh
module="PWMMod"
device="PWMMod"
major="253"
# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
/sbin/insmod ./$module.ko $* || exit 1
# remove stale nodes
rm -f /dev/${device}[0-7]
major=$(awk "\\$2= =\"$module\" {print \\$1}" /proc/devices)
mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3
mknod /dev/${device}3 c $major 4
mknod /dev/${device}3 c $major 5
mknod /dev/${device}3 c $major 6
mknod /dev/${device}3 c $major 7
mknod /dev/${device}3 c $major 8