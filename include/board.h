/****************************************************************************
 * boards/arm/rp23xx/picocalc/include/board.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_BOARD_H
#define __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "rp23xx_i2cdev.h"
#include "rp23xx_i2sdev.h"
#include "rp23xx_spidev.h"
#include "rp23xx_spisd.h"
#include "picocalc_kbd.h"
#include "picocalc_lcd.h"


#ifndef __ASSEMBLY__
#include <stdint.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

#define MHZ 1000000

#define BOARD_XOSC_FREQ (12 * MHZ)
#define BOARD_XOSC_STARTUPDELAY 1
#define BOARD_PLL_SYS_FREQ (150 * MHZ)
#define BOARD_PLL_USB_FREQ (48 * MHZ)

#define BOARD_REF_FREQ (12 * MHZ)
#define BOARD_SYS_FREQ (150 * MHZ)
#define BOARD_PERI_FREQ (150 * MHZ)
#define BOARD_USB_FREQ (48 * MHZ)
#define BOARD_ADC_FREQ (48 * MHZ)
#define BOARD_HSTX_FREQ (150 * MHZ)

#define BOARD_UART_BASEFREQ BOARD_PERI_FREQ

#define BOARD_TICK_CLOCK (1 * MHZ)

/* definitions for pico-sdk */

/* GPIO definitions *********************************************************/

#define BOARD_GPIO_LED_PIN 25
#define BOARD_NGPIOOUT 1
#define BOARD_NGPIOIN 1
#define BOARD_NGPIOINT 1

/* LED definitions **********************************************************/

/* If CONFIG_ARCH_LEDS is not defined, then the user can control the LEDs
 * in any way. The following definitions are used to access individual LEDs.
 */

/* LED index values for use with board_userled() */

#define BOARD_LED1 0
#define BOARD_NLEDS 1

#define BOARD_LED_GREEN BOARD_LED1

/* LED bits for use with board_userled_all() */

#define BOARD_LED1_BIT (1 << BOARD_LED1)

/* This LED is not used by the board port unless CONFIG_ARCH_LEDS is
 * defined.  In that case, the usage by the board port is defined in
 * include/board.h and src/rp23xx_autoleds.c. The LED is used to encode
 * OS-related events as follows:
 *
 *   -------------------- ----------------------------- ------
 *   SYMBOL                   Meaning                   LED
 *   -------------------- ----------------------------- ------
 */

#define LED_STARTED 0      /* NuttX has been started  OFF    */
#define LED_HEAPALLOCATE 0 /* Heap has been allocated OFF    */
#define LED_IRQSENABLED 0  /* Interrupts enabled      OFF    */
#define LED_STACKCREATED 1 /* Idle stack created      ON     */
#define LED_INIRQ 2        /* In an interrupt         N/C    */
#define LED_SIGNAL 3       /* In a signal handler     ON    */
#define LED_ASSERTION 4    /* An assertion failed     ON    */
#define LED_PANIC 5        /* The system has crashed  FLASH  */
#define LED_IDLE  6           /* Not used                       */

/* Thus if the LED is statically on, NuttX has successfully  booted and is,
 * apparently, running normally.  If the LED is flashing at approximately
 * 2Hz, then a fatal error has been detected and the system has halted.
 */

/* BUTTON definitions *******************************************************/

#define NUM_BUTTONS 0

#define BUTTON_USER1 0
#define BUTTON_USER2 1
#define BUTTON_USER1_BIT (1 << BUTTON_USER1)
#define BUTTON_USER2_BIT (1 << BUTTON_USER2)

/****************************************************************************
 * PicoCalc definitions
 ****************************************************************************/

#define DISPLAY_RST 15
#define DISPLAY_DC 14
#define DISPLAY_BCKL -1
#define DISPLAY_SPI 1
#define DISPLAY_SPI_FREQ (75 * MHZ)

#define PICOCALC_KBD_I2C_BUS 1
#define PICOCALC_KBD_I2C_ADDR 0x1f
#define PICOCALC_KBD_I2C_FIFO_CMD 0x09
#define PICOCALC_KBD_I2C_FREQ 10 * 1000 // 10Khz
#define PICOCALC_KBD_DEVICE "/dev/kbd"
#define PICOCALC_KBD_POLL_INTERVAL_MSEC 30

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

  /****************************************************************************
   * Public Function Prototypes
   ****************************************************************************/

  /****************************************************************************
   * Name: rp23xx_boardearlyinitialize
   *
   * Description:
   *
   ****************************************************************************/

  void rp23xx_boardearlyinitialize(void);

  /****************************************************************************
   * Name: rp23xx_boardinitialize
   *
   * Description:
   *
   ****************************************************************************/

  void rp23xx_boardinitialize(void);

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_BOARD_H */
