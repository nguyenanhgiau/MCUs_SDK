/**
  ******************************************************************************
  * @file:      test_app.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      5/31/2018
  * @brief:     Implementation test app
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
 
/* Includes ------------------------------------------------------------------*/
#include "test_app.h"
#include "console.h"
 
/* TEST_BME280 */
#include "bme280.h"

/* TEST_VL53L0X */
#include "Adafruit_VL53L0X.h"
#include "g_flash.h"

/* TEST_LED7_SEG */
#include "LED7_SEGMENTS.h"
#include "g_spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
 
void test_bme280(void)
{
  struct bme280_dev dev;
  
  dev.dev_id = BME280_I2C_ADDR_PRIM;
  dev.intf = BME280_I2C_INTF;
  dev.read = user_i2c_read;
  dev.write = user_i2c_write;
  dev.delay_ms = user_delay_ms;
  
  if (bme280_init(&dev) == BME280_OK){
    CONSOLE_Print("BME280 Init OK\n");
  }
  else{
    CONSOLE_Print("BME280 Init Fail\n");
  }
  
  uint8_t settings_sel;
  struct bme280_data comp_data;
  
  /* Recommended mode of operation: Indoor navigation */
  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  dev.settings.filter = BME280_FILTER_COEFF_16;
  dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;
  
  settings_sel = BME280_OSR_PRESS_SEL;
  settings_sel |= BME280_OSR_TEMP_SEL;
  settings_sel |= BME280_OSR_HUM_SEL;
  settings_sel |= BME280_STANDBY_SEL;
  settings_sel |= BME280_FILTER_SEL;
  bme280_set_sensor_settings(settings_sel, &dev);
  bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
  
  CONSOLE_Print("Temperature, Pressure, Humidity\r\n");
  while (1) {
    /* Delay while the sensor completes a measurement */
    dev.delay_ms(500);
    bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
    print_sensor_data(&comp_data);
  }
}


void test_vl53l0x(void)
{ 
  Adafruit_VL53L0X_begin();
  
  VL53L0X_RangingMeasurementData_t measure;
//  uint32_t isCalibration = 0;
//  static int32_t offset = 0;
//  uint8_t count = 0;
//  
//  isCalibration = *(__IO uint32_t *)FLASH_ADDR_CHECK_CALIB;
//  /* check calibration done? */
//  if(isCalibration == CALIBRATION_DONE){
//    offset = *(__IO int32_t *)(FLASH_ADDR_CHECK_CALIB + 4);
//  }
//  else{
//    /* calculate offset value if not calibration */
//    while(count < 10){
//      count++;
//      Adafruit_VL53L0X_getSingleRangingMeasurement(&measure);
//      offset += 100 - measure.RangeMilliMeter;
//    }
//    /* average of 10 times */
//    offset = offset/10;
//
//    /* handle write offset_value to flash */
//    
//  }
//  
//  CONSOLE_Print("Offset Value: %d\n", offset);
  
  while(1){
    Adafruit_VL53L0X_getSingleRangingMeasurement(&measure);
    CONSOLE_Print("Raw Data: %d\n", measure.RangeMilliMeter);
//    CONSOLE_Print("After Calculate Offset: %d\n", measure.RangeMilliMeter + offset);
    
    g_timebase_delay(500);
  }
}

void test_led7_segments(void)
{
  static uint8_t codeLED7[] = {0xC0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
  static uint8_t display[] = {0x92, 0x0F};
  uint8_t count = 0;
  
  while(1){
    display[0] = codeLED7[count++];
    count = (count > 9 )? 0 : count;
    g_spi_write_read(display, 2);
    g_timebase_delay(1000);
  }
}

void test_case(void)
{
#ifdef TEST_VL53L0X
  test_vl53l0x();
#endif
  
#ifdef TEST_BME280
  test_bme280();
#endif
  
#ifdef TEST_FLASH
  is_calibration_vl53l0x();
//  test_flash_program();
#endif
  
#ifdef TEST_LED7_SEG
  test_led7_segments();
#endif
}
