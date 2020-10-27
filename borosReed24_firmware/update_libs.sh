#!/bin/bash

LIB_PATH="lib"

LIBS=(  "https://github.com/nRF24/RF24.git"
        "https://github.com/nRF24/RF24Network.git"
        "https://github.com/nRF24/RF24Mesh.git" 
        "https://github.com/mcci-catena/arduino-lmic"
        "https://github.com/ludiazv/arduino-SHTC3.git")

PATHS=( "$LIB_PATH/RF24" 
        "$LIB_PATH/RF24Network" 
        "$LIB_PATH/RF24Mesh"
        "$LIB_PATH/arduino-lmic"
        "$LIB_PATH/arduino-SHTC3" )


VERSIONS=( "v1.3.9" 
           "v1.0.13" 
           "v1.1.3"
           "v3.2.0" 
           "v0.0.3")

CWD=$(pwd)

echo "Install or update[i/U]?"
read -n1 option
echo ""
if [ "$option" == "i" ] ; then

    echo "Installing libs..."
    for i in "${!LIBS[@]}" ; do
        printf "Lib:${LIBS[$i]} Version:${VERSIONS[$i]} in ${PATHS[$i]} ..."
        if [ -d "${PATHS[$i]}" ] ; then
            cd ${PATHS[$i]}
            git checkout ${VERSIONS[$i]}
            git show --oneline -s
            cd $CWD
            echo "skipped!"
        else 
            git submodule add -b master ${LIBS[$i]} ${PATHS[$i]}
            cd ${PATHS[$i]}
            git checkout ${VERSIONS[$i]}
            git show --oneline -s
            cd $CWD
            echo "installed!"
        fi
    done

else
    echo "Update Libraries...."
    git submodule update --remote
    for i in "${!LIBS[@]}" ; do
        printf "Lib:${LIBS[$i]} Version:${VERSIONS[$i]} in ${PATHS[$i]} ..."
        cd ${PATHS[$i]}
        git checkout ${VERSIONS[$i]}
        git show --oneline -s
        cd $CWD
        echo "updated!"
    done
fi

# Patching is not necessay using new define 
#echo "Patching lmic_project_config.h"
#echo "//Removed as it configured via platformio INI\n" > $LIB_PATH/arduino-lmic/project_config/lmic_project_config.h

echo "Done!"