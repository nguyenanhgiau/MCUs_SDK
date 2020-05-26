/*------------------------------------------------------------------------/
/  Universal String Handler for Console Input and Output
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2012, ChaN, all right reserved.
/
/ xprintf module is an open source software. Redistribution and use of
/ xprintf module in source and binary forms, with or without modification,
/ are permitted provided that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "dbg.h"
#include <string.h>

/* Private Typedef -----------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Private Variables Declarations --------------------------------------------*/
/* Private Function Definitions ----------------------------------------------*/

#if _USE_XFUNC_OUT
#include <stdarg.h>
typedef struct
{
    DBG_tpfOpen     pfOpen;
    DBG_tpfWrite    pfWrite;
    DBG_tpfRead     pfRead;
}DBG_tsCommon;

static DBG_tsCommon    DBG_sCommon;
static char *outptr;

/*
********************************************************************************
*               PUT A CHARACTER
* @brief:  This function send a character
* @param:
*       - c: is a character need to send
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xputc (char c)
{
  if (_CR_CRLF && c == '\n') xputc('\r');		/* CR -> CRLF */
  
  if (outptr) {		/* Destination is memory */
    *outptr++ = (unsigned char)c;
    return;
  }
  if (DBG_sCommon.pfWrite) {	/* Destination is device */
    DBG_sCommon.pfWrite((unsigned char)c);
  }
}


/*
********************************************************************************
*               PUT A STRING
* @brief:  This function send a string
* @param:
*       - str: is a pointer to string
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xputs ( const char *str)
{
  while (*str) {
    xputc(*str++);
  }
}


/*
********************************************************************************
*               PUT A STRING TO SPECIFIED DEVICE
* @brief:  This function send a string to the specified device
* @param:
*       - func: is a pointer to the output function
*       - str: is a pointer to the string
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xfputs ( void(*func)(unsigned char), const char *str)
{
  void (*pf)(unsigned char);
  
  
  pf = DBG_sCommon.pfWrite;		/* Save current output device */
  DBG_sCommon.pfWrite = func;	        /* Switch output to specified device */
  while (*str) {		/* Put the string */
    xputc(*str++);
  }
  DBG_sCommon.pfWrite = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
xprintf("%-6u", 100);			"100   "
xprintf("%ld", 12345678);		"12345678"
xprintf("%llu", 0x100000000);	"4294967296"	<_USE_LONGLONG>
xprintf("%04x", 0xA3);			"00a3"
xprintf("%08lX", 0x123ABC);		"00123ABC"
xprintf("%016b", 0x550F);		"0101010100001111"
xprintf("%s", "String");		"String"
xprintf("%-5s", "abc");			"abc  "
xprintf("%5s", "abc");			"  abc"
xprintf("%c", 'a');				"a"
xprintf("%f", 10.0);            <xprintf lacks floating point support. Use regular printf.>
*/

