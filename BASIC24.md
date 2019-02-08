__BorosReed24__ is a simple magnetic sensor based on a reed switch to detect a report in real time the using __NRF24L01+__ compatible wireless modules.

Ideal for DIY projects, the device provide a flexible way add magnetic sensors to doors, windows, drawers... to your home/office atomation projects without hassle. There is no need to compile and flash the device it can be configured via usb with a PC using simple commands.

## How to use it:

> __WARNING:__ When connecting a Serial<->USB adpator/FTDI be sure that it proved only __3.3V__ power. __5V__ adaptors will __DESTROY__ the radio module.

1. Change the on-board switch to configuration mode (up/left side).
2. Connect a USB<->Serial __(3.3V)__ adapter to __BorosReed24__ FTDI header and to your PC board.
3. Open a terminal application at 9600,1,N. *(PuTTY/Screen/Minicom/miniterm)*
4. Configure your __BorosReed24__ via simple commands.
5. Unplug the USB<->Serial adapter.
7. Change the on-board switch to notification mode (down/right side).

done!

> __WARNING:__ When connecting a Serial<->USB adpator/FTDI be sure that it proved only __3.3V__ power. __5V__ adaptors will __DESTROY__ the module.

## Features:

- Light, small and compact: 
    - Coin size: 1.1x1.5 inches (30x35mm)
    - xx grams it can attached to aomost any surface.
    - Reed switch and battery holder is included on the board.
- Standard/cheap battery: Powered by a single CR2032 cell.
- Low-power design: years of battery with provided firmware.
- Felxible:
    - Support plain and mesh rf24 protocols.
    - Fully configurable via serial interface (requires a USB<->Serial adpator)
- Expandible & hackable:
    - Uses standard arduino ATMEGA328 with optimized bootloader.
    - Open source firmware you can tweek for your needs using standard arduino tools.
    - Breaks out I2C for adding additional sensors.
    - Breaks out Low level external interrupt.
    - Breaks out ICSP pins for fully reprogram the device.

## What is included?

- Fully assembled and programmed board ATMEGA328PB(M version).
- NRF24L01+ compatible wireless module based on SiR24 chip with enhanced transmission power.
- As option: soldered female or male pinheader and/or 2x small neodymium magents.

## What's not included

- CR2032 batery: 2032s are cheap and wide avaiable online or a convenience store.
- USB<->serial adaptor: genuie FTDI232 board is recommended.
- Case: 3D printable case .STL files can be downloaded form projects repository.
