/*! @file
  @brief
  mruby/c Fixnum and Float class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/


#include "vm_config.h"
#include "opcode.h"
#include <stdio.h>
#if MRBC_USE_FLOAT
#include <math.h>
#endif

#include "value.h"
#include "static.h"
#include "class.h"
#include "console.h"
#include "c_numeric.h"
#include "c_string.h"


//================================================================
/*! (operator) [] bit reference
 */
static void c_fixnum_bitref(mrb_vm *vm, mrb_value v[], int argc)
{
  if( 0 <= v[1].i && v[1].i < 32 ) {
    SET_INT_RETURN( (v[0].i & (1 << v[1].i)) ? 1 : 0 );
  } else {
    SET_INT_RETURN( 0 );
  }
}


//================================================================
/*! (operator) unary -
*/
static void c_fixnum_negative(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(0);
  SET_INT_RETURN( -num );
}


//================================================================
/*! (operator) ** power
 */
static void c_fixnum_power(mrb_vm *vm, mrb_value v[], int argc)
{
  if( v[1].tt == MRB_TT_FIXNUM ) {
    int32_t x = 1;
    int i;

    if( v[1].i < 0 ) x = 0;
    for( i = 0; i < v[1].i; i++ ) {
      x *= v[0].i;;
    }
    SET_INT_RETURN( x );
  }

#if MRBC_USE_FLOAT && MRBC_USE_MATH
  else if( v[1].tt == MRB_TT_FLOAT ) {
    SET_FLOAT_RETURN( pow( v[0].i, v[1].d ) );
  }
#endif
}


//================================================================
/*! (operator) %
 */
static void c_fixnum_mod(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i % num );
}


//================================================================
/*! (operator) &; bit operation AND
 */
static void c_fixnum_and(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(1);
  SET_INT_RETURN(v->i & num);
}


//================================================================
/*! (operator) |; bit operation OR
 */
static void c_fixnum_or(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(1);
  SET_INT_RETURN(v->i | num);
}


//================================================================
/*! (operator) ^; bit operation XOR
 */
static void c_fixnum_xor(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i ^ num );
}


//================================================================
/*! (operator) ~; bit operation NOT
 */
static void c_fixnum_not(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t num = GET_INT_ARG(0);
  SET_INT_RETURN( ~num );
}


//================================================================
/*! x-bit left shift for x
 */
static int32_t shift(int32_t x, int32_t y)
{
  if( y >= 33 ){
    x = 0;
  } else if( y >= 0 ){
    x <<= y;
  } else if( y > -33 ){
    x = x >> -y;
  } else {
    x = 0;
  }
  return x;
}


//================================================================
/*! (operator) <<; bit operation LEFT_SHIFT
 */
static void c_fixnum_lshift(mrb_vm *vm, mrb_value v[], int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, num) );
}


//================================================================
/*! (operator) >>; bit operation RIGHT_SHIFT
 */
static void c_fixnum_rshift(mrb_vm *vm, mrb_value v[], int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, -num) );
}


//================================================================
/*! (method) abs
*/
static void c_fixnum_abs(mrb_vm *vm, mrb_value v[], int argc)
{
  if( v[0].i < 0 ) {
    v[0].i = -v[0].i;
  }
}


#if MRBC_USE_FLOAT
//================================================================
/*! (method) to_f
*/
static void c_fixnum_to_f(mrb_vm *vm, mrb_value v[], int argc)
{
  double f = GET_INT_ARG(0);
  SET_FLOAT_RETURN( f );
}
#endif


//================================================================
/*! (method) times
*/
static void c_fixnum_times(mrb_vm *vm, mrb_value v[], int argc)
{
  uint32_t code[2] = {
    MKOPCODE(OP_CALL) | MKARG_A(argc),
    MKOPCODE(OP_ABORT)
  };
  mrb_irep irep = {
    0,     // nlocals
    0,     // nregs
    0,     // rlen
    2,     // ilen
    0,     // plen
    (uint8_t *)code,   // iseq
    NULL,  // pools
    NULL,  // ptr_to_sym
    NULL,  // reps
  };

  // count of times
  int cnt = v[0].i;

  mrb_value block = v[1];
  mrbc_dup(&block);

  mrbc_push_callinfo(vm, 0);

  // adjust reg_top for reg[0]==Proc
  vm->current_regs += v - vm->regs + 1;

  int i;
  for( i=0 ; i<cnt ; i++ ){
    if(v[1].tt!=MRB_TT_PROC){//TODO:in normal case, ref is increased. Need to be checked
      v[1]=block;
      if(block.proc->ref_count < 2) mrbc_dup(&block);
    }
    // set index
    mrbc_release( &v[2] );
    v[2].tt = MRB_TT_FIXNUM;
    v[2].i = i;

    // set OP_CALL irep
    vm->pc = 0;
    vm->pc_irep = &irep;

    // execute OP_CALL
    mrbc_vm_run(vm);
  }
  mrbc_pop_callinfo(vm);
}



