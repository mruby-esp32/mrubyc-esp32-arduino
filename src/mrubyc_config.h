/*
  libmrubyc_config.h

  Header file for Arduino application

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/
#ifndef __LIBMRUBYC_CONFIG_H__
#define __LIBMRUBYC_CONFIG_H__

#define ESP32MRBC_VERSION "0.1.2"

/* Specific Devices */
#define USE_USB_SERIAL_FOR_STDIO
#define SERIAL_FOR_STDIO_BAUDRATE 115200
/* for debug */
#define ESP32_DEBUG


/* for M5Stack */
//#define USE_M5AVATAR

/* for Seeed RGB LCD */
/* Please install depending library: https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight */
#define USE_RGB_LCD

/* for remote mrib */
//#define ENABLE_RMIRB

#endif
