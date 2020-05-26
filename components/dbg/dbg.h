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

#ifndef DBG_H_
#define DBG_H_

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "chip_selection.h"

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define _USE_XFUNC_OUT	        1	/* 1: Use output functions */
#define	_CR_CRLF	            1	/* 1: Convert \n ==> \r\n in the output char */
#define	_USE_LONGLONG	        1	/* 1: Enable long long integer in type "ll". */
#define	_LONGLONG_t	            long long	/* Platform dependent long long integer type */
   
#define _USE_XFUNC_IN	        1	/* 1: Use input function */
#define	_LINE_ECHO	            1	/* 1: Echo back input chars in xgets function */

#define NOTSET              (0)
#define DEBUG               (10)
#define INFO                (20)
#define WARN                (30)
#define ERROR               (40)
#define CRITICAL            (50)

#ifndef DBG_vPrintf
#define DBG_vPrintf(STREAM, FOMART, ARGS...)    \
        do {                                    \
            if (STREAM)                         \
                xprintf((FOMART), ## ARGS);     \
        } while (0);
#endif

#ifndef DBG_LEVEL_LOG
#define DBG_LEVEL_LOG       (INFO)
#endif

#ifndef DBG_vLog
#define DBG_vLog(LEVEL, FOMART, ARGS...)            \
        do {                                        \
            if (LEVEL <= DBG_LEVEL_LOG) {           \
                switch (LEVEL) {                    \
                case 10:                            \
                    xprintf("[DEBUG] ");            \
                    break;                          \
                case 20:                            \
                    xprintf("[INFO] ");             \
                    break;                          \
                case 30:                            \
                    xprintf("[WARN] ");             \
                    break;                          \
                case 40:                            \
                    xprintf("[ERROR] ");            \
                    break;                          \
                case 50:                            \
                    xprintf("[CRIT] ");             \
                    break;                          \
                default:                            \
                    break;                          \
                }                                   \
                if (LEVEL != 0)                     \
                    xprintf((FOMART), ## ARGS);     \
                    xprintf("\n");                  \
            }                                       \
        } while (0);
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum
{
    E_DBG_OK,
    E_DBG_FAIL
}DBG_teStatus;

typedef void (*DBG_tpfOpen)(void);
typedef void (*DBG_tpfWrite)(unsigned char);
typedef unsigned char (*DBG_tpfRead)(void);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
#if _USE_XFUNC_OUT
void xputc (char c);
void xputs (const char *str);
void xfputs (void (*func)(unsigned char), const char* str);
void xprintf (const char *fmt, ...);
void xsprintf (char* buff, const char *fmt, ...);
void xfprintf (void (*func)(unsigned char), const char *fmt, ...);
void put_dump (const void *buff, unsigned long addr, int len, int width);
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif /* _USE_XFUNC_OUT */

#if _USE_XFUNC_IN
int xgets (char *buff, int len);
int xfgets (unsigned char (*func)(void), char *buff, int len);
int xatoi (char **str, long *res);
#endif

DBG_teStatus DBG_vInit(DBG_tpfOpen pfOpen, DBG_tpfWrite pfWrite, DBG_tpfRead pfRead);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /*DBG_H_*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