#if MRBC_USE_STRING
//================================================================
/*! (method) chr
*/
static void c_fixnum_chr(mrb_vm *vm, mrb_value v[], int argc)
{
  char buf[2] = { GET_INT_ARG(0) };

  mrb_value value = mrbc_string_new(vm, buf, 1);
  SET_RETURN(value);
}


//================================================================
/*! (method) to_s
*/
static void c_fixnum_to_s(mrb_vm *vm, mrb_value v[], int argc)
{
  int base = 10;
  if( argc ) {
    base = GET_INT_ARG(1);
    if( base < 2 || base > 36 ) {
      return;	// raise ? ArgumentError
    }
  }

  mrb_printf pf;
  char buf[16];
  mrbc_printf_init( &pf, buf, sizeof(buf), NULL );
  pf.fmt.type = 'd';
  mrbc_printf_int( &pf, v->i, base );
  mrbc_printf_end( &pf );

  mrb_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}
#endif



void mrbc_init_class_fixnum(mrb_vm *vm)
{
  // Fixnum
  mrbc_class_fixnum = mrbc_define_class(vm, "Fixnum", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_fixnum, "[]", c_fixnum_bitref);
  mrbc_define_method(vm, mrbc_class_fixnum, "-@", c_fixnum_negative);
  mrbc_define_method(vm, mrbc_class_fixnum, "**", c_fixnum_power);
  mrbc_define_method(vm, mrbc_class_fixnum, "%", c_fixnum_mod);
  mrbc_define_method(vm, mrbc_class_fixnum, "&", c_fixnum_and);
  mrbc_define_method(vm, mrbc_class_fixnum, "|", c_fixnum_or);
  mrbc_define_method(vm, mrbc_class_fixnum, "^", c_fixnum_xor);
  mrbc_define_method(vm, mrbc_class_fixnum, "~", c_fixnum_not);
  mrbc_define_method(vm, mrbc_class_fixnum, "<<", c_fixnum_lshift);
  mrbc_define_method(vm, mrbc_class_fixnum, ">>", c_fixnum_rshift);
  mrbc_define_method(vm, mrbc_class_fixnum, "abs", c_fixnum_abs);
  mrbc_define_method(vm, mrbc_class_fixnum, "to_i", c_ineffect);
  mrbc_define_method(vm, mrbc_class_fixnum, "times", c_fixnum_times);
#if MRBC_USE_FLOAT
  mrbc_define_method(vm, mrbc_class_fixnum, "to_f", c_fixnum_to_f);
#endif
#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_fixnum, "chr", c_fixnum_chr);
  mrbc_define_method(vm, mrbc_class_fixnum, "to_s", c_fixnum_to_s);
#endif
}


// Float
#if MRBC_USE_FLOAT

//================================================================
/*! (operator) unary -
*/
static void c_float_negative(mrb_vm *vm, mrb_value v[], int argc)
{
  double num = GET_FLOAT_ARG(0);
  SET_FLOAT_RETURN( -num );
}


#if MRBC_USE_MATH
//================================================================
/*! (operator) ** power
 */
static void c_float_power(mrb_vm *vm, mrb_value v[], int argc)
{
  double n = 0;
  switch( v[1].tt ) {
  case MRB_TT_FIXNUM:	n = v[1].i;	break;
  case MRB_TT_FLOAT:	n = v[1].d;	break;
  default:				break;
  }

  SET_FLOAT_RETURN( pow( v[0].d, n ));
}
#endif


//================================================================
/*! (method) abs
*/
static void c_float_abs(mrb_vm *vm, mrb_value v[], int argc)
{
  if( v[0].d < 0 ) {
    v[0].d = -v[0].d;
  }
}


//================================================================
/*! (method) to_i
*/
static void c_float_to_i(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t i = (int32_t)GET_FLOAT_ARG(0);
  SET_INT_RETURN( i );
}


#if MRBC_USE_STRING
//================================================================
/*! (method) to_s
*/
static void c_float_to_s(mrb_vm *vm, mrb_value v[], int argc)
{
  char buf[16];

  snprintf( buf, sizeof(buf), "%g", v->d );
  mrb_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}
#endif


//================================================================
/*! initialize class Float
*/
void mrbc_init_class_float(mrb_vm *vm)
{
  // Float
  mrbc_class_float = mrbc_define_class(vm, "Float", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_float, "-@", c_float_negative);
#if MRBC_USE_MATH
  mrbc_define_method(vm, mrbc_class_float, "**", c_float_power);
#endif
  mrbc_define_method(vm, mrbc_class_float, "abs", c_float_abs);
  mrbc_define_method(vm, mrbc_class_float, "to_i", c_float_to_i);
  mrbc_define_method(vm, mrbc_class_float, "to_f", c_ineffect);
#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_float, "to_s", c_float_to_s);
#endif
}

#endif
