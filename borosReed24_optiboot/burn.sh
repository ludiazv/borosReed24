#!/bin/bash
# = Configuration =
# =====================
AVRDUDE_DIR="$HOME/.platformio/packages/tool-avrdude"
PROGRAMMER="usbasp"
BITCLOCK="-B 10"
PORT="usb"

# Options
echo "======================================================"
echo "Opiboot burner...."
echo "======================================================"
echo "Chose version:"
echo " 0- Exit"
echo " 1- BorosReed24 ATMEGA168P/PA - 1MHZ"
echo " 2- BorosReed24 ATMEGA168P/PA - 8MHz"
echo " 3- BorosReed24 Mesh ATMEGA328P/PA - 1MHZ"
echo " 4- BorosReed24 Mesh ATMEGA328P/PA - 8MHZ"
echo " 5- BorosReed24 Mesh ATMEGA328PB - 1MHZ"
echo " 6- BorosReed24 Mesh ATMEGA328PB - 8MHZ"
printf "?"
read -n1 option

# Bootloader file and model selection
case "$option" in
    1|2)
    PREFIX="./optiboot_borosReed24_UART0_9600"
    MODEL="m168p"
    ;;
    3|4)
    PREFIX="./optiboot_borosReed24M_UART0_9600"
    MODEL="m328p"
    ;;
    5|6)
    PREFIX="./optiboot_borosReed24MB_UART0_9600"
    MODEL="m328pb"
    ;;
    *)
      echo ""
      echo "Aborted!"
      exit 0
esac

# Freq selection
case "$option" in 
    1|3|5)
    FREQ="_1000000L.hex"
    ;;
    2|4|6)
    FREQ="_8000000L.hex"
    ;;
esac

BL_FILE="${PREFIX}${FREQ}"

# Fuse selection
case "$option" in
    1)
    # LFUSE ->Internal oscilator 8Mh/8=1Mhz + 0 delay startup + no Clock out on PB0 
    LFUSE="0x42"
    # HFUSE -> BOD disabled + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF + EESAVE
    HFUSE="0xD7"
    # EFUSE-> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader
    EFUSE="0xFC"
     ;;
    2)
    # LFUSE ->Internal oscilator 8Mh + 0 delay startup + no Clock out on PB0 
    LFUSE="0xC2"
    # HFUSE -> BOD disabled + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF + EESAVE
    HFUSE="0xD7"
    # EFUSE-> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader
    EFUSE="0xFC"
    ;;
    3|5)
    # LFUSE ->Internal oscilator 8Mh/8=1Mhz + 0 delay startup + no Clock out on PB0 
    LFUSE="0x42"
    # HFUSE -> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF +EESAVE
    HFUSE="0xD6"
    # EFUSE->     BOD disabled 
    EFUSE="0xFF"
     ;;

    4|6)
    # LFUSE ->Internal oscilator 8Mh + 0 delay startup + no Clock out on PB0 
    LFUSE="0xC2"
    # HFUSE -> Bootloader parameters 256 words(512bytes) + Reset vector to boot loader + Reset Enable + Debug disebled + SPI programming enbabled + WDT OFF +EESAVE
    HFUSE="0xD6"
    # EFUSE->     BOD disabled 
    EFUSE="0xFF"
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
echo
echo "Press Any key to continue [check the programmer is ready] or Ctrl+C to abort...."
read -n1 option

AVRD_CMD="$AVRDUDE_DIR/avrdude -C $AVRDUDE_DIR/avrdude.conf $VERBOSE -p $MODEL -P $PORT -c $PROGRAMMER $BITCLOCK "

echo "Setting up fuses...."
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


