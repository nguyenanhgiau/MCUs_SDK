#ifndef __SERIAL_H__
#define __SERIAL_H__

/**
 * struct struct dm_serial_ops - Driver model serial operations
 *
 * The uclass interface is implemented by all serial devices which use
 * driver model.
 */
struct serial_ops {
	/**
	 * setbrg() - Set up the baud rate generator
	 *
	 * Adjust baud rate divisors to set up a new baud rate for this
	 * device. Not all devices will support all rates. If the rate
	 * cannot be supported, the driver is free to select the nearest
	 * available rate. or return -EINVAL if this is not possible.
	 *
	 * @baudrate: New baud rate to use
	 * @return 0 if OK, -ve on error
	 */
	int (*setbrg)(int baudrate);
	/**
	 * getc() - Read a character and return it
	 *
	 * If no character is available, this should return -EAGAIN without
	 * waiting.
	 *
	 * @return character (0..255), -ve on error
	 */
	int (*getc)(void);
	/**
	 * putc() - Write a character
	 *
	 * @ch: character to write
	 * @return 0 if OK, -ve on error
	 */
	int (*putc)(const char ch);
	/**
	 * pending() - Check if input/output characters are waiting
	 *
	 * This can be used to return an indication of the number of waiting
	 * characters if the driver knows this (e.g. by looking at the FIFO
	 * level). It is acceptable to return 1 if an indeterminant number
	 * of characters is waiting.
	 *
	 * This method is optional.
	 *
	 * @input: true to check input characters, false for output
	 * @return number of waiting characters, 0 for none, -ve on error
	 */
	int (*pending)(bool input);
	/**
	 * clear() - Clear the serial FIFOs/holding registers
	 *
	 * This method is optional.
	 *
	 * This quickly clears any input/output characters from the UART.
	 * If this is not possible, but characters still exist, then it
	 * is acceptable to return -EAGAIN (try again) or -EINVAL (not
	 * supported).
	 *
	 * @return 0 if OK, -ve on error
	 */
	int (*clear)(void);

	/**
	 * getconfig() - Get the uart configuration
	 * (parity, 5/6/7/8 bits word length, stop bits)
	 *
	 * Get a current config for this device.
	 *
	 * @serial_config: Returns config information (see SERIAL_... above)
	 * @return 0 if OK, -ve on error
	 */
	int (*getconfig)(uint16 *serial_config);
	/**
	 * setconfig() - Set up the uart configuration
	 * (parity, 5/6/7/8 bits word length, stop bits)
	 *
	 * Set up a new config for this device.
	 *
	 * @serial_config: number of bits, parity and number of stopbits to use
	 * @return 0 if OK, -ve on error
	 */
	int (*setconfig)(uint serial_config);
	/**
	 * getinfo() - Get serial device information
	 *
	 * @info: struct serial_device_info to fill
	 * @return 0 if OK, -ve on error
	 */
	int (*getinfo)(struct serial_device_info *info);
};

/**
 * serial_getconfig() - Get the uart configuration
 * (parity, 5/6/7/8 bits word length, stop bits)
 *
 * Get a current config for this device.
 *
 * @serial_id: serial index
 * @serial_config: Returns config information (see SERIAL_... above)
 * @return 0 if OK, -ve on error
 */
int serial_getconfig(unsigned int serial_id, uint *config);

/**
 * serial_setconfig() - Set up the uart configuration
 * (parity, 5/6/7/8 bits word length, stop bits)
 *
 * Set up a new config for this device.
 *
 * @serial_id: serial index
 * @serial_config: number of bits, parity and number of stopbits to use
 * @return 0 if OK, -ve on error
 */
int serial_setconfig(unsigned int serial_id, uint config);

int serial_init(void);
void serial_setbrg(unsigned int serial_id, int baudrate);
void serial_putc(unsigned int serial_id, const char ch);
void serial_putc_raw(unsigned int serial_id, const char ch);
void serial_puts(unsigned int serial_id, const char *str);
int serial_getc(unsigned int serial_id);

#endif /*__SERIAL_H__*/
