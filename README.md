# A flexible magnetic/reed sensor with nrf24l01(+) wireless or sx127x LoRA/LoRAWAN

__BorosReed24__ / __BorosReedLoRa__ is a simple magnetic sensor based on a reed switch to detect a report in real time the using __NRF24L01+__ or Sx127x wireless modules.

Ideal for DIY projects, the device provide a flexible way add magnetic sensors to doors, windows, drawers... to your home/office atomation projects without hassle. There is no need to compile and flash the device it can be configured via usb with a PC using simple commands.

## Usage

> __WARNING:__ When connecting a Serial<->USB adpator/FTDI be sure that it proved only __3.3V__ power. __5V__ adaptors will __DESTROY__ the module.

1. Change the on-board switch to configuration mode (up/left side).
2. Connect a USB<->Serial __(3.3V)__ adapter to __BorosReed24__ FTDI header and to your PC board.
3. Open a terminal application at 9600,1,N. *(PuTTY/Screen/Minicom/miniterm)*
4. Configure your __BorosReed24__ via simple commands.
5. Unplug the USB<->Serial adapter.
6. Change the on-board switch to notification mode (down/right side).

done!

> __WARNING:__ When connecting a Serial<->USB adpator/FTDI be sure that it proved only __3.3V__ power. __5V__ adaptors will __DESTROY__ the module.


## Guides

1. NRF24L01 guide
2. LoraWan  guide
3. Hardware reference

