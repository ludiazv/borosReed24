#!/bin/bash

VERSIONS=( br24 bm24 brTTNEU868 bmTTNEU868 )
DBG_VERSIONS=()
ALL_VERSIONS=()

for i in "${VERSIONS[@]}"
do
    DBG_VERSIONS+=( "${i}_dbg" )
    ALL_VERSIONS+=( "${i}" )
    ALL_VERSIONS+=( "${i}_dbg")
done

echo "BOROSREED BUILDER..."
echo "Versions:" "${VERSIONS[@]}" "Debug:" "${DBG_VERSIONS[@]}"


if [ $# -lt 1 ] ; then
    echo "Bad usage: $0 <version number>"
    exit 1
fi
VER="V$1"
echo "Building version $VER..."
BASE="$(dirname $(realpath "$0"))/.."
BASE=$(realpath $BASE)
RELEASE_PATH="$BASE/releases"
VERPATH="${RELEASE_PATH}/$VER"
FIRMPATH="$BASE/borosReed24_firmware"
echo "Using base path=$BASE , release path=$RELEASE_PATH , version path=$VERPATH"
echo "Press any key to continue...CRTL+C to abort"
read -n 1 dummy

mkdir -p $RELEASE_PATH
mkdir -p $VERPATH

pushd $FIRMPATH
for v in "${ALL_VERSIONS[@]}"
do
    pio run -t clean -e $v 
    pio run -e $v
    if [ $? -eq 0 ] ; then
        cp .pioenvs/${v}/firmware.hex ${VERPATH}/${v}.hex
    else
        pio run -t clean -e $v
        echo "FAIL!!"
        popd
        exit 2
    fi
    pio run -t clean -e $v

done
popd
echo "Done!"