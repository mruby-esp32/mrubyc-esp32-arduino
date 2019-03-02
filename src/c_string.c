/*! @file
  @brief
  mruby/c String object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "value.h"
#include "vm.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "symbol.h"
#include "c_string.h"
#include "console.h"


#if MRBC_USE_STRING
//================================================================
/*! constructor

  @param  vm	pointer to VM.
  @param  src	source string or NULL
  @param  len	source length
  @return 	string object
*/
mrb_value mrbc_string_new(struct VM *vm, const void *src, int len)
{
  mrb_value value = {.tt = MRB_TT_STRING};

  /*
    Allocate handle and string buffer.
  */
  mrb_string *h;
  h = (mrb_string *)mrbc_alloc(vm, sizeof(mrb_string));
  if( !h ) return value;		// ENOMEM

  uint8_t *str = mrbc_alloc(vm, len+1);
  if( !str ) {				// ENOMEM
    mrbc_raw_free( h );
    return value;
  }

  h->ref_count = 1;
  h->tt = MRB_TT_STRING;	// TODO: for DEBUG
  h->size = len;
  h->data = str;

  /*
    Copy a source string.
  */
  if( src == NULL ) {
    str[0] = '\0';
  } else {
    memcpy( str, src, len );
    str[len] = '\0';
  }

  value.string = h;
  return value;
}


//================================================================
/*! constructor by c string

  @param  vm	pointer to VM.
  @param  src	source string or NULL
  @return 	string object
*/
mrb_value mrbc_string_new_cstr(struct VM *vm, const char *src)
{
  return mrbc_string_new(vm, src, (src ? strlen(src) : 0));
}


//================================================================
/*! constructor by allocated buffer

  @param  vm	pointer to VM.
  @param  buf	pointer to allocated buffer
  @param  len	length
  @return 	string object
*/
mrb_value mrbc_string_new_alloc(struct VM *vm, void *buf, int len)
{
  mrb_value value = {.tt = MRB_TT_STRING};

  /*
    Allocate handle
  */
  mrb_string *h;
  h = (mrb_string *)mrbc_alloc(vm, sizeof(mrb_string));
  if( !h ) return value;		// ENOMEM

  h->ref_count = 1;
  h->tt = MRB_TT_STRING;	// TODO: for DEBUG
  h->size = len;
  h->data = buf;

  value.string = h;
  return value;
}


//================================================================
/*! destructor

  @param  str	pointer to target value
*/
void mrbc_string_delete(mrb_value *str)
{
  mrbc_raw_free(str->string->data);
  mrbc_raw_free(str->string);
}



//================================================================
/*! clear vm_id
*/
void mrbc_string_clear_vm_id(mrb_value *str)
{
  mrbc_set_vm_id( str->string, 0 );
  mrbc_set_vm_id( str->string->data, 0 );
}


//================================================================
/*! duplicate string

  @param  vm	pointer to VM.
  @param  s1	pointer to target value 1
  @param  s2	pointer to target value 2
  @return	new string as s1 + s2
*/
mrb_value mrbc_string_dup(struct VM *vm, mrb_value *s1)
{
  mrb_string *h1 = s1->string;

  mrb_value value = mrbc_string_new(vm, NULL, h1->size);
  if( value.string == NULL ) return value;		// ENOMEM

  memcpy( value.string->data, h1->data, h1->size + 1 );

  return value;
}


//================================================================
/*! add string (s1 + s2)

  @param  vm	pointer to VM.
  @param  s1	pointer to target value 1
  @param  s2	pointer to target value 2
  @return	new string as s1 + s2
*/
mrb_value mrbc_string_add(struct VM *vm, mrb_value *s1, mrb_value *s2)
{
  mrb_string *h1 = s1->string;
  mrb_string *h2 = s2->string;

  mrb_value value = mrbc_string_new(vm, NULL, h1->size + h2->size);
  if( value.string == NULL ) return value;		// ENOMEM

  memcpy( value.string->data,            h1->data, h1->size );
  memcpy( value.string->data + h1->size, h2->data, h2->size + 1 );

  return value;
}


//================================================================
/*! append string (s1 += s2)

  @param  s1	pointer to target value 1
  @param  s2	pointer to target value 2
  @param	mrb_error_code
*/
int mrbc_string_append(mrb_value *s1, mrb_value *s2)
{
  int len1 = s1->string->size;
  int len2 = (s2->tt == MRB_TT_STRING) ? s2->string->size : 1;

  uint8_t *str = mrbc_raw_realloc(s1->string->data, len1+len2+1);
  if( !str ) return E_NOMEMORY_ERROR;

  if( s2->tt == MRB_TT_STRING ) {
    memcpy(str + len1, s2->string->data, len2 + 1);
  } else if( s2->tt == MRB_TT_FIXNUM ) {
    str[len1] = s2->i;
    str[len1+1] = '\0';
  }

  s1->string->size = len1 + len2;
  s1->string->data = str;

  return 0;
}


