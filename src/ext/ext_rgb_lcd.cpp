/*
  ext_rgb_lcd.cpp

  Defining extension methods of Arduino

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/
#include "mrubyc_config.h"

#ifdef USE_RGB_LCD

#include "mrubyc.h"
#include "ext.h"
#include <Wire.h>
#include <rgb_lcd.h>

static rgb_lcd* lcd=NULL;

static void class_rbg_lcd_initialize(mrb_vm *vm, mrb_value *v, int argc ){
  if(lcd!=NULL){
    DEBUG_PRINTLN("RGB_LCD is already existing!");
    return;
  }
  DEBUG_PRINTLN("Initialize RGB_LCD");
  lcd = new rgb_lcd();
  lcd->begin(16, 2);
}


static void class_rbg_lcd_clear(mrb_vm *vm, mrb_value *v, int argc ){
  if(lcd==NULL){
    SET_NIL_RETURN();
    return;
  }
  DEBUG_PRINTLN("lcd.clear");
  lcd->clear();
  //keep self
}

static void class_rbg_lcd_set_cursor(mrb_vm *vm, mrb_value *v, int argc ){
  if(lcd==NULL){
    SET_NIL_RETURN();
    return;
  }
  DEBUG_PRINTLN("lcd.set_cursor");
  //read arg
  if(argc!=2){
    SET_NIL_RETURN();
    DEBUG_PRINTLN("invalid argc");
    return;
  }
  int8_t x = GET_INT_ARG(1);
  int8_t y = GET_INT_ARG(2);

  lcd->setCursor(y,x);
  //keep self
}

static void class_rbg_lcd_write(mrb_vm *vm, mrb_value *v, int argc ){
  if(lcd==NULL){
    SET_NIL_RETURN();
    return;
  }
  const char* text = (const char *)GET_STRING_ARG(1);
  DEBUG_PRINTLN("lcd.write");
  lcd->write(text);
  //keep self
}

static void class_rbg_lcd_set_rgb(mrb_vm *vm, mrb_value *v, int argc ){
  if(lcd==NULL){
    SET_NIL_RETURN();
    return;
  }
  DEBUG_PRINTLN("lcd.set_rbg");
  int16_t r = GET_INT_ARG(1);
  int16_t g = GET_INT_ARG(2);
  int16_t b = GET_INT_ARG(3);

  lcd->setRGB(r,g,b);

  //keep self
}

void define_rgb_lcd_class(void){
  mrb_class *class_rbg_lcd;
  class_rbg_lcd = mrbc_define_class(0, "RGB_LCD", mrbc_class_object);
  mrbc_define_method(0, class_rbg_lcd, "initialize", class_rbg_lcd_initialize);
  mrbc_define_method(0, class_rbg_lcd, "clear", class_rbg_lcd_clear);
  mrbc_define_method(0, class_rbg_lcd, "set_cursor", class_rbg_lcd_set_cursor);
  mrbc_define_method(0, class_rbg_lcd, "write", class_rbg_lcd_write);
  mrbc_define_method(0, class_rbg_lcd, "set_rgb", class_rbg_lcd_set_rgb);
}


#endif //USE_RGB_LCD
