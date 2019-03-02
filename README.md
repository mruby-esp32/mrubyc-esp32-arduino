# mrubyc_for_ESP32_Arduino


An Arduino library of mruby/c for ESP32 Arduino

This library can be used for M5Stack.


**This is my experimental work. There is no guarantee about this library.**

## Usage

### Condition

This library depends on ESP32 Arduino library. Please install it in advance.

### Import the library to Arduino IDE

Clone this repository.

    clone https://github.com/kishima/mrubyc_for_ESP32_Arduino.git

Copy "mrubyc_for_ESP32_Arduino" to your Arduino library directory.

    cp -r mrubyc_for_ESP32_Arduino [your document directory]/Arduino/libraries/mrubyc_for_ESP32_Arduino

Include a header file mrubyc_for_ESP32_Arduino.h.

### Implement mruby/c functions

Nothing changed in basic usage of mruby/c. You can use mruby/c APIs.
I added following function(s).

- mrbc_define_user_class

You can refer example files about detailed usage.
Of course, you need compiled mruby byte code to run on it.

For example, a LED connected to GPIO 4 can be controlled by following script.

```rb
puts "mruby/c example: control LED"
Arduino.pin_mode(4,:OUTPUT)
while true
	Arduino.digital_write(4,:HIGH)
	sleep(1)
	Arduino.digital_write(4,:LOW)
	sleep(1)
end
```

## M5Stack support

This library can be applied to M5Stack.
In source code, a flag "ARDUINO_M5Stack_Core_ESP32" that comes from compile options is checked.
If it is defined, M5 class will be defined. This class is also in development stage. Please refer ext_m5stack.cpp if you want to check more detail.

## Configuration

Some configuration parameters can be modified in "mrubyc_config.h".

1. USE_USB_SERIAL_FOR_STDIO  
   If this is defined, standard out is connected to USB Serial(UART0). Default baudrate is 115200.
1. USE_M5AVATAR  
   If this is defined, M5Stack Avatar class is defined. (Under construction...)
   https://github.com/meganetaaan/m5stack-avatar
1. USE_RGB_LCD
   If this is defined, RGB_LCD class is defined. It partially supports the Grove RGB character LCD provided by Seeed.
   https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight
1. ENABLE_RMIRB  
   If this is defined, remote mirb is available. In that case, mruby/c is only used for remote mrib because this affects VM behavior.
1. ESP32_DEBUG  
   If this is defined, some debug messages are shown.

## Future work (if I'm good...)

- define more mruby methods of Arduino library.
- define more mruby methods of ESP32 library.
- implement an automatical executer and an uploader of mruby byte code.
- implement a function about downloading mruby byte code Over The Air.

You may find latest news about this library in my blog.

https://silentworlds.info/

## Copyright

Refer LICENSE file.