//================================================================
/*! locate a substring in a string

  @param  src		pointer to target string
  @param  pattern	pointer to substring
  @param  offset	search offset
  @return		position index. or minus value if not found.
*/
int mrbc_string_index(mrb_value *src, mrb_value *pattern, int offset)
{
  char *p1 = mrbc_string_cstr(src) + offset;
  char *p2 = mrbc_string_cstr(pattern);
  int try_cnt = mrbc_string_size(src) - mrbc_string_size(pattern) - offset;

  while( try_cnt >= 0 ) {
    if( memcmp( p1, p2, mrbc_string_size(pattern) ) == 0 ) {
      return p1 - mrbc_string_cstr(src);	// matched.
    }
    try_cnt--;
    p1++;
  }

  return -1;
}


//================================================================
/*! remove the whitespace in myself

  @param  src	pointer to target value
  @param  mode	1:left-side, 2:right-side, 3:each
  @return	0 when not removed.
*/
int mrbc_string_strip(mrb_value *src, int mode)
{
  static const char ws[] = " \t\r\n\f\v";	// '\0' on tail
  char *p1 = mrbc_string_cstr(src);
  char *p2 = p1 + mrbc_string_size(src) - 1;
  int n_left = 0;

  // left-side
  if( mode & 0x01 ) {
    n_left = strspn( p1, ws );
    p1 += n_left;
  }

  // right-side
  if( mode & 0x02 ) {
    while( p1 <= p2 ) {
      int i;
      for( i = 0; i < sizeof(ws); i++ ) {
	if( *p2 == ws[i] ) goto NEXTLOOP;
      }
      break;	// not match

    NEXTLOOP:
      p2--;
    }
  }

  int new_size = p2 - p1 + 1;
  if( mrbc_string_size(src) == new_size ) return 0;

  char *buf = mrbc_string_cstr(src);
  if( n_left ) memmove( buf, p1, new_size );
  buf[new_size] = '\0';
  mrbc_raw_realloc(buf, new_size+1);	// shrink suitable size.
  src->string->size = new_size;

  return 1;
}


//================================================================
/*! remove the CR,LF in myself

  @param  src	pointer to target value
  @return	0 when not removed.
*/
int mrbc_string_chomp(mrb_value *src)
{
  char *p1 = mrbc_string_cstr(src);
  char *p2 = p1 + mrbc_string_size(src) - 1;

  if( *p2 == '\n' ) {
    p2--;
  }
  if( *p2 == '\r' ) {
    p2--;
  }

  int new_size = p2 - p1 + 1;
  if( mrbc_string_size(src) == new_size ) return 0;

  char *buf = mrbc_string_cstr(src);
  buf[new_size] = '\0';
  src->string->size = new_size;

  return 1;
}



//================================================================
/*! (method) +
*/
static void c_string_add(mrb_vm *vm, mrb_value v[], int argc)
{
  if( v[1].tt != MRB_TT_STRING ) {
    console_print( "Not support STRING + Other\n" );
    return;
  }

  mrb_value value = mrbc_string_add(vm, &v[0], &v[1]);
  SET_RETURN(value);
}



