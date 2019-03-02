/*! @file
  @brief
  console output module. (not yet input)

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_CONSOLE_H_
#define MRBC_SRC_CONSOLE_H_

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "hal/hal.h"

#ifdef __cplusplus
extern "C" {
#endif


//================================================================
/*! printf tiny (mruby/c) version data container.
*/
typedef struct RPrintf {
  char *buf;		//!< output buffer.
  const char *buf_end;	//!< output buffer end point.
  char *p;		//!< output buffer write point.
  const char *fstr;	//!< format string. (e.g. "%d %03x")

  struct RPrintfFormat {
    char type;				//!< format char. (e.g. 'd','f','x'...)
    unsigned int flag_plus : 1;
    unsigned int flag_minus : 1;
    unsigned int flag_space : 1;
    unsigned int flag_zero : 1;
    int width;				//!< display width. (e.g. %10d as 10)
    int precision;			//!< precision (e.g. %5.2f as 2)
  } fmt;
} mrb_printf;



void console_printf(const char *fstr, ...);
int mrbc_printf_main(mrb_printf *pf);
int mrbc_printf_char(mrb_printf *pf, int ch);
int mrbc_printf_str(mrb_printf *pf, const char *str, int pad);
int mrbc_printf_int(mrb_printf *pf, int32_t value, int base);
int mrbc_printf_float( mrb_printf *pf, double value );
void mrbc_printf_replace_buffer(mrb_printf *pf, char *buf, int size);


//================================================================
/*! output a character

  @param  c	character
*/
static inline void console_putchar(char c)
{
  hal_write(1, &c, 1);
}


//================================================================
/*! output string

  @param str	str
*/
static inline void console_print(const char *str)
{
  hal_write(1, str, strlen(str));
}


//================================================================
/*! initialize data container.

  @param  pf	pointer to mrb_printf
  @param  buf	pointer to output buffer.
  @param  size	buffer size.
  @param  fstr	format string.
*/
static inline void mrbc_printf_init( mrb_printf *pf, char *buf, int size,
				     const char *fstr )
{
  pf->p = pf->buf = buf;
  pf->buf_end = buf + size - 1;
  pf->fstr = fstr;
  int i=0;
  for(i=0;i<sizeof(pf->fmt);i++){
    *(((char*)(&pf->fmt))+i)=0;
  }
}


//================================================================
/*! clear output buffer in container.

  @param  pf	pointer to mrb_printf
*/
static inline void mrbc_printf_clear( mrb_printf *pf )
{
  pf->p = pf->buf;
}


//================================================================
/*! terminate ('\0') output buffer.

  @param  pf	pointer to mrb_printf
*/
static inline void mrbc_printf_end( mrb_printf *pf )
{
  *pf->p = '\0';
}


//================================================================
/*! return string length in buffer

  @param  pf	pointer to mrb_printf
  @return	length
*/
static inline int mrbc_printf_len( mrb_printf *pf )
{
  return pf->p - pf->buf;
}

#ifdef __cplusplus
}
#endif
#endif
