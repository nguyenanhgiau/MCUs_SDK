/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2009 Sergey Kubushyn <ksi@koi8.net>
 * Copyright (C) 2009 - 2013 Heiko Schocher <hs@denx.de>
 * Changes for multibus/multiadapter I2C support.
 *
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 *
 * The original I2C interface was
 *   (C) 2000 by Paolo Scaffardi (arsenio@tin.it)
 *   AIRVENT SAM s.p.a - RIMINI(ITALY)
 * but has been changed substantially.
 */

#ifndef _I2C_H_
#define _I2C_H_

/*
 * For now there are essentially two parts to this file - driver model
 * here at the top, and the older code below (with CONFIG_SYS_I2C being
 * most recent). The plan is to migrate everything to driver model.
 * The driver model structures and API are separate as they are different
 * enough as to be incompatible for compilation purposes.
 */

void i2c_init(void);

/*
 * Probe the given I2C chip address.  Returns 0 if a chip responded,
 * not 0 on failure.
 */
int i2c_probe(uint8_t chip);

/*
 * Read/Write interface:
 *   chip:    I2C chip address, range 0..127
 *   addr:    Memory (register) address within the chip
 *   alen:    Number of bytes to use for addr (typically 1, 2 for larger
 *              memories, 0 for register type devices with only one
 *              register)
 *   buffer:  Where to read/write the data
 *   len:     How many bytes to read/write
 *
 *   Returns: 0 on success, not 0 on failure
 */
int i2c_read(uint8_t chip, unsigned int addr, int alen,
				uint8_t *buffer, int len);

int i2c_write(uint8_t chip, unsigned int addr, int alen,
				uint8_t *buffer, int len);

/*
 * Utility routines to read/write registers.
 */
uint8_t i2c_reg_read(uint8_t addr, uint8_t reg);

void i2c_reg_write(uint8_t addr, uint8_t reg, uint8_t val);

#endif /*_I2C_H_*/
