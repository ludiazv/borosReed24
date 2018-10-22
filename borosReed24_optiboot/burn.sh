#!/bin/bash
# = Configuration =
# =====================
AVRDUDE_DIR="$HOME/.platformio/packages/tool-avrdude"
PROGRAMMER="usbasp"
BITCLOCK="-B 10"
PORT="usb"
PREFIX="./optiboot_borosReed24_UART0_9600"
PREFIXM="./optiboot_borosReed24M_UART0_9600"

# Options
echo "======================================================"
echo "Opiboot burner...."
echo "======================================================"
echo "Chose version:"
echo " 0- Exit"
echo " 1- ATMEGA168P/PA - 1MHZ"
echo " 2- ATMEGA168P/PA - 8MHz"
echo " 3- ATMEGA328P/PA - 1MHZ"
echo " 4- ATMEGA328P/PA - 8MHZ"
printf "?"
read -n1 option

case "$option" in
    1)
    # LFUSE ->Internal oscilator 8Mh/8=1Mhz + 0 delay startup + Clock out on PB0 
    LFUSE="0x02"
    # HFUSE -> BOD disabled + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF + EESAVE
    HFUSE="0xD7"
    # EFUSE-> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader
    EFUSE="0xFC"
    MODEL="m168"
    BL_FILE="${PREFIX}_1000000L.hex"
     ;;
    2)
    # LFUSE ->Internal oscilator 8Mh + 0 delay startup + Clock out on PB0 
    LFUSE="0x82"
    # HFUSE -> BOD disabled + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF + EESAVE
    HFUSE="0xD7"
    # EFUSE-> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader
    EFUSE="0xFC"
    MODEL="m168p"
    BL_FILE="${PREFIX}_8000000L.hex"
    ;;
    3)
    # LFUSE ->Internal oscilator 8Mh/8=1Mhz + 0 delay startup + Clock out on PB0 
    LFUSE="0x02"
    # HFUSE -> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF +EESAVE
    HFUSE="0xD6"
    # EFUSE->     BOD disabled 
    EFUSE="0xFF"
    MODEL="m328p"
    BL_FILE="${PREFIXM}_1000000L.hex"
     ;;

    4)
    # LFUSE ->Internal oscilator 8Mh + 0 delay startup + Clock out on PB0 
    LFUSE="0x82"
    # HFUSE -> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF +EESAVE
    HFUSE="0xD6"
    # EFUSE->     BOD disabled 
    EFUSE="0xFF"
    MODEL="m328p"
    BL_FILE="${PREFIXM}_1000000L.hex"
    ;;
    *)
      echo ""
      echo "Aborted!"
      exit 0
esac

echo 
echo "Do you whant verbose output [N/y]?"
read -n1 option
if [ "$option" == "y" ] ; then
    VERBOSE="-v"
else
    VERBOSE=""
fi

echo "======================================================"
echo "Start burn processs..."
echo "======================================================"
echo "AVR-DUDE: $AVRDUDE_DIR/avrdude"
echo "MCU Model: $MODEL"
echo "Programmer: $PROGRAMMER / port: $PORT"
echo "BitClock: $BITCLOCK"
echo "FUSES: L: $LFUSE , H: $HFUSE , E: $EFUSE"
echo "VERBOSE: $VERBOSE"
echo "Bootloader: $BL_FILE"
echo "======================================================"

AVRD_CMD="$AVRDUDE_DIR/avrdude -C $AVRDUDE_DIR/avrdude.conf $VERBOSE -p $MODEL -P $PORT -c $PROGRAMMER $BITCLOCK "

echo "Settin up fuses...."
echo "======================================================"
ACMD="-U lfuse:w:$LFUSE:m -U hfuse:w:$HFUSE:m -U efuse:w:$EFUSE:m"
echo "Command [ $ACMD ]"
$AVRD_CMD $ACMD
echo "======================================================"
echo "Burning optiboot $BL_FILE ..."
ACMD="-e -U flash:w:$BL_FILE:i"
echo "Command [ $ACMD ]"
$AVRD_CMD $ACMD
echo "======================================================"

echo "Done!"