//================================================================
/*! (method) ===
*/
static void c_string_eql(mrb_vm *vm, mrb_value v[], int argc)
{
  int result = 0;
  if( v[1].tt != MRB_TT_STRING ) goto DONE;

  mrb_string *h1 = v[0].string;
  mrb_string *h2 = v[1].string;

  if( h1->size != h2->size ) goto DONE;	// false
  result = !memcmp(h1->data, h2->data, h1->size);

 DONE:
  if( result ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}



//================================================================
/*! (method) size, length
*/
static void c_string_size(mrb_vm *vm, mrb_value v[], int argc)
{
  int32_t size = mrbc_string_size(&v[0]);

  SET_INT_RETURN( size );
}



//================================================================
/*! (method) to_i
*/
static void c_string_to_i(mrb_vm *vm, mrb_value v[], int argc)
{
  int base = 10;
  if( argc ) {
    base = v[1].i;
    if( base < 2 || base > 36 ) {
      return;	// raise ? ArgumentError
    }
  }

  int32_t i = mrbc_atoi( mrbc_string_cstr(v), base );

  SET_INT_RETURN( i );
}


#if MRBC_USE_FLOAT
//================================================================
/*! (method) to_f
*/
static void c_string_to_f(mrb_vm *vm, mrb_value v[], int argc)
{
  double d = atof(mrbc_string_cstr(v));

  SET_FLOAT_RETURN( d );
}
#endif


//================================================================
/*! (method) <<
*/
static void c_string_append(mrb_vm *vm, mrb_value v[], int argc)
{
  if( !mrbc_string_append( &v[0], &v[1] ) ) {
    // raise ? ENOMEM
  }
}


//================================================================
/*! (method) []
*/
static void c_string_slice(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value *v1 = &v[1];
  mrb_value *v2 = &v[2];

  /*
    in case of slice(nth) -> String | nil
  */
  if( argc == 1 && v1->tt == MRB_TT_FIXNUM ) {
    int len = v->string->size;
    int idx = v1->i;
    int ch = -1;
    if( idx >= 0 ) {
      if( idx < len ) {
        ch = *(v->string->data + idx);
      }
    } else {
      idx += len;
      if( idx >= 0 ) {
        ch = *(v->string->data + idx);
      }
    }
    if( ch < 0 ) goto RETURN_NIL;

    mrb_value value = mrbc_string_new(vm, NULL, 1);
    if( !value.string ) goto RETURN_NIL;		// ENOMEM

    value.string->data[0] = ch;
    value.string->data[1] = '\0';
    SET_RETURN(value);
    return;		// normal return
  }

  /*
    in case of slice(nth, len) -> String | nil
  */
  if( argc == 2 && v1->tt == MRB_TT_FIXNUM && v2->tt == MRB_TT_FIXNUM ) {
    int len = v->string->size;
    int idx = v1->i;
    if( idx < 0 ) idx += len;
    if( idx < 0 ) goto RETURN_NIL;

    int rlen = (v2->i < (len - idx)) ? v2->i : (len - idx);
						// min( v2->i, (len-idx) )
    if( rlen < 0 ) goto RETURN_NIL;

    mrb_value value = mrbc_string_new(vm, v->string->data + idx, rlen);
    if( !value.string ) goto RETURN_NIL;		// ENOMEM

    SET_RETURN(value);
    return;		// normal return
  }

  /*
    other case
  */
  console_print( "Not support such case in String#[].\n" );
  return;


 RETURN_NIL:
  SET_NIL_RETURN();
}


//================================================================
/*! (method) []=
*/
static void c_string_insert(mrb_vm *vm, mrb_value v[], int argc)
{
  int nth;
  int len;
  mrb_value *val;

  /*
    in case of self[nth] = val
  */
  if( argc == 2 &&
      v[1].tt == MRB_TT_FIXNUM &&
      v[2].tt == MRB_TT_STRING ) {
    nth = v[1].i;
    len = 1;
    val = &v[2];
  }
  /*
    in case of self[nth, len] = val
  */
  else if( argc == 3 &&
	   v[1].tt == MRB_TT_FIXNUM &&
	   v[2].tt == MRB_TT_FIXNUM &&
	   v[3].tt == MRB_TT_STRING ) {
    nth = v[1].i;
    len = v[2].i;
    val = &v[3];
  }
  /*
    other cases
  */
  else {
    console_print( "Not support\n" );
    return;
  }

  int len1 = v->string->size;
  int len2 = val->string->size;
  if( nth < 0 ) nth = len1 + nth;               // adjust to positive number.
  if( len > len1 - nth ) len = len1 - nth;
  if( nth < 0 || nth > len1 || len < 0) {
    console_print( "IndexError\n" );  // raise?
    return;
  }

  uint8_t *str = mrbc_realloc(vm, mrbc_string_cstr(v), len1 + len2 - len + 1);
  if( !str ) return;

  memmove( str + nth + len2, str + nth + len, len1 - nth - len + 1 );
  memcpy( str + nth, mrbc_string_cstr(val), len2 );
  v->string->size = len1 + len2 - len;

  v->string->data = str;
}


//================================================================
/*! (method) chomp
*/
static void c_string_chomp(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_dup(vm, &v[0]);

  mrbc_string_chomp(&ret);

  SET_RETURN(ret);
}


//================================================================
/*! (method) chomp!
*/
static void c_string_chomp_self(mrb_vm *vm, mrb_value v[], int argc)
{
  if( mrbc_string_chomp(&v[0]) == 0 ) {
    SET_RETURN( mrb_nil_value() );
  }
}


//================================================================
/*! (method) dup
*/
static void c_string_dup(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_dup(vm, &v[0]);

  SET_RETURN(ret);
}


//================================================================
/*! (method) index
*/
static void c_string_index(mrb_vm *vm, mrb_value v[], int argc)
{
  int index;
  int offset;

  if( argc == 1 ) {
    offset = 0;

  } else if( argc == 2 && v[2].tt == MRB_TT_FIXNUM ) {
    offset = v[2].i;
    if( offset < 0 ) offset += mrbc_string_size(&v[0]);
    if( offset < 0 ) goto NIL_RETURN;

  } else {
    goto NIL_RETURN;	// raise? ArgumentError
  }

  index = mrbc_string_index(&v[0], &v[1], offset);
  if( index < 0 ) goto NIL_RETURN;

  SET_INT_RETURN(index);
  return;

 NIL_RETURN:
  SET_NIL_RETURN();
}


//================================================================
/*! (method) ord
*/
static void c_string_ord(mrb_vm *vm, mrb_value v[], int argc)
{
  int i = mrbc_string_cstr(v)[0];

  SET_INT_RETURN( i );
}


//================================================================
/*! (method) sprintf
*/
static void c_object_sprintf(mrb_vm *vm, mrb_value v[], int argc)
{
  static const int BUF_INC_STEP = 32;	// bytes.

  mrb_value *format = &v[1];
  if( format->tt != MRB_TT_STRING ) {
    console_printf( "TypeError\n" );	// raise?
    return;
  }

  int buflen = BUF_INC_STEP;
  char *buf = mrbc_alloc(vm, buflen);
  if( !buf ) { return; }	// ENOMEM raise?

  mrb_printf pf;
  mrbc_printf_init( &pf, buf, buflen, mrbc_string_cstr(format) );

  int i = 2;
  int ret;
  while( 1 ) {
    mrb_printf pf_bak = pf;
    ret = mrbc_printf_main( &pf );
    if( ret == 0 ) break;	// normal break loop.
    if( ret < 0 ) goto INCREASE_BUFFER;

    if( i > argc ) {console_print("ArgumentError\n"); break;}	// raise?

    // maybe ret == 1
    switch(pf.fmt.type) {
    case 'c':
      if( v[i].tt == MRB_TT_FIXNUM ) {
	ret = mrbc_printf_char( &pf, v[i].i );
      }
      break;

    case 's':
      if( v[i].tt == MRB_TT_STRING ) {
	ret = mrbc_printf_str( &pf, mrbc_string_cstr( &v[i] ), ' ');
      } else if( v[i].tt == MRB_TT_SYMBOL ) {
	ret = mrbc_printf_str( &pf, mrbc_symbol_cstr( &v[i] ), ' ');
      }
      break;

    case 'd':
    case 'i':
    case 'u':
      if( v[i].tt == MRB_TT_FIXNUM ) {
	ret = mrbc_printf_int( &pf, v[i].i, 10);
#if MRBC_USE_FLOAT
      } else if( v[i].tt == MRB_TT_FLOAT ) {
	ret = mrbc_printf_int( &pf, (int32_t)v[i].d, 10);
#endif
      } else if( v[i].tt == MRB_TT_STRING ) {
	int32_t ival = atol(mrbc_string_cstr(&v[i]));
	ret = mrbc_printf_int( &pf, ival, 10 );
      }
      break;

    case 'b':
    case 'B':
      if( v[i].tt == MRB_TT_FIXNUM ) {
	ret = mrbc_printf_int( &pf, v[i].i, 2);
      }
      break;

    case 'x':
    case 'X':
      if( v[i].tt == MRB_TT_FIXNUM ) {
	ret = mrbc_printf_int( &pf, v[i].i, 16);
      }
      break;

#if MRBC_USE_FLOAT
    case 'f':
    case 'e':
    case 'E':
    case 'g':
    case 'G':
      if( v[i].tt == MRB_TT_FLOAT ) {
	ret = mrbc_printf_float( &pf, v[i].d );
      } else
	if( v[i].tt == MRB_TT_FIXNUM ) {
	  ret = mrbc_printf_float( &pf, (double)v[i].i );
	}
      break;
#endif

    default:
      break;
    }
    if( ret >= 0 ) {
      i++;
      continue;		// normal next loop.
    }

    // maybe buffer full. (ret == -1)
    if( pf.fmt.width > BUF_INC_STEP ) buflen += pf.fmt.width;
    pf = pf_bak;

  INCREASE_BUFFER:
    buflen += BUF_INC_STEP;
    buf = mrbc_realloc(vm, pf.buf, buflen);
    if( !buf ) { return; }	// ENOMEM raise? TODO: leak memory.
    mrbc_printf_replace_buffer(&pf, buf, buflen);
  }
  mrbc_printf_end( &pf );

  buflen = mrbc_printf_len( &pf );
  mrbc_realloc(vm, pf.buf, buflen+1);	// shrink suitable size.

  mrb_value value = mrbc_string_new_alloc( vm, pf.buf, buflen );

  SET_RETURN(value);
}


//================================================================
/*! (method) lstrip
*/
static void c_string_lstrip(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_dup(vm, &v[0]);

  mrbc_string_strip(&ret, 0x01);	// 1: left side only

  SET_RETURN(ret);
}


//================================================================
/*! (method) lstrip!
*/
static void c_string_lstrip_self(mrb_vm *vm, mrb_value v[], int argc)
{
  if( mrbc_string_strip(&v[0], 0x01) == 0 ) {	// 1: left side only
    SET_RETURN( mrb_nil_value() );
  }
}


//================================================================
/*! (method) rstrip
*/
static void c_string_rstrip(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_dup(vm, &v[0]);

  mrbc_string_strip(&ret, 0x02);	// 2: right side only

  SET_RETURN(ret);
}


//================================================================
/*! (method) rstrip!
*/
static void c_string_rstrip_self(mrb_vm *vm, mrb_value v[], int argc)
{
  if( mrbc_string_strip(&v[0], 0x02) == 0 ) {	// 2: right side only
    SET_RETURN( mrb_nil_value() );
  }
}


//================================================================
/*! (method) strip
*/
static void c_string_strip(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_dup(vm, &v[0]);

  mrbc_string_strip(&ret, 0x03);	// 3: left and right

  SET_RETURN(ret);
}


//================================================================
/*! (method) strip!
*/
static void c_string_strip_self(mrb_vm *vm, mrb_value v[], int argc)
{
  if( mrbc_string_strip(&v[0], 0x03) == 0 ) {	// 3: left and right
    SET_RETURN( mrb_nil_value() );
  }
}


//================================================================
/*! (method) to_sym
*/
static void c_string_to_sym(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_symbol_new(vm, mrbc_string_cstr(&v[0]));

  SET_RETURN(ret);
}



//================================================================
/*! initialize
*/
void mrbc_init_class_string(struct VM *vm)
{
  mrbc_class_string = mrbc_define_class(vm, "String", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_string, "+",	c_string_add);
  mrbc_define_method(vm, mrbc_class_string, "===",	c_string_eql);
  mrbc_define_method(vm, mrbc_class_string, "size",	c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "length",	c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "to_i",	c_string_to_i);
  mrbc_define_method(vm, mrbc_class_string, "to_s",	c_ineffect);
  mrbc_define_method(vm, mrbc_class_string, "<<",	c_string_append);
  mrbc_define_method(vm, mrbc_class_string, "[]",	c_string_slice);
  mrbc_define_method(vm, mrbc_class_string, "[]=",	c_string_insert);
  mrbc_define_method(vm, mrbc_class_string, "chomp",	c_string_chomp);
  mrbc_define_method(vm, mrbc_class_string, "chomp!",	c_string_chomp_self);
  mrbc_define_method(vm, mrbc_class_string, "dup",	c_string_dup);
  mrbc_define_method(vm, mrbc_class_string, "index",	c_string_index);
  mrbc_define_method(vm, mrbc_class_string, "ord",	c_string_ord);
  mrbc_define_method(vm, mrbc_class_string, "lstrip",	c_string_lstrip);
  mrbc_define_method(vm, mrbc_class_string, "lstrip!",	c_string_lstrip_self);
  mrbc_define_method(vm, mrbc_class_string, "rstrip",	c_string_rstrip);
  mrbc_define_method(vm, mrbc_class_string, "rstrip!",	c_string_rstrip_self);
  mrbc_define_method(vm, mrbc_class_string, "strip",	c_string_strip);
  mrbc_define_method(vm, mrbc_class_string, "strip!",	c_string_strip_self);
  mrbc_define_method(vm, mrbc_class_string, "to_sym",	c_string_to_sym);
  mrbc_define_method(vm, mrbc_class_string, "intern",	c_string_to_sym);

#if MRBC_USE_FLOAT
  mrbc_define_method(vm, mrbc_class_string, "to_f",	c_string_to_f);
#endif

  mrbc_define_method(vm, mrbc_class_object, "sprintf",	c_object_sprintf);
}


#endif // MRBC_USE_STRING
