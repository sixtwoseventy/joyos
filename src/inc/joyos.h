#ifndef _JOYOS_H_
#define _JOYOS_H_

#include <stdint.h>
#include <analog.h>
#include <board.h>
#include <digital.h>
#include <buttons.h>
#include <config.h>
#include <encoder.h>
#include <gyro.h>
#include <lcd.h>
#include <motor.h>
#include <servo.h>
#include <rf.h>

#include <nrf24l01.h>

#include <hal/adc.h>
#include <hal/delay.h>
#include <hal/io.h>
#include <hal/i2c.h>
#include <hal/uart.h>
#include <hal/spi.h>

#include <kern/global.h>
#include <kern/isr.h>
#include <kern/lock.h>
#include <kern/thread.h>

#ifndef SIMULATE
#include <socket.h>
#endif

#endif
