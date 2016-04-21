#ifndef I2C_H_
#define I2C_H_

#include <inttypes.h>

/** defines the data direction (reading from I2C device) in i2cStart(),i2cRepStart() */
#define I2C_READ    1
/** defines the data direction (writing to I2C device) in i2cStart(),i2cRepStart() */
#define I2C_WRITE   0

#ifdef __cplusplus
extern "C" {
#endif

extern void i2cInit(unsigned long clockSpeedHz);
extern void i2cStartWait(unsigned char address);
extern unsigned char i2cStart(unsigned char address);
extern void i2cStartAddr(unsigned char address, unsigned int pos);
extern void i2cStop(void);
extern void i2cStartBit(void);
extern void i2cCompleted (void);
extern void i2cTxFinished(void);
extern void i2cWriting(uint8_t data);
extern unsigned char i2cWrite(unsigned char data);
extern unsigned char i2cReadAck(void);
extern unsigned char i2cReadNak(void);

#ifdef __cplusplus
}
#endif

#endif
