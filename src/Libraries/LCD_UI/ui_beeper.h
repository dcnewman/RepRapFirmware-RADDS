#ifndef UI_BEEPER_H__
#define UI_BEEPER_H__

#if BEEPER_TYPE == 2 && defined(UI_HAS_I2C_KEYS) && UI_I2C_KEY_ADDRESS != BEEPER_ADDRESS
#error Beeper address and i2c key address must be identical
#elif BEEPER_TYPE == 2
#define UI_I2C_KEY_ADDRESS BEEPER_ADDRESS
#endif

static void beep(uint8_t duration, uint8_t count)
{
#if FEATURE_BEEPER && BEEPER_TYPE != 0
#if BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0
    SET_OUTPUT(BEEPER_PIN);
#endif // BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0
#if BEEPER_TYPE == 2
    i2cStartWait(BEEPER_ADDRESS + I2C_WRITE);
#if UI_DISPLAY_I2C_CHIPTYPE == 1
    i2cWrite(0x14); // Start at port a
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
#endif // BEEPER_TYPE == 2
    for (uint8_t i = 0; i < count; i++)
    {
#if BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0
#if defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
        WRITE(BEEPER_PIN,LOW);
#else
        WRITE(BEEPER_PIN,HIGH);
#endif // defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
#else // !(BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0)
#if UI_DISPLAY_I2C_CHIPTYPE == 0
#if BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
        i2cWrite(outputMask & ~BEEPER_PIN);
#else
        i2cWrite(~BEEPER_PIN);
#endif // BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
#endif // UI_DISPLAY_I2C_CHIPTYPE == 0
#if UI_DISPLAY_I2C_CHIPTYPE == 1
        i2cWrite((BEEPER_PIN) | outputMask);
        i2cWrite(((BEEPER_PIN) | outputMask) >> 8);
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
#endif // BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0
        delay(duration);
#if BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0
#if defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
        WRITE(BEEPER_PIN,HIGH);
#else
        WRITE(BEEPER_PIN,LOW);
#endif // defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
#else // !(BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0)
#if UI_DISPLAY_I2C_CHIPTYPE == 0
#if BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
        i2cWrite((BEEPER_PIN) | outputMask);
#else
        i2cWrite(255);
#endif // BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
#endif // UI_DISPLAY_I2C_CHIPTYPE == 0
#if UI_DISPLAY_I2C_CHIPTYPE == 1
        i2cWrite(outputMask);
        i2cWrite(outputMask >> 8);
#endif // UI_DISPLAY_I2C_CHIPTYPE == 1
#endif // BEEPER_TYPE == 1 && defined(BEEPER_PIN) && BEEPER_PIN >= 0

		delay(duration);
    }
#if BEEPER_TYPE == 2
    i2cStop();
#endif // BEEPER_TYPE == 2
#endif // FEATURE_BEEPER && BEEPER_TYPE != 0
}

#endif
