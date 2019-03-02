/*
  ext_arduino.cpp

  Defining extension methods of Arduino

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/

#include "mrubyc_config.h"
#include "mrubyc.h"
#include "ext.h"
#include <Wire.h>

static void class_arduino_delay(mrb_vm *vm, mrb_value *v, int argc )
{
	int d=GET_INT_ARG(1);
	delay(d);
	SET_TRUE_RETURN();
}

static uint8_t sym_to_pinmode(mrb_sym sym_in){
	uint8_t mode = INPUT;

	if(sym_in == str_to_symid("INPUT")){
		mode = INPUT;
	}else if(sym_in == str_to_symid("OUTPUT")){
		mode = OUTPUT;
	}else if(sym_in == str_to_symid("INPUT_PULLUP")){
		mode = INPUT_PULLUP;
	}
	return mode;
}

static uint8_t sym_to_siglevel(mrb_sym sym_in){
	uint8_t sig = LOW;

	if(sym_in == str_to_symid("HIGH")){
		sig = HIGH;
	}else{
		sig = LOW;
	}
	return sig;
}

static void class_arduino_pin_mode(mrb_vm *vm, mrb_value *v, int argc )
{
	int pin = 0;
	if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
		pin = GET_INT_ARG(1);
	}else{
		SET_FALSE_RETURN();
		return;
	}
	//DEBUG_PRINT("pin=");
	//DEBUG_PRINTLN(pin);

	mrb_sym sym_in = 0;
	if(GET_TT_ARG(2) == MRB_TT_SYMBOL){
		sym_in = GET_INT_ARG(2);
	}else if(GET_TT_ARG(2) == MRB_TT_STRING){
		sym_in = str_to_symid((const char *)GET_STRING_ARG(2));
	}else{
		SET_FALSE_RETURN();
		return;
	}
	uint8_t mode = sym_to_pinmode( sym_in );
	//DEBUG_PRINT("mode=");
	//DEBUG_PRINTLN(mode);

	pinMode(pin,mode);
	SET_TRUE_RETURN();
}

static void class_arduino_digital_wirte(mrb_vm *vm, mrb_value *v, int argc )
{
	int pin = 0;
	if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
		pin = GET_INT_ARG(1);
	}else{
		SET_FALSE_RETURN();
		return;
	}
	mrb_sym sym_in = 0;
	if(GET_TT_ARG(2) == MRB_TT_SYMBOL){
		sym_in = GET_INT_ARG(2);
	}else if(GET_TT_ARG(2) == MRB_TT_STRING){
		sym_in = str_to_symid((const char *)GET_STRING_ARG(2));
	}else{
		SET_FALSE_RETURN();
		return;
	}
	uint8_t sig = sym_to_siglevel( sym_in );

	digitalWrite(pin,sig);
	SET_TRUE_RETURN();
}
static void class_arduino_digital_read(mrb_vm *vm, mrb_value *v, int argc )
{
	int pin = 0;
	if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
		pin = GET_INT_ARG(1);
	}else{
		SET_FALSE_RETURN();
		return;
	}
	uint8_t sig = INPUT;
	sig = digitalRead(pin);
	SET_INT_RETURN(sig);
}

static void class_arduino_random(mrb_vm *vm, mrb_value *v, int argc )
{
	int32_t i=0;
	int32_t min=0,max=0;
	if(argc==2){
		if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
			min = GET_INT_ARG(1);
		}else{
			SET_FALSE_RETURN();
			return;
		}
		if(GET_TT_ARG(2) == MRB_TT_FIXNUM){
			max = GET_INT_ARG(2);
		}else{
			SET_FALSE_RETURN();
			return;
		}
	}else if(argc==1){
		if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
			max = GET_INT_ARG(1);
		}else{
			SET_FALSE_RETURN();
			return;
		}
	}else{
		SET_FALSE_RETURN();
		return;
	}
	i = random(min,max);
	SET_INT_RETURN(i);
}

void define_arduino_class()
{
	mrb_class *class_arduino;
	class_arduino = mrbc_define_class(0, "Arduino", mrbc_class_object);
	mrbc_define_method(0, class_arduino, "delay", class_arduino_delay);
	mrbc_define_method(0, class_arduino, "pin_mode", class_arduino_pin_mode);
	mrbc_define_method(0, class_arduino, "digital_write", class_arduino_digital_wirte);
	mrbc_define_method(0, class_arduino, "digital_read", class_arduino_digital_read);
	mrbc_define_method(0, class_arduino, "random", class_arduino_random);

}

static HardwareSerial* HwSerial = NULL;

static void class_serial_begin(mrb_vm *vm, mrb_value *v, int argc )
{
  //TODO: re-consider better interface implementation
  //      Pin no should be set by user
  if(HwSerial==NULL){
    HwSerial = new HardwareSerial(2);
    //Don't use Serial1 since these ports are used by SPI-Flash
  }
  int baud = GET_INT_ARG(1);
  DEBUG_PRINT("Serial(2)->begin baudrate=");
  DEBUG_PRINTLN(baud);
  HwSerial->begin(baud);
  SET_TRUE_RETURN();
}
static void class_serial_end(mrb_vm *vm, mrb_value *v, int argc )
{
	HwSerial->end();
	SET_TRUE_RETURN();
}
static void class_serial_available(mrb_vm *vm, mrb_value *v, int argc )
{
	int res = HwSerial->available();
	if(res!=0){
		SET_TRUE_RETURN();
	}else{
		SET_FALSE_RETURN();
	}
}
static void class_serial_readline(mrb_vm *vm, mrb_value *v, int argc )
{
	static char readbuff[120];
	static int readp=0;

	while (HwSerial->available()) {
		char data = HwSerial->read();
		if (data == '\r') continue;
		if (data == '\n') {
			readbuff[readp] = '\0';
			readp = 0;
			mrb_value string = mrbc_string_new_cstr(vm,(const char*)readbuff);
			SET_RETURN(string);
			return;
		}

		if (readp > sizeof (readbuff) - 1) {
			readp = 0;
			DEBUG_PRINT("\nOVERFLOW\n");
			SET_FALSE_RETURN();
			return;
		}
		readbuff[readp++] = data;
	}
	SET_NIL_RETURN();
}
static void class_serial_write(mrb_vm *vm, mrb_value *v, int argc )
{
	DEBUG_PRINT("class_serial_write not supported\n");
}

void define_serial_class()
{
	mrb_class *class_serial;
	class_serial = mrbc_define_class(0, "Serial", mrbc_class_object);
	mrbc_define_method(0, class_serial, "begin", class_serial_begin);
	mrbc_define_method(0, class_serial, "end", class_serial_end);
	mrbc_define_method(0, class_serial, "available", class_serial_available);
	mrbc_define_method(0, class_serial, "readline", class_serial_readline);
	mrbc_define_method(0, class_serial, "write", class_serial_write);
}
