/**
  ******************************************************************************
  * Copyright of anhgiau (nguyenanhgiau1008@gmail.com)
  * This coding style used at AT&T and Sun in writing C programs.
  * visit: http://www.csd.uoc.gr/~zakkak/hy255/reading/c_coding_style.pdf
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  * - Redistributions of source code must retain the above copyright
  * notice, this list of conditions and the following disclaimer.
  * - Redistributions in binary form must reproduce the above copyright
  * notice, this list of conditions and the following disclaimer in the
  * documentation and/or other materials provided with the distribution.
  * - Development or contribute must follow this coding style.
  *
  * @file:      flash.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      6/6/2018
  * @brief:     Implementation of Flash Driver
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
 #include "g_flash.h"
 
/* Private Typedef -----------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Private Variables Declarations --------------------------------------------*/
/* Private Function Definitions ----------------------------------------------*/
/* Function Definitions ------------------------------------------------------*/
void test_flash_program(void)
{
}

/**
* @brief Function check calibration for VL53L0X
*
* @return 0 if was calibrated, otherwise if yet
*/
int is_calibration_vl53l0x(void)
{
  return 0;
}

int flash_write_word(uint32_t address, uint32_t data)
{
  return 0;
}