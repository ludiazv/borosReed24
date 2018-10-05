#!/bin/bash

LIB_PATH="lib"

LIBS=( "https://github.com/rocketscream/Low-Power.git"
        "https://github.com/nRF24/RF24.git"
        "https://github.com/nRF24/RF24Network.git"
        "https://github.com/nRF24/RF24Mesh.git" )

PATHS=( "$LIB_PATH/Low-Power" 
        "$LIB_PATH/RF24" 
        "$LIB_PATH/RF24Network" 
        "$LIB_PATH/RF24Mesh" )


VERSIONS=( "v1.8" 
           "v1.3.1" 
           "2d1099581aebede5e0c40eb14cf6adaccbce3ba0" 
           "1d77422c667c3011ce507f469736606d8da5752a" )

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
    git submodule update
    for i in "${!LIBS[@]}" ; do
        printf "Lib:${LIBS[$i]} Version:${VERSIONS[$i]} in ${PATHS[$i]} ..."
        cd ${PATHS[$i]}
        git checkout ${VERSIONS[$i]}
        git show --oneline -s
        cd $CWD
    done
fi

echo "Done!"