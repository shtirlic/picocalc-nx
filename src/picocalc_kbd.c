/****************************************************************************
 * picocalc/src/picocalc_kbd.c
 *
 * SPDX-License-Identifier: Apache-2.0
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <nuttx/kthread.h>
#include <nuttx/sched.h>
#include <nuttx/clock.h>
#include <nuttx/signal.h>
#include <nuttx/fs/fs.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/irq.h>
#include <nuttx/input/kbd_codec.h>
#include <nuttx/input/keyboard.h>
#include <nuttx/wqueue.h>

#include <arch/board/board.h>
#include "picocalc_kbd.h"
#include "rp23xx_i2c.h"

static struct picocalc_kbd_dev_s g_picocalc_kbd;

static int i2c_kbd_read(uint16_t *outval)
{
  uint8_t cmd = PICOCALC_KBD_I2C_FIFO_CMD;
  uint8_t buf[2];
  int     ret;

  struct i2c_config_s config;
  config.frequency = PICOCALC_KBD_I2C_FREQ;
  config.address   = PICOCALC_KBD_I2C_ADDR;
  config.addrlen   = 7;

  g_picocalc_kbd.i2c_status = 0;
  ret = i2c_writeread(g_picocalc_kbd.i2c, &config, &cmd, sizeof(cmd), buf,
                      sizeof(buf));

  if (ret < 0)
    {
      _err("i2c_kbd_read: Write/Read transfer failed: %d (%d)\n", ret,
           errno);
      return ret;
    }
  g_picocalc_kbd.i2c_status = 1;

  *outval = ((uint16_t)buf[1] << 8) | buf[0];

  return 0;
}

static int picocalc_kbd_read(void)
{
  static int ctrlheld = 0;
  uint16_t   buff     = 0;
  int        c        = -1;
  int        ret;

  ret = i2c_kbd_read(&buff);
  if (ret < 0)
    return ret;

  if (buff)
    {
      /*  _info("Raw keycode: 0x%04x\n", buff); */

      if (buff == KBD_CTRL_RELEASED)
        {
          ctrlheld = 0;
        }
      else if (buff == KBD_CTRL_HOLD)
        {
          ctrlheld = 1;
        }
      else if (KEY_EVENT_PRESS(buff) || KEY_EVENT_RELEASE(buff))
        {
          int kc = KEY_EVENT_CODE(buff);
          if (kc >= 'a' && kc <= 'z' && ctrlheld)
            c = kc - 'a' + 1;
          else
            c = kc;

          if (c > 0 && c <= 255)
            {
#if 0
              _info("Char: %d %s\n", c,
                    KEY_EVENT_PRESS(buff) ? "press" : "release");
#endif
              keyboard_event(
                  &g_picocalc_kbd.lower, keyboard_translate_picocalc_code(c),
                  KEY_EVENT_PRESS(buff) ? KEYBOARD_PRESS : KEYBOARD_RELEASE);
            }
        }
    }

  return 0;
}

static int picocalc_kbd_poll_worker(int argc, char *argv[])
{
  int ret;

  g_picocalc_kbd.opened = true;

  while (g_picocalc_kbd.thread_running)
    {
      if (g_picocalc_kbd.opened)
        {
          ret = picocalc_kbd_read();
          if (ret < 0)
            {
              _err("Failed to picocalc_kbd_read: %d\n", ret);
            }
        }
      else
        {
          _err("g_picocalc_kbd.opened: %d\n", g_picocalc_kbd.opened);
        }
      usleep(PICOCALC_KBD_POLL_INTERVAL_MSEC * USEC_PER_MSEC);
    }

  /*  _info("Polling thread stopped\n");  */

  return 0;
}

static int picocalc_kbd_open(FAR struct keyboard_lowerhalf_s *lower)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR void *)lower;
  int                            ret;

  if (priv->opened)
    return 0;

  priv->thread_running = true;
  ret = kthread_create("picocalc_kbd_poll", PICOCALC_KBD_POLL_PRIORITY, 4096,
                       picocalc_kbd_poll_worker, NULL);

  if (ret < 0)
    {
      _err("Failed to create polling thread: %d\n", ret);
      priv->opened         = false;
      priv->thread_running = false;
      return ret;
    }
  priv->kthread = ret;

  /*   _info("picocalc_kbd polling thread started\n"); */

  return 0;
}

static int picocalc_kbd_close(FAR struct keyboard_lowerhalf_s *lower)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR void *)lower;

  priv->opened = false;

  priv->thread_running = false;
  kthread_delete(priv->kthread);

  _info("picocalc_kbd closed\n");

  return 0;
}

static ssize_t picocalc_kbd_write(FAR struct keyboard_lowerhalf_s *lower,
                                  FAR const char *buffer, size_t buflen)
{
  /* Not typically used; return -ENOSYS unless needed for testing */

  return -ENOSYS;
}

int board_picocalc_kbd_initialize(void)
{
  int                        ret;
  struct picocalc_kbd_dev_s *priv = &g_picocalc_kbd;

  memset(priv, 0, sizeof(*priv));

  priv->lower.open  = picocalc_kbd_open;
  priv->lower.close = picocalc_kbd_close;
  priv->lower.write = picocalc_kbd_write;
  priv->lower.priv  = priv;
  priv->i2c         = rp23xx_i2cbus_initialize(1);
  if (!priv->i2c)
    {
      _err("Failed to initialize I2C bus\n");
      return -ENODEV;
    }

  ret = keyboard_register(&priv->lower, PICOCALC_KBD_DEVICE,
                          CONFIG_INPUT_PICOCALC_KBD_BUFFSIZE);
  if (ret < 0)
    {
      _err("Failed to register /dev/kbd: %d\n", ret);
      return ret;
    }

  _info("picocalc_kbd initialized, /dev/kbd registered\n");
  return 0;
}
