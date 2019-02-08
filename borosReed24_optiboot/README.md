# Custom Build of optiboot for Reed24 Board

This directory contains some scripts and helpers to build and flash Reed bootloader.

## Custom parameters

UART=0: Bootloader runs on UART0
BAUD_RATE=9600 : Fix low speed BAUD_RATE
LED=D4 LED_DATA_FLASH=1 LED_START_FLASHES=2  : Use on board led D4 falshing on boot loader activation.

## Build 

```bash
./buil.sh
```

## Flash bootloader
`
``bash
./buil.sh
```

### Submodule init/update

Procedure:

```bash
# This for init
git submodule add -b master https://github.com/Optiboot/optiboot.git borosReed24_optiboot/optiboot

git submodule init
cd borosReed24_optiboot/optiboot

# Check out the version -> V8.0
git checkout $VERSION

#This will  update the master branch
git submodule update

```

### Deinit Submodule

```bash
git submodule deinit -f -- a/submodule    
rm -rf .git/modules/a/submodule
git rm -f a/submodule

```

## Change Log:

- Inital version using optiboot v8.0