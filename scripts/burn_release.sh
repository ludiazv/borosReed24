#!/bin/bash

AVRDUDE_PATH="$HOME/.platformio/packages/tool-avrdude"
AVRDUDE="${AVRDUDE_PATH}/avrdude"
AVRDUDE_OPTS="-C ${AVRDUDE_PATH}/avrdude.conf -v -c arduino -b 9600 -D"
echo "BOOROSREED BURN SCRIPT"
echo "======================"

if [ $# -lt 2 ] ; then
    echo "Bad usage: $0 <version number> </dev/port>"
    exit 1
fi

VER="V$1"

BASE="$(dirname $(realpath "$0"))/.."
BASE=$(realpath $BASE)
RELEASE_PATH="$BASE/releases"
VERPATH="${RELEASE_PATH}/$VER"

echo "Choose version:"
echo "---------------"
VERSIONS=( $(ls $VERPATH) )
for i in "${!VERSIONS[@]}"
do
 printf $i
 echo " - ${VERSIONS[$i]}"
done
echo "----------------"
printf "0-${#VERSIONS[@]} >"
read selection
if [ $selection -lt ${#VERSIONS[@]} ] ; then
    AVRCMD="$AVRDUDE $AVRDUDE_OPTS -p atmega328pb -P $2"
    echo "Clear EEPROM..."
    $AVRCMD -U eeprom:w:0x00:m
    echo "Uploading..."
    $AVRCMD -U flash:w:${VERPATH}/${VERSIONS[$selection]}:i
else
    echo "Selection not valid"
    exit 0
fi
