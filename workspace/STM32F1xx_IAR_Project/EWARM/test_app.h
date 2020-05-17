/**
  ******************************************************************************
  * @file:      test_app.h
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      5/31/2018
  * @brief:     Header file for test application
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WE SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
  * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEST_APP_H
#define __TEST_APP_H
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Uncomment to enable, comment to disable */
//#define TEST_BME280   /* temp, humi, press sensor */
#define TEST_VL53L0X    /* laser sensor */
//#define TEST_FLASH      /* flash program */
//#define TEST_LED7_SEG   /**/
/* Exported functions --------------------------------------------------------*/
void test_bme280(void);
   
void test_vl53l0x(void);

void test_led7_segments(void);

void test_case(void);
#ifdef __cplusplus
}
#endif
#endif /*__TEST_APP_H*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