static
void xvprintf (	const char *fmt, va_list arp)
{
  unsigned int r, i, j, w, f;
  char s[24], c, d, *p;
#if _USE_LONGLONG
  _LONGLONG_t v;
  unsigned _LONGLONG_t vs;
#else
  long v;
  unsigned long vs;
#endif
  
  
  for (;;) {
    c = *fmt++;					/* Get a format character & increment pointer */
    if (!c) break;				/* End of format? */
    if (c != '%') {				/* Pass it through if not a % sequense */
      xputc(c); continue;
    }
    f = 0;					/* Clear flags */
    c = *fmt++;					/* Get first char of the sequense */
    if (c == '0') {				/* Flag: left '0' padded */
      f = 1; c = *fmt++;
    } else {
      if (c == '-') {			        /* Flag: left justified */
        f = 2; c = *fmt++;
      }
    }
    for (w = 0; c >= '0' && c <= '9'; c = *fmt++) {	/* Minimum width */
      w = w * 10 + c - '0';
    }
    if (c == 'l' || c == 'L') {	                /* Prefix: Size is long */
      f |= 4; c = *fmt++;
#if _USE_LONGLONG
      if (c == 'l' || c == 'L') {	        /* Prefix: Size is long long */
        f |= 8; c = *fmt++;
      }
#endif
    }
    if (!c) break;				/* End of format? */
    d = c;
    if (d >= 'a') d -= 0x20;
    switch (d) {				/* Type is... */
    case 'S' :					/* String */
      p = va_arg(arp, char*);
      for (j = 0; p[j]; j++) ;
      while (!(f & 2) && j++ < w) xputc(' ');
      xputs(p);
      while (j++ < w) xputc(' ');
      continue;
    case 'C' :					/* Character */
      xputc((char)va_arg(arp, int)); continue;
    case 'B' :					/* Binary */
      r = 2; break;
    case 'O' :					/* Octal */
      r = 8; break;
    case 'D' :					/* Signed decimal */
    case 'U' :					/* Unsigned decimal */
      r = 10; break;
    case 'X' :					/* Hexdecimal */
      r = 16; break;
    case 'f':
      xputc((char)va_arg(arp, double)); continue;
    default:					/* Unknown type (passthrough) */
      xputc(c); continue;
    }
    
    /* Get an argument and put it in numeral */
#if _USE_LONGLONG
    if (f & 8) {	/* long long argument? */
      v = va_arg(arp, _LONGLONG_t);
    } else {
      if (f & 4) {	/* long argument? */
        v = (d == 'D') ? (long)va_arg(arp, long) : (long)va_arg(arp, unsigned long);
      } else {		/* int/short/char argument */
        v = (d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int);
      }
    }
#else
    if (f & 4) {	/* long argument? */
      v = va_arg(arp, long);
    } else {		/* int/short/char argument */
      v = (d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int);
    }
#endif
    if (d == 'D' && v < 0) {	/* Negative value? */
      v = 0 - v; f |= 16;
    }
    i = 0; vs = v;
    do {
      d = (char)(vs % r); vs /= r;
      if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
      s[i++] = d + '0';
    } while (vs != 0 && i < sizeof s);
    if (f & 16) s[i++] = '-';
    j = i; d = (f & 1) ? '0' : ' ';
    while (!(f & 2) && j++ < w) xputc(d);
    do xputc(s[--i]); while (i != 0);
    while (j++ < w) xputc(' ');
  }
}

/*
********************************************************************************
*               PUT A FORMATTED STRING TO THE DEFAULT DEVICE
* @brief:  This function will out a formatted string to default device
* @param:
*       - fm: is string format
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xprintf ( const char *fmt,	...)
{
  va_list arp;
  
  
  va_start(arp, fmt);
  xvprintf(fmt, arp);
  va_end(arp);
}

/*
********************************************************************************
*               PUT A FORMATTED STRING TO THE BUFFER
* @brief:  This function will out a formatted string to the buffer
* @param:
*       - buf: is a pointer to output buffer
*       - fm: is string format
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xsprintf (	char* buff, const char*	fmt, ...)
{
  va_list arp;
  
  
  outptr = buff;		/* Switch destination for memory */
  
  va_start(arp, fmt);
  xvprintf(fmt, arp);
  va_end(arp);
  
  *outptr = 0;		        /* Terminate output string with a \0 */
  outptr = 0;			/* Switch destination for device */
}

