#!/bin/bash
echo Custom boot loader build

GCCROOT=$HOME/.platformio/packages/toolchain-atmelavr/bin
BUILD_DIR=optiboot/optiboot/bootloaders/optiboot

PWD_O=$(pwd)
cd optiboot
# Reset any change on the branch.
echo "Reseting respository"
git reset --hard
echo "\n\n Current commit->"
git show --oneline -s
cd ..

cd $BUILD_DIR

# Patch Makefile.custom with board defintion
cat <<EOF >> Makefile.custom
# borosReed24 based on atmega168pa/p
borosReed24: TARGET = atmega168p
borosReed24: MCU_TARGET = atmega168p
borosReed24: CFLAGS += \$(COMMON_OPTIONS) \$(UART_CMD)
borosReed24: AVR_FREQ ?= 1000000L 
borosReed24: LDSECTIONS = -Wl,--section-start=.text=0x3e00 -Wl,--section-start=.version=0x3ffe
borosReed24: \$(PROGRAM)_borosReed24_UART\$(UART)_\$(BAUD_RATE)_\$(AVR_FREQ).hex
borosReed24: \$(PROGRAM)_borosReed24_UART\$(UART)_\$(BAUD_RATE)_\$(AVR_FREQ).lst

\$(info FREQ Selected->\$(AVR_FREQ))
EOF


# use platformio tool chain
what="borosReed24"
# Using platformio toolchain.
toolchain="GCCROOT=$HOME/.platformio/packages/toolchain-atmelavr/bin/"
LED_OPTIONS="LED=B5 LED_DATA_FLASH=1"
COMMON_OPTIONS="UART=0 BAUD_RATE=9600"
make $what ${toolchain} ${COMMON_OPTIONS} ${LED_OPTIONS} AVR_FREQ=1000000L
rm baudcheck.tmp.sh
make $what ${toolchain} ${COMMON_OPTIONS} ${LED_OPTIONS} AVR_FREQ=8000000L
rm baudcheck.tmp.sh
cd $PWD_O
echo $(pwd)
echo "Copy results..."
echo $BUILD_DIR/*boros*.{hex,lst}
mv $BUILD_DIR/*boros*.{hex,lst} .
# Restoring submodule
cd optiboot
git reset --hard
echo "Done!"


