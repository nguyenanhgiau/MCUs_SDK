/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2017 Google, Inc
 */

#ifndef _WDT_H_
#define _WDT_H_

#include "chip_selection.h"
#include "prj_options.h"

/*
 * Implement a simple watchdog uclass. Watchdog is basically a timer that
 * is used to detect or recover from malfunction. During normal operation
 * the watchdog would be regularly reset to prevent it from timing out.
 * If, due to a hardware fault or program error, the computer fails to reset
 * the watchdog, the timer will elapse and generate a timeout signal.
 * The timeout signal is used to initiate corrective action or actions,
 * which typically include placing the system in a safe, known state.
 */

/*
 * Start the timer
 *
 * @timeout_ms: Number of ticks (milliseconds) before timer expires
 * @flags: Driver specific flags. This might be used to specify
 * which action needs to be executed when the timer expires
 * @return: 0 if OK, -ve on error
 */
int wdt_start(unsigned long timeout_ms, unsigned long flags);

/*
 * Stop the timer, thus disabling the Watchdog. Use wdt_start to start it again.
 *
 * @return: 0 if OK, -ve on error
 */
int wdt_stop(void);

/*
 * Reset the timer, typically restoring the counter to
 * the value configured by start()
 *
 * @dev: WDT Device
 * @return: 0 if OK, -ve on error
 */
int wdt_reset(void);

/*
 * Expire the timer, thus executing its action immediately.
 * This is typically used to reset the board or peripherals.
 *
 * @flags: Driver specific flags
 * @return 0 if OK -ve on error. If wdt action is system reset,
 * this function may never return.
 */
int wdt_expire_now(unsigned long flags);

/*
 * struct wdt_ops - Driver model wdt operations
 *
 * The uclass interface is implemented by all wdt devices which use
 * driver model.
 */
struct wdt_ops {
	/*
	 * Start the timer
	 *
	 * @timeout_ms: Number of ticks (milliseconds) before the timer expires
	 * @flags: Driver specific flags. This might be used to specify
	 * which action needs to be executed when the timer expires
	 * @return: 0 if OK, -ve on error
	 */
	int (*start)(unsigned long timeout_ms, unsigned long flags);
	/*
	 * Stop the timer
	 *
	 * @return: 0 if OK, -ve on error
	 */
	int (*stop)(void);
	/*
	 * Reset the timer, typically restoring the counter to
	 * the value configured by start()
	 *
	 * @return: 0 if OK, -ve on error
	 */
	int (*reset)(void);
	/*
	 * Expire the timer, thus executing the action immediately (optional)
	 *
	 * If this function is not provided, a default implementation
	 * will be used, which sets the counter to 1
	 * and waits forever. This is good enough for system level
	 * reset, where the function is not expected to return, but might not be
	 * good enough for other use cases.
	 *
	 * @flags: Driver specific flags
	 * @return 0 if OK -ve on error. May not return.
	 */
	int (*expire_now)(unsigned long flags);
};

int init_watchdog(void);

#endif /*_WDT_H_*/
