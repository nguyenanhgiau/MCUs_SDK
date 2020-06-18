/*
********************************************************************************
* Copyright of anhgiau (nguyenanhgiau1008@gmail.com)
* Follow this coding style used at:
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
* @file:      rtc.h
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of RTC Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RTC_H_
#define RTC_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "prj_options.h"
#include "rtc_def.h"

struct rtc_ops{
    /**
	 * get() - get the current time
	 *
	 * Returns the current time read from the RTC device. The driver
	 * is responsible for setting up every field in the structure.
	 *
	 * @time:	Place to put the time that is read
	 */
	int (*get)(struct rtc_time *time);

    /**
	 * set() - set the current time
	 *
	 * Sets the time in the RTC device. The driver can expect every
	 * field to be set correctly.
	 *
	 * @time:	Time to write
	 */
	int (*set)(const struct rtc_time *time);

    /**
	 * reset() - reset the RTC to a known-good state
	 *
	 * This function resets the RTC to a known-good state. The time may
	 * be unset by this method, so should be set after this method is
	 * called.
	 *
	 * @return 0 if OK, -ve on error
	 */
	int (*reset)(void);
};

/**
 * rtc_get() - Read the time from an RTC
 *
 * @time:	Place to put the current time
 * @return 0 if OK, -ve on error
 */
int rtc_get (struct rtc_time *);

/**
 * rtc_set() - Write a time to an RTC
 *
 * @time:	Time to write into the RTC
 * @return 0 if OK, -ve on error
 */
int rtc_set (struct rtc_time *);

/**
 * rtc_reset() - reset the RTC to a known-good state
 *
 * If the RTC appears to be broken (e.g. it is not counting up in seconds)
 * it may need to be reset to a known good state. This function achieves this.
 * After resetting the RTC the time should then be set to a known value by
 * the caller.
 *
 * @return 0 if OK, -ve on error
 */
int rtc_reset (void);

/**
 * rtc_init() - Set up the real time clock ready for use
 */
void rtc_init(void);

/**
 * is_leap_year - Check if year is a leap year
 *
 * @year	Year
 * @return	1 if leap year
 */
static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

/**
 * rtc_calc_weekday() - Work out the weekday from a time
 *
 * This only works for the Gregorian calendar - i.e. after 1752 (in the UK).
 * It sets time->tm_wdaay to the correct day of the week.
 *
 * @time:	Time to inspect. tm_wday is updated
 * @return 0 if OK, -EINVAL if the weekday could not be determined
 */
int rtc_calc_weekday(struct rtc_time *time);

/**
 * rtc_to_tm() - Convert a time_t value into a broken-out time
 *
 * The following fields are set up by this function:
 *	tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday
 *
 * Note that tm_yday and tm_isdst are set to 0.
 *
 * @time_t:	Number of seconds since 1970-01-01 00:00:00
 * @time:	Place to put the broken-out time
 */
void rtc_to_tm(unsigned long time_t, struct rtc_time *time);

/**
 * rtc_mktime() - Convert a broken-out time into a time_t value
 *
 * The following fields need to be valid for this function to work:
 *	tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year
 *
 * Note that tm_wday and tm_yday are ignored.
 *
 * @time:	Broken-out time to convert
 * @return corresponding time_t value, seconds since 1970-01-01 00:00:00
 */
unsigned long rtc_mktime(struct rtc_time *time);

/**
 * rtc_month_days() - The number of days in the month
 *
 * @month:	month (January = 0)
 * @year:	year (4 digits)
 */
int rtc_month_days(unsigned int month, unsigned int year);

#ifdef __cplusplus
}
#endif
#endif /*RTC_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
