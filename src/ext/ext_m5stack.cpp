/*
  ext_m5stack.cpp

  Defining extension methods of Arduino

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/
#include "mrubyc_config.h"

#ifdef ARDUINO_M5Stack_Core_ESP32

#include "mrubyc.h"
#include "ext.h"
#include <M5Stack.h>

static uint16_t sym_to_colorcode(mrb_sym sym_in){
	uint16_t col = BLACK;

	if(sym_in == str_to_symid("WHITE")){
		col = WHITE;
	}else if(sym_in == str_to_symid("RED")){
		col = RED;
	}else if(sym_in == str_to_symid("GREEN")){
		col = GREEN;
	}else if(sym_in == str_to_symid("BLUE")){
		col = BLUE;
	}else if(sym_in == str_to_symid("YELLOW")){
		col = YELLOW;
	}else if(sym_in == str_to_symid("BLACK")){
		col = BLACK;
	}
	return col;
}
static int arg_to_colorcode(mrb_value *v, int no, uint16_t* col){
	mrb_sym sym_in;
	if(GET_TT_ARG(no) == MRB_TT_SYMBOL){
		sym_in = GET_INT_ARG(no);
	}else if(GET_TT_ARG(no) == MRB_TT_STRING){
		sym_in = str_to_symid((const char *)GET_STRING_ARG(no));
	}else if(GET_TT_ARG(no) == MRB_TT_FIXNUM){
		*col = GET_INT_ARG(no);
		return true;
	}else{
		DEBUG_PRINT("arg_to_colorcode:Error! No=");
		DEBUG_PRINT(no);
		DEBUG_PRINT(" TT=");
		DEBUG_PRINT(GET_TT_ARG(no));
		DEBUG_PRINTLN("");
		SET_FALSE_RETURN();
		return false;
	}
	*col = sym_to_colorcode(sym_in);
	return true;
}
static void class_m5_update(mrb_vm *vm, mrb_value *v, int argc ){
	M5.update();
	SET_NIL_RETURN();
}

static void class_lcd_width(mrb_vm *vm, mrb_value *v, int argc ){
	SET_INT_RETURN(M5.Lcd.width());
}

static void class_lcd_height(mrb_vm *vm, mrb_value *v, int argc ){
	SET_INT_RETURN(M5.Lcd.height());
}

static void class_lcd_fill_screen(mrb_vm *vm, mrb_value *v, int argc ){
	uint16_t col=BLACK;
	if(argc==0){
		SET_FALSE_RETURN();
		return;
	}
	if(!arg_to_colorcode(v,1,&col)){
		DEBUG_PRINTLN("class_lcd_fill_screen:ERROR");
		SET_FALSE_RETURN();
		return;
	}
	M5.Lcd.fillScreen(col);
	SET_TRUE_RETURN();
}

static void class_lcd_set_cursor(mrb_vm *vm, mrb_value *v, int argc ){
	if(argc<2){
		DEBUG_PRINTLN("class_lcd_set_cursor:ERROR");
		SET_FALSE_RETURN();
		return;
	}
	int16_t x=0, y=0;
	x = GET_INT_ARG(1);
	y = GET_INT_ARG(2);
	M5.Lcd.setCursor(x, y);
	SET_NIL_RETURN();
}
static void class_lcd_set_text_color(mrb_vm *vm, mrb_value *v, int argc ){
	if(argc==0)return;

	uint16_t col = BLACK;
	if(!arg_to_colorcode(v,1,&col)){
		SET_FALSE_RETURN();
		return;
	}

	M5.Lcd.setTextColor(col);

	SET_NIL_RETURN();
}
static void class_lcd_set_text_size(mrb_vm *vm, mrb_value *v, int argc ){
	int16_t size=1;
	if(GET_TT_ARG(1) == MRB_TT_FIXNUM){
		size = GET_INT_ARG(1);
	}else{
		SET_FALSE_RETURN();
		return;
	}

	M5.Lcd.setTextSize(size);

	SET_NIL_RETURN();
}
static void class_lcd_printf(mrb_vm *vm, mrb_value *v, int argc ){
	if(argc==0){
		SET_FALSE_RETURN();
		return;
	}
	const char* text = (const char *)GET_STRING_ARG(1);
	M5.Lcd.printf(text);
	SET_NIL_RETURN();
}

static void disp_method_rect(mrb_vm *vm, mrb_value *v, int argc, uint8_t type ){
	if(argc<5){
		SET_FALSE_RETURN();
		return;
	}
	int32_t x,y,w,h;

	x = GET_INT_ARG(1);
	y = GET_INT_ARG(2);
	w = GET_INT_ARG(3);
	h = GET_INT_ARG(4);

	uint16_t col = BLACK;
	if(!arg_to_colorcode(v,5,&col)){
		SET_FALSE_RETURN();
		return;
	}

	if(type==0){
		M5.Lcd.drawRect(x, y, w, h, col);
	}else{
		M5.Lcd.fillRect(x, y, w, h, col);
	}

	SET_NIL_RETURN();
}
static void class_lcd_draw_rect(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_rect(vm, v, argc, 0);
}
static void class_lcd_fill_rect(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_rect(vm, v, argc, 1);
}

static void disp_method_circle(mrb_vm *vm, mrb_value *v, int argc, uint8_t type ){
	if(argc<4){
		DEBUG_PRINTLN("disp_method_circle:wrong argc");
		SET_FALSE_RETURN();
		return;
	}
	int32_t x,y,r;

	x = GET_INT_ARG(1);
	y = GET_INT_ARG(2);
	r = GET_INT_ARG(3);

	uint16_t col = BLACK;
	if(!arg_to_colorcode(v,4,&col)){
		SET_FALSE_RETURN();
		return;
	}

	if(type==0){
		M5.Lcd.drawCircle(x, y, r, col);
	}else{
		M5.Lcd.fillCircle(x, y, r, col);
	}

	SET_NIL_RETURN();
}

static void class_lcd_draw_circle(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_circle(vm,v,argc,0);
}
static void class_lcd_fill_circle(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_circle(vm,v,argc,1);
}
static void disp_method_triangle(mrb_vm *vm, mrb_value *v, int argc, uint8_t type ){
	if(argc<7){
		SET_FALSE_RETURN();
		return;
	}
	int32_t x0,y0,x1,y1,x2,y2;

	x0 = GET_INT_ARG(1);
	y0 = GET_INT_ARG(2);
	x1 = GET_INT_ARG(3);
	y1 = GET_INT_ARG(4);
	x2 = GET_INT_ARG(5);
	y2 = GET_INT_ARG(6);

	uint16_t col = BLACK;
	if(!arg_to_colorcode(v,7,&col)){
		SET_FALSE_RETURN();
		return;
	}

	if(type==0){
		M5.Lcd.drawTriangle(x0, y0, x1, y1, x2, y2, col);
	}else{
		M5.Lcd.fillTriangle(x0, y0, x1, y1, x2, y2, col);
	}

	SET_NIL_RETURN();
}
static void class_lcd_draw_triangle(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_triangle(vm,v,argc,0);
}
static void class_lcd_fill_triangle(mrb_vm *vm, mrb_value *v, int argc ){
	disp_method_triangle(vm,v,argc,1);
}

void define_m5stack_class(void){
	M5.begin();
	mrb_class *class_m5;
	class_m5 = mrbc_define_class(0, "M5", mrbc_class_object);
	mrbc_define_method(0, class_m5, "update", class_m5_update);

	mrb_class *class_lcd;
	class_lcd = mrbc_define_class(0, "Lcd", mrbc_class_object);
	mrbc_define_method(0, class_lcd, "width", class_lcd_width);
	mrbc_define_method(0, class_lcd, "height", class_lcd_height);
	mrbc_define_method(0, class_lcd, "fill_screen", class_lcd_fill_screen);
	mrbc_define_method(0, class_lcd, "set_cursor", class_lcd_set_cursor);
	mrbc_define_method(0, class_lcd, "set_text_color", class_lcd_set_text_color);
	mrbc_define_method(0, class_lcd, "set_text_size", class_lcd_set_text_size);
	mrbc_define_method(0, class_lcd, "printf", class_lcd_printf);
	mrbc_define_method(0, class_lcd, "draw_rect", class_lcd_draw_rect);
	mrbc_define_method(0, class_lcd, "fill_rect", class_lcd_fill_rect);
	mrbc_define_method(0, class_lcd, "draw_circle", class_lcd_draw_circle);
	mrbc_define_method(0, class_lcd, "fill_circle", class_lcd_fill_circle);
	mrbc_define_method(0, class_lcd, "draw_triangle", class_lcd_draw_triangle);
	mrbc_define_method(0, class_lcd, "fill_triangle", class_lcd_fill_triangle);

}

#endif //ARDUINO_M5Stack_Core_ESP32
