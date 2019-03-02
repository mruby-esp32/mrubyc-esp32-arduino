/*! @file
  @brief
  mruby/c Range object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include "value.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "c_range.h"
#include "console.h"
#include "opcode.h"


//================================================================
/*! constructor

  @param  vm		pointer to VM.
  @param  first		pointer to first value.
  @param  last		pointer to last value.
  @param  flag_exclude	true: exclude the end object, otherwise include.
  @return		range object.
*/
mrb_value mrbc_range_new( struct VM *vm, mrb_value *first, mrb_value *last, int flag_exclude)
{
  mrb_value value = {.tt = MRB_TT_RANGE};

  value.range = mrbc_alloc(vm, sizeof(mrb_range));
  if( !value.range ) return value;		// ENOMEM

  value.range->ref_count = 1;
  value.range->tt = MRB_TT_STRING;	// TODO: for DEBUG
  value.range->flag_exclude = flag_exclude;
  value.range->first = *first;
  value.range->last = *last;

  return value;
}


//================================================================
/*! destructor

  @param  target 	pointer to range object.
*/
void mrbc_range_delete(mrb_value *v)
{
  mrbc_release( &v->range->first );
  mrbc_release( &v->range->last );

  mrbc_raw_free( v->range );
}


//================================================================
/*! clear vm_id
*/
void mrbc_range_clear_vm_id(mrb_value *v)
{
  mrbc_set_vm_id( v->range, 0 );
  mrbc_clear_vm_id( &v->range->first );
  mrbc_clear_vm_id( &v->range->last );
}


//================================================================
/*! compare
*/
int mrbc_range_compare(const mrb_value *v1, const mrb_value *v2)
{
  int res;

  res = mrbc_compare( &v1->range->first, &v2->range->first );
  if( res != 0 ) return res;

  res = mrbc_compare( &v1->range->last, &v2->range->last );
  if( res != 0 ) return res;

  return (int)v2->range->flag_exclude - (int)v1->range->flag_exclude;
}



//================================================================
/*! (method) ===
*/
static void c_range_equal3(mrb_vm *vm, mrb_value v[], int argc)
{
  int result = 0;

  mrb_value *v_first = &v[0].range->first;
  mrb_value *v_last =&v[0].range->last;
  mrb_value *v1 = &v[1];

  if( v_first->tt == MRB_TT_FIXNUM && v1->tt == MRB_TT_FIXNUM ) {
    if( v->range->flag_exclude ) {
      result = (v_first->i <= v1->i) && (v1->i < v_last->i);
    } else {
      result = (v_first->i <= v1->i) && (v1->i <= v_last->i);
    }
    goto DONE;
  }
  console_printf( "Not supported\n" );
  return;

 DONE:
  if( result ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}


//================================================================
/*! (method) first
*/
static void c_range_first(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_range_first(v);
  SET_RETURN(ret);
}


//================================================================
/*! (method) last
*/
static void c_range_last(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_range_last(v);
  SET_RETURN(ret);
}



//================================================================
/*! (method) each
*/
static void c_range_each(mrb_vm *vm, mrb_value v[], int argc)
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

  // get range
  mrb_range *range = v[0].range;

  mrbc_push_callinfo(vm, 0);

  // adjust reg_top for reg[0]==Proc
  vm->current_regs += v - vm->regs + 1;

  if( range->first.tt == MRB_TT_FIXNUM && range->last.tt == MRB_TT_FIXNUM ){
    int i, i_last = range->last.i;
    if( range->flag_exclude ) i_last--;
    for( i=range->first.i ; i<=i_last ; i++ ){
      v[2].tt = MRB_TT_FIXNUM;
      v[2].i = i;
      // set OP_CALL irep
      vm->pc = 0;
      vm->pc_irep = &irep;
      
      // execute OP_CALL
      mrbc_vm_run(vm);
    }
  } else {
    console_printf( "Not supported\n" );
  }
  
  mrbc_pop_callinfo(vm);
}



//================================================================
/*! initialize
*/
void mrbc_init_class_range(mrb_vm *vm)
{
  mrbc_class_range = mrbc_define_class(vm, "Range", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_range, "===", c_range_equal3);
  mrbc_define_method(vm, mrbc_class_range, "first", c_range_first);
  mrbc_define_method(vm, mrbc_class_range, "last", c_range_last);
  mrbc_define_method(vm, mrbc_class_range, "each", c_range_each);
}
