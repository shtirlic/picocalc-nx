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
#include <nuttx/signal.h>
#include <nuttx/fs/fs.h>
#include <nuttx/i2c/i2c_master.h>
#include <nuttx/input/kbd_codec.h>
#include <nuttx/input/keyboard.h>

#include <arch/board/board.h>
#include "picocalc_kbd.h"

static int i2c_kbd_read(FAR struct picocalc_kbd_dev_s *priv, uint16_t *outval)
{
  uint8_t          cmd = PICOCALC_KBD_I2C_FIFO_CMD;
  uint8_t          buf[2];
  int              ret = OK;
  struct i2c_config_s config;

  nxmutex_lock(&priv->dev_lock);

  config.frequency = PICOCALC_KBD_I2C_FREQ;
  config.address   = PICOCALC_KBD_I2C_ADDR;
  config.addrlen   = 7;

  // g_picocalc_kbd.i2c_status = 0;
  ret = i2c_writeread(priv->i2c, &config, &cmd, sizeof(cmd), buf,
                      sizeof(buf));
  if (ret < 0)
    {
      _err("i2c_kbd_read: Write/Read transfer failed: %d (%d)\n", ret, errno);
      goto err;
    }
  // g_picocalc_kbd.i2c_status = 1;

  *outval = ((uint16_t)buf[1] << 8) | buf[0];
  nxmutex_unlock(&priv->dev_lock);

err:
  return ret;
}

static int picocalc_kbd_get_key(FAR struct picocalc_kbd_dev_s *priv)
{
  static int ctrlheld = 0;
  uint16_t   buff     = 0;
  int        c        = -1;
  int        ret;

  ret = i2c_kbd_read(priv,&buff);

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

              keyboard_event(&priv->lower,
                keyboard_translate_picocalc_code(c),
                KEY_EVENT_PRESS(buff) ? KEYBOARD_PRESS : KEYBOARD_RELEASE);
            }
        }
    }

  return 0;
}

static int picocalc_kbd_thread(int argc, char **argv)
{
  FAR struct picocalc_kbd_dev_s *priv = (FAR struct picocalc_kbd_dev_s *)
        ((uintptr_t)strtoul(argv[1], NULL, 16));
  int ret = OK;

  while (true)
    {
      ret = picocalc_kbd_get_key(priv);
      if (ret < 0)
        {
          _err("Failed to picocalc_kbd_get_key: %d\n", ret);
          return ret;
        }
      nxsig_usleep(PICOCALC_KBD_POLL_INTERVAL_MSEC * USEC_PER_MSEC);
    }

  return ret;
}

int board_picocalc_kbd_initialize(FAR struct i2c_master_s *i2c)
{
  FAR struct keyboard_lowerhalf_s  *lower;
  FAR struct picocalc_kbd_dev_s    *priv;
  FAR char *argv[2];
  char arg1[32];
  int ret = OK;

  /* Initialize the PicoCalc keyboard device structure */

  priv = kmm_zalloc(sizeof(struct picocalc_kbd_dev_s));
  if (priv == NULL)
    {
      _err("ERROR: Failed to allocate instance (err = %d)\n", ret);
      return -ENOMEM;
    }

  priv->i2c = i2c;
  nxmutex_init(&priv->dev_lock);

  lower = &priv->lower;
  lower->open  = NULL;
  lower->close = NULL;
  lower->write = NULL;
  lower->priv  = priv;

  ret = keyboard_register(lower, PICOCALC_KBD_DEVICE,
                          CONFIG_INPUT_PICOCALC_KBD_BUFFSIZE);
  if (ret < 0)
    {
      _err("Failed to register keyboard driver  (err = %d)\n", ret);
      goto err_init;
    }

  /* Create thread for polling keyboard data */

  snprintf(arg1, 16, "%p", priv);
  argv[0] = arg1;
  argv[1] = NULL;
  ret = kthread_create("picocalc_kbd_thread",
                            SCHED_PRIORITY_DEFAULT,4096,
                       picocalc_kbd_thread, argv);
  if (ret < 0)
    {
      _err("ERROR: Failed to create poll thread (err = %d)\n", ret);
      goto err_register;
    }

  _info("picocalc_kbd driver driver loaded successfully!\n");
  return OK;

err_register:
  keyboard_unregister(lower, PICOCALC_KBD_DEVICE);
err_init:
  nxmutex_destroy(&priv->dev_lock);
  kmm_free(priv);
  return ret;
}