/*
********************************************************************************
*               PUT A FORMATTED STRING TO THE SPECIFIED DEVICE
* @brief:  This function will out a formatted string to specified device
* @param:
*       - func: is a pointer to the output function
*       - fm: is string format
* @retval: none
* @NOTE: 
********************************************************************************
*/
void xfprintf ( void(*func)(unsigned char), const char*	fmt, ...)
{
  va_list arp;
  void (*pf)(unsigned char);
  
  
  pf = DBG_sCommon.pfWrite;		/* Save current output device */
  DBG_sCommon.pfWrite = func;	        /* Switch output to specified device */
  
  va_start(arp, fmt);
  xvprintf(fmt, arp);
  va_end(arp);
  
  DBG_sCommon.pfWrite = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void put_dump (
               const void* buff,		/* Pointer to the array to be dumped */
               unsigned long addr,		/* Heading address value */
               int len,				/* Number of items to be dumped */
               int width			/* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
                 )
{
  int i;
  const unsigned char *bp;
  const unsigned short *sp;
  const unsigned long *lp;
  
  
  xprintf("%08lX ", addr);		/* address */
  
  switch (width) {
  case DW_CHAR:
    bp = buff;
    for (i = 0; i < len; i++)		/* Hexdecimal dump */
      xprintf(" %02X", bp[i]);
    xputc(' ');
    for (i = 0; i < len; i++)		/* ASCII dump */
      xputc((unsigned char)((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.'));
    break;
  case DW_SHORT:
    sp = buff;
    do					/* Hexdecimal dump */
      xprintf(" %04X", *sp++);
    while (--len);
    break;
  case DW_LONG:
    lp = buff;
    do					/* Hexdecimal dump */
      xprintf(" %08LX", *lp++);
    while (--len);
    break;
  }
  
  xputc('\n');
}

#endif /* _USE_XFUNC_OUT */

#if _USE_XFUNC_IN

/*
********************************************************************************
*               GET A LINE FROM THE INPUT
* @brief:  This function is called to get a line from the input
* @param:
*       - buff: pointer to the buffer
*       - len: buffer length
* @retval: 0 if end of stream, 1:A line arrived
* @NOTE: 
********************************************************************************
*/
int xgets ( char* buff, int len )
{
  int c, i;
  
  
  if (!DBG_sCommon.pfRead) return 0;		/* No input function specified */
  
  i = 0;
  for (;;) {
    c = DBG_sCommon.pfRead();			/* Get a char from the incoming stream */
    if (!c) return 0;		        /* End of stream? */
    if (c == '\r') break;		/* End of line? */
    if (c == '\b' && i) {		/* Back space? */
      i--;
      if (_LINE_ECHO) xputc((unsigned char)c);
      continue;
    }
    if (c >= ' ' && i < len - 1) {	/* Visible chars */
      buff[i++] = c;
      if (_LINE_ECHO) xputc((unsigned char)c);
    }
  }
  buff[i] = 0;	                        /* Terminate with a \0 */
  if (_LINE_ECHO) xputc('\n');
  return 1;
}

/*
********************************************************************************
*               GET A LINE FROM THE SPECIFIED INPUT
* @brief:  This function is called to get a line from the specified input
* @param:
*       - func: is a pointer to the inout stream function
*       - buff: pointer to the buffer
*       - len: buffer length
* @retval: 0 if end of stream, 1:A line arrived
* @NOTE: 
********************************************************************************
*/
int xfgets ( unsigned char (*func)(void), char* buff, int len)
{
  unsigned char (*pf)(void);
  int n;
  
  
  pf = DBG_sCommon.pfRead;		/* Save current input device */
  DBG_sCommon.pfRead = func;		/* Switch input to specified device */
  n = xgets(buff, len);	        /* Get a line */
  DBG_sCommon.pfRead = pf;		/* Restore input device */
  
  return n;
}

/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
          ^                           1st call returns 123 and next ptr
              ^                       2nd call returns -5 and next ptr
                    ^                 3rd call returns 1023 and next ptr
                          ^           4th call returns 15 and next ptr
                                ^     5th call returns 255 and next ptr
                                    ^ 6th call fails and returns 0
*/

int xatoi (			/* 0:Failed, 1:Successful */
           char **str,		/* Pointer to pointer to the string */
           long *res		/* Pointer to the valiable to store the value */
             )
{
  unsigned long val;
  unsigned char c, r, s = 0;
  
  
  *res = 0;
  
  while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */
  
  if (c == '-') {		/* negative? */
    s = 1;
    c = *(++(*str));
  }
  
  if (c == '0') {
    c = *(++(*str));
    switch (c) {
    case 'x':		/* hexdecimal */
      r = 16; c = *(++(*str));
      break;
    case 'b':		/* binary */
      r = 2; c = *(++(*str));
      break;
    default:
      if (c <= ' ') return 1;	/* single zero */
      if (c < '0' || c > '9') return 0;	/* invalid char */
      r = 8;		/* octal */
    }
  } else {
    if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
    r = 10;			/* decimal */
  }
  
  val = 0;
  while (c > ' ') {
    if (c >= 'a') c -= 0x20;
    c -= '0';
    if (c >= 17) {
      c -= 7;
      if (c <= 9) return 0;	/* invalid char */
    }
    if (c >= r) return 0;		/* invalid char for current radix */
    val = val * r + c;
    c = *(++(*str));
  }
  if (s) val = 0 - val;			/* apply sign if needed */
  
  *res = val;
  return 1;
}
#endif /* _USE_XFUNC_IN */

DBG_teStatus DBG_vInit(DBG_tpfOpen pfOpen, DBG_tpfWrite pfWrite, DBG_tpfRead pfRead)
{
    if (pfOpen != NULL || pfWrite != NULL || pfRead != NULL)
    {
        return E_DBG_FAIL;
    }

    DBG_sCommon.pfOpen = pfOpen;
    DBG_sCommon.pfWrite = pfWrite;
    DBG_sCommon.pfRead = pfRead;

    /* initialize hardware debugger */
    DBG_sCommon.pfOpen();

    return E_DBG_OK;
}
/* Function Definitions ------------------------------------------------------*/
