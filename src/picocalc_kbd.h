/****************************************************************************
 * boards/arm/rp23xx/picocalc/src/picocalc_kbd.h
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

#ifndef __BOARDS_ARM_RP23XX_PICOCALC_SRC_PICOCALC_KBD_H
#define __BOARDS_ARM_RP23XX_PICOCALC_SRC_PICOCALC_KBD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#define KEY_EVENT_PRESS(code) (((code) & 0xff) == 1)
#define KEY_EVENT_RELEASE(code) (((code) & 0xff) == 3)
#define KEY_EVENT_CODE(code) ((code) >> 8)

#define KBD_CTRL_PRESSED 0x7e02
#define KBD_CTRL_RELEASED 0x7e03

enum key_state
{
  KEY_STATE_IDLE = 0,
  KEY_STATE_PRESSED,
  KEY_STATE_HOLD,
  KEY_STATE_RELEASED,
};

#define KEY_JOY_UP 0x01
#define KEY_JOY_DOWN 0x02
#define KEY_JOY_LEFT 0x03
#define KEY_JOY_RIGHT 0x04
#define KEY_JOY_CENTER 0x05
#define KEY_BTN_LEFT1 0x06
#define KEY_BTN_RIGHT1 0x07

#define KEY_BACKSPACE 0x08
#define KEY_TAB 0x09
#define KEY_ENTER 0x0A
// 0x0D - CARRIAGE RETURN
#define KEY_BTN_LEFT2 0x11
#define KEY_BTN_RIGHT2 0x12

#define KEY_MOD_ALT 0xA1
#define KEY_MOD_SHL 0xA2
#define KEY_MOD_SHR 0xA3
#define KEY_MOD_SYM 0xA4
#define KEY_MOD_CTRL 0xA5

#define KEY_ESC 0xB1
#define KEY_UP 0xb5
#define KEY_DOWN 0xb6
#define KEY_LEFT 0xb4
#define KEY_RIGHT 0xb7

#define KEY_BREAK 0xd0 // == KEY_PAUSE
#define KEY_INSERT 0xD1
#define KEY_HOME 0xD2
#define KEY_DEL 0xD4
#define KEY_END 0xD5
#define KEY_PAGE_UP 0xd6
#define KEY_PAGE_DOWN 0xd7

#define KEY_CAPS_LOCK 0xC1

#define KEY_F1 0x81
#define KEY_F2 0x82
#define KEY_F3 0x83
#define KEY_F4 0x84
#define KEY_F5 0x85
#define KEY_F6 0x86
#define KEY_F7 0x87
#define KEY_F8 0x88
#define KEY_F9 0x89
#define KEY_F10 0x90

static uint32_t keyboard_translate_picocalc_code(uint16_t keycode)
{
  switch (keycode)
    {
    case KEY_BREAK:
      return KEYCODE_BREAK;
    case KEY_DEL:
      return KEYCODE_FWDDEL;
    case KEY_BACKSPACE:
      return KEYCODE_BACKDEL;
    case KEY_HOME:
      return KEYCODE_HOME;
    case KEY_END:
      return KEYCODE_END;
    case KEY_LEFT:
      return KEYCODE_LEFT;
    case KEY_RIGHT:
      return KEYCODE_RIGHT;
    case KEY_UP:
      return KEYCODE_UP;
    case KEY_DOWN:
      return KEYCODE_DOWN;
    case KEY_ENTER:
      return KEYCODE_ENTER;
    case KEY_CAPS_LOCK:
      return KEYCODE_CAPSLOCK;
    case KEY_F1:
      return KEYCODE_F1;
    case KEY_F2:
      return KEYCODE_F2;
    case KEY_F3:
      return KEYCODE_F3;
    case KEY_F4:
      return KEYCODE_F4;
    case KEY_F5:
      return KEYCODE_F5;
    case KEY_F6:
      return KEYCODE_F6;
    case KEY_F7:
      return KEYCODE_F7;
    case KEY_F8:
      return KEYCODE_F8;
    case KEY_F9:
      return KEYCODE_F9;
    case KEY_F10:
      return KEYCODE_F10;
    default:
      return keycode;
    }
}

#endif /* __BOARDS_ARM_RP23XX_PICOCALC_SRC_PICOCALC_KBD_H */
