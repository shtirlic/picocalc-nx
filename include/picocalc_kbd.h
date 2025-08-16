/****************************************************************************
 * boards/risc-v/rp23xx-rv/picocalc/include/picocalc_kbd.h
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

#ifndef __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_PICOCALC_KBD_H
#define __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_PICOCALC_KBD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/irq.h>
#include <stdbool.h>

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
   * Name: board_picocalc_kbd_initialize
   *
   * Description:
   *   Initialize picocalc kbd driver and register the /dev/kbd device.
   *
   ****************************************************************************/

#ifdef CONFIG_INPUT_PICOCALC_KBD
  int board_picocalc_kbd_initialize(void);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_RP23XX_PICOCALC_INCLUDE_PICOCALC_KBD_H */
