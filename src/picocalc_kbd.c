/****************************************************************************
 *
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
#include <sys/types.h>

#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <nuttx/clock.h>
#include <nuttx/fs/fs.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/input/kbd_codec.h>
#include <nuttx/input/keyboard.h>
#include <nuttx/wqueue.h>

#include "picocalc_kbd.h"

/* Configurable */

#define I2C_KBD_DEV "/dev/i2c1"
#define I2C_KBD_ADDR 0x1f
#define KBD_DEVICE "/dev/kbd"
#define KBD_POLL_INTERVAL_MSEC 10
#define KBD_BUFFER_SIZE 64

struct picocalc_kbd_dev_s
{
  struct keyboard_lowerhalf_s lower; /* Must be first */
  bool opened;
  struct work_s work;
};

static struct picocalc_kbd_dev_s g_picocalc_kbd;

static int i2c_kbd_transfer(int fd, FAR struct i2c_msg_s *msgv, int msgc)
{
  struct i2c_transfer_s xfer;
  xfer.msgv = msgv;
  xfer.msgc = msgc;
  return ioctl(fd, I2CIOC_TRANSFER, (unsigned long)((uintptr_t)&xfer));
}

static int i2c_kbd_read(struct picocalc_kbd_dev_s *priv, uint16_t *outval)
{
  struct i2c_msg_s msgs[2];
  uint8_t cmd = 0x09;
  uint8_t buf[2];
  int ret, fd;

  fd = open(I2C_KBD_DEV, O_RDONLY);
  if (fd < 0)
    {
      _err("Failed to open I2C device %s\n", I2C_KBD_DEV);
      return -1;
    }
  // _info("Opened I2C device: fd=%d\n", fd);

  // Write message: send command 0x09
  msgs[0].frequency = 100000;
  msgs[0].addr = I2C_KBD_ADDR;
  msgs[0].flags = I2C_M_NOSTOP;
  msgs[0].buffer = &cmd;
  msgs[0].length = 1;

  // Read message: read 2 bytes into buf
  msgs[1].frequency = 100000;
  msgs[1].addr = I2C_KBD_ADDR;
  msgs[1].flags = I2C_M_READ;
  msgs[1].buffer = buf;
  msgs[1].length = 2;

  // Send write part (command)
  ret = i2c_kbd_transfer(fd, &msgs[0], 1);
  if (ret < 0)
    {
      _err("i2c_kbd_read: Write transfer failed: %d (%d)\n", ret, errno);
      close(fd);
      return ret;
    }

  usleep(16000); // Wait for device to prepare data

  // Read response
  ret = i2c_kbd_transfer(fd, &msgs[1], 1);
  close(fd);
  if (ret < 0)
    {
      _err("i2c_kbd_read: Read transfer failed: %d (%d)\n", ret, errno);
      return ret;
    }

  // Assemble 16-bit result from two 8-bit values (little-endian)
  *outval = ((uint16_t)buf[1] << 8) | buf[0];

  return 0;
}

static int picocalc_kbd_read(void *arg)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR struct picocalc_kbd_dev_s *)arg;

  static int ctrlheld = 0;
  uint16_t buff = 0;
  int c = -1;

  if (i2c_kbd_read(priv, &buff) < 0) return -1;

  if (buff)
    {
      // _info("Raw keycode: 0x%04x\n", buff);
      if (buff == KBD_CTRL_RELEASED)
        {
          ctrlheld = 0;
        }
      else if (buff == KBD_CTRL_PRESSED)
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
              // _info("Char: %d %s\n", c,
              // KEY_EVENT_PRESS(buff) ? "press" : "release");

              keyboard_event(
                  &priv->lower, keyboard_translate_picocalc_code(c),
                  KEY_EVENT_PRESS(buff) ? KEYBOARD_PRESS : KEYBOARD_RELEASE);
            }
        }
    }

  return 0;
}

static void picocalc_kbd_poll_func(FAR void *arg)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR struct picocalc_kbd_dev_s *)arg;

  if (!priv->opened) return;

  int ret = picocalc_kbd_read(priv);
  if (ret < 0)
    {
      _err("Failed to picocalc_kbd_read: %d\n", ret);
    }

  ret = work_queue(HPWORK, &priv->work, picocalc_kbd_poll_func, priv,
                   MSEC2TICK(KBD_POLL_INTERVAL_MSEC));

  if (ret < 0)
    {
      _err("Failed to queue work: %d\n", ret);
    }
}

static int picocalc_kbd_open(FAR struct keyboard_lowerhalf_s *lower)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR void *)lower;

  if (priv->opened) return 0;

  priv->opened = true;

  /* Start polling loop when device is opened */
  work_queue(HPWORK, &priv->work, picocalc_kbd_poll_func, priv,
             MSEC2TICK(KBD_POLL_INTERVAL_MSEC));
  _info("\npicocalc_kbd polling started\n");

  return 0;
}

static int picocalc_kbd_close(FAR struct keyboard_lowerhalf_s *lower)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR void *)lower;

  priv->opened = false;

  work_cancel(HPWORK, &priv->work);

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
  int ret;
  struct picocalc_kbd_dev_s *priv = &g_picocalc_kbd;

  memset(priv, 0, sizeof(*priv));

  priv->lower.open = picocalc_kbd_open;
  priv->lower.close = picocalc_kbd_close;
  priv->lower.write = picocalc_kbd_write;
  priv->lower.priv = priv;

  ret = keyboard_register(&priv->lower, KBD_DEVICE,
                          CONFIG_INPUT_PICOCALC_KBD_BUFFSIZE);
  if (ret < 0)
    {
      _err("Failed to register /dev/kbd: %d\n", ret);
      return ret;
    }

  _info("\npicocalc_kbd initialized, /dev/kbd registered\n");
  return 0;
}
