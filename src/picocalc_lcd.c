/****************************************************************************
 * boards/xtensa/esp32/common/src/esp32_ili9341.c
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
 #include <inttypes.h>
 #include <stdbool.h>
 #include <errno.h>
 #include <sys/param.h>
 #include <debug.h>

 #include <nuttx/arch.h>
 #include <nuttx/irq.h>
 #include <nuttx/lcd/ili9341.h>
 #include <nuttx/lcd/ili9488.h>
 #include <nuttx/lcd/lcd.h>
 #include <nuttx/spi/spi.h>

 #if defined(CONFIG_VIDEO_FB) && defined(CONFIG_LCD_FRAMEBUFFER)
 #  include <nuttx/video/fb.h>
 #endif

 #include <nuttx/video/rgbcolors.h>
 #include <arch/board/board.h>


 #include "rp23xx_gpio.h"
 #include "rp23xx_spi.h"

 #include "picocalc_lcd.h"

 /****************************************************************************
  * Preprocessor Definitions
  ****************************************************************************/

 /****************************************************************************
  * Private Type Definition
  ****************************************************************************/
 struct picocalc_lcd_config_data
 {
   uint8_t cmd;
   uint8_t data_bytes;
   uint8_t data[16];
 };

 struct picocalc_lcd_s
 {
   struct ili9341_lcd_s dev;
   struct spi_dev_s *spi;
 };

 static const struct picocalc_lcd_config_data g_lcd_config[] =
 {
  // {
  //   0x01, 0,
  // },
  // {
  //   0xcf,3,
  //   {
  //     0x00,0xc1,0x30
  //   }
  // },
  // {
  //   0xed,4,
  //   {
  //     0x64,0x03,0x12,0x81
  //   }
  // },
  // {
  //   0xe8, 3,
  //   {
  //     0x85,0x00,0x78
  //   }
  // },
  // {
  //   0xcb, 5,
  //   {
  //     0x39,0x2C,0x00,0x34,0x02
  //   }
  // },
  // {
  //   0xf7, 1,
  //   {
  //     0x20
  //   }
  // },
  // {
  //   0xea, 2,
  //   {
  //     0x0,0x0
  //   }
  // },
  // {
  //   0xc0, 1,
  //   {
  //     0x23
  //   }
  // },
  // {
  //   0xc1, 1,
  //   {
  //     0x10
  //   }
  // },
  // {
  //   0xc5, 2,
  //   {
  //     0x3e,0x28
  //   }
  // },
  // {
  //   0xc7, 1,
  //   {
  //     0x86
  //   }
  // },
  // {
  //   0x37, 1,
  //   {
  //     0x00
  //   }
  // },
  // {
  //   0x3a, 1,
  //   {
  //     0x55
  //   }
  // },
  // {
  //   0xb1, 2,
  //   {
  //     0x00,0x18
  //   }
  // },
  // {
  //   0xb6, 2,
  //   {
  //     0x02,0x02
  //   }
  // },
  // {
  //   0x26, 1,
  //   {
  //     0x01
  //   }
  // },
  // {
  //   0xe0, 15,
  //   {
  //     0x0F,0x31,0x2B,0x0C,0x0E,0x08,
  //     0x4E,0xF1,0x37,0x07,0x10,0x03,
  //     0x0E,0x09,0x00
  //   }
  // },
  //  {
  //   0xe1, 15,
  //   {
  //     0x00,0x0E,0x14,0x03,0x11,0x07,
  //     0x31,0xC1,0x48,0x08,0x0F,0x0C,
  //     0x31,0x36,0x0F
  //   }
  // }
  // {
  //   ILI9488_CMD_MEMORY_ACCESS_CONTROL, 1, //0x36
  //   {
  //     0x48
  //   }
  // },
  // {
  //   ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET, 1, //0x3a
  //   {
  //     0x55
  //   }
  // },
  {
    ILI9488_CMD_DISPLAY_INVERSION_CONTROL, 1, //0xb4
     {
       0x0
     }
  },
  // {
  //   ILI9488_CMD_ENTRY_MODE_SET, 1, //0xb7
  //   {
  //     0xc6
  //   }
  // },
  // {
  //   ILI9488_CMD_BACKLIGHT_CONTROL_1, 2, //0xb9
  //   {
  //     0x02,0xe0
  //   }
  // },
  // {
  //  ILI9488_CMD_POWER_CONTROL_NORMAL_3, 1, //0xc2
  //  {
  //    0xa7
  //  }
  // },
  // {
  //   ILI9488_CMD_VCOM_CONTROL_1, 3, //0xc5
  //   {
  //     0x04
  //   }
  // },
  {
    ILI9488_CMD_POSITIVE_GAMMA_CORRECTION, 15, //0xe0
    {
      0x00, 0x03, 0x09, 0x08, 0x16, 0x0a, 0x3f,
      0x78, 0x4c, 0x09, 0x0a, 0x08, 0x16, 0x1a, 0x0f
    }
  },
  {
    ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION, 15, //0xe1
    {
      0x00, 0x16, 0x19, 0x03, 0x0f, 0x05, 0x32,
      0x45, 0x46, 0x04, 0x0e, 0x0d, 0x35, 0x37,0x0f
    }
   },
   {
    ILI9488_CMD_POWER_CONTROL_1, 2, //0xc0
    {
      0x17, 0x15
    }
   },
   {
    ILI9488_CMD_POWER_CONTROL_2, 1, //0xc1
    {
      0x41
    }
   },
  // {
  //   ILI9488_CMD_TEARING_EFFECT_LINE_ON, 1, //0x35
  //   {
  //     0x00
  //   }
  // }
  //  {
  //   ILI9488_CMD_COLUMN_ADDRESS_SET, 4,
  //   {
  //     0,0,(ILI9341_XRES >> 8), (ILI9341_XRES & 0xff)
  //   }
  //  },
  //  {
  //  ILI9488_CMD_PAGE_ADDRESS_SET, 4,
  //  {
  //    0,0,(ILI9341_YRES >> 8), (ILI9341_YRES & 0xff)
  //  }
  //  },
   {
    ILI9488_CMD_INTERFACE_MODE_CONTROL, 1, //0xb0
    {
      0x00
    }
   },
   {
     ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL, 1, //0xb1
     {
       0xa0
     }
   },
  //  {
  //    ILI9488_CMD_DISP_INVERSION_ON, 0, //0x21
  //  },

    {
    ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, 3, //0xb6
    {
      0x02, 0x02, 0x3b
    }
   },
  //  {
  //    ILI9488_CMD_ADJUST_CONTROL_3, 4, //0xf7
  //    {
  //      0xa9, 0x51, 0x2c, 0x82
  //    }
  //  },

  //  {
  //    /* Sleep out */

  //    ILI9341_SLEEP_OUT, 0
  //  },
  //  {
  //    /* Display on */

  //    ILI9341_DISPLAY_ON, 0
  //  },
  //  {
  //   ILI9341_MEMORY_ACCESS_CONTROL, 0
  //  },
  //  {
  //    ILI9341_MEMORY_ACCESS_CONTROL, 1,
  //    {
  //     ILI9341_MEMORY_ACCESS_CONTROL_MX | ILI9341_MEMORY_ACCESS_CONTROL_BGR
  //    }
  //  }
 };


 #define LAT                 4
 #define LATCLR              5
 #define LATSET              6
 #define LATINV              7


 void pin_set_bit(int pin, unsigned int offset) {
  switch (offset) {
      case LATCLR:
        rp23xx_gpio_set_pulls(pin, false, false);
        rp23xx_gpio_set_pulls(pin, false, true);
        rp23xx_gpio_put(pin, false);
          return;
      case LATSET:
        rp23xx_gpio_set_pulls(pin, false, false);
        rp23xx_gpio_set_pulls(pin, true, false);
        rp23xx_gpio_put(pin, true);
          return;
      default:
          break;
          //printf("Unknown pin_set_bit command");
  }
}

 /****************************************************************************
  * Private Function Protototypes
  ****************************************************************************/

 static void picocalc_lcd_select(struct ili9341_lcd_s *lcd);
 static void picocalc_lcd_deselect(struct ili9341_lcd_s *lcd);
 static int picocalc_lcd_backlight(struct ili9341_lcd_s *lcd,
                                       int level);
 static int picocalc_lcd_sendcmd(struct ili9341_lcd_s *lcd,
                                     const uint8_t cmd);
 static int picocalc_lcd_sendparam(struct ili9341_lcd_s *lcd,
                                       const uint8_t param);
 static int picocalc_lcd_sendgram(struct ili9341_lcd_s *lcd,
                                      const uint16_t *wd, uint32_t nwords);
 static int picocalc_lcd_recvparam(struct ili9341_lcd_s *lcd,
                                       uint8_t *param);
 static int picocalc_lcd_recvgram(struct ili9341_lcd_s *lcd,
                                      uint16_t *wd, uint32_t nwords);

 /****************************************************************************
  * Private Data
  ****************************************************************************/

 static struct picocalc_lcd_s g_lcddev;
 static struct lcd_dev_s *g_lcd = NULL;

 /****************************************************************************
  * Private Functions
  ****************************************************************************/

static void picocalc_lcd_configure(struct ili9341_lcd_s *lcd,
    uint8_t cmd,
    uint8_t data_bytes,
    const uint8_t *data)
{
lcd->select(lcd);
lcd->sendcmd(lcd, cmd);
if (data_bytes)
{
for (int i = 0; i < data_bytes; i++)
{
lcd->sendparam(lcd, data[i]);
}
}

lcd->deselect(lcd);
}

 /****************************************************************************
  * Name: picocalc_lcd_select
  *
  * Description:
  *   Select the SPI, lock and reconfigure if necessary
  *
  * Input Parameters:
  *   lcd  - Reference to the public driver structure
  *
  ****************************************************************************/

 static void picocalc_lcd_select(struct ili9341_lcd_s *lcd)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   SPI_LOCK(priv->spi, true);
   SPI_SELECT(priv->spi, SPIDEV_DISPLAY(0), true);
 }

 /****************************************************************************
  * Name: picocalc_lcd_deselect
  *
  * Description:
  *   De-select the SPI
  *
  * Input Parameters:
  *   lcd  - Reference to the public driver structure
  *
  ****************************************************************************/

 static void picocalc_lcd_deselect(struct ili9341_lcd_s *lcd)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   SPI_SELECT(priv->spi, SPIDEV_DISPLAY(0), false);
   SPI_LOCK(priv->spi, false);
 }

 /****************************************************************************
  * Name: picocalc_lcd_backlight
  *
  * Description:
  *   Set the backlight level of the connected display.
  *   NOTE: Currently this function either sets the brightness to the maximum
  *         level (level > 0) or turns the display off (level == 0). Although
  *         the ILI9341 chip provides an interface for configuring the
  *         backlight level via WRITE_DISPLAY_BRIGHTNESS (0x51), it depends on
  *         the actual circuit of the display device. Usually the backlight
  *         pins are hardwired to Vcc, making the backlight level setting
  *         effectless.
  *
  * Input Parameters:
  *   lcd   - Reference to the public driver structure
  *   level - Backlight level
  *
  * Returned Value:
  *   OK - On Success
  *
  ****************************************************************************/

 static int picocalc_lcd_backlight(struct ili9341_lcd_s *lcd,
                                       int level)
 {
   if (level > 0)
     {
       lcd->sendcmd(lcd, ILI9341_WRITE_CTRL_DISPLAY);
       lcd->sendparam(lcd, 0x24);
     }
   else
     {
       lcd->sendcmd(lcd, ILI9341_WRITE_CTRL_DISPLAY);
       lcd->sendparam(lcd, 0x0);
     }

   return OK;
 }

 /****************************************************************************
  * Name: picocalc_lcd_sendcmd
  *
  * Description:
  *   Send a command to the lcd driver.
  *
  * Input Parameters:
  *   lcd  - Reference to the ili9341_lcd_s driver structure
  *   cmd  - command to send
  *
  * Returned Value:
  *   On success - OK
  *
  ****************************************************************************/

 static int picocalc_lcd_sendcmd(struct ili9341_lcd_s *lcd,
                                     const uint8_t cmd)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   lcdinfo("%02x\n", cmd);

   SPI_SETBITS(priv->spi, 8);

   LCD_CMDDATA(priv->spi, SPIDEV_DISPLAY(0), true);
   SPI_SEND(priv->spi, cmd);
   LCD_CMDDATA(priv->spi, SPIDEV_DISPLAY(0), false);

   return OK;
 }

 /****************************************************************************
  * Name: picocalc_lcd_sendparam
  *
  * Description:
  *   Send a parameter to the lcd driver.
  *
  * Input Parameters:
  *   lcd    - Reference to the ili9341_lcd_s driver structure
  *   param  - Parameter to send
  *
  * Returned Value:
  *   OK - On Success
  *
  ****************************************************************************/

 static int picocalc_lcd_sendparam(struct ili9341_lcd_s *lcd,
                                       const uint8_t param)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   lcdinfo("param=%04x\n", param);

   SPI_SETBITS(priv->spi, 8);

   LCD_CMDDATA(priv->spi, SPIDEV_DISPLAY(0), false);
   SPI_SEND(priv->spi, param);

   return OK;
 }

 /****************************************************************************
  * Name: picocalc_lcd_sendgram
  *
  * Description:
  *   Send a number of pixel words to the lcd driver gram.
  *
  * Input Parameters:
  *   lcd    - Reference to the ili9341_lcd_s driver structure
  *   wd     - Reference to the words to send
  *   nwords - Number of words to send
  *
  * Returned Value:
  *   OK - On Success
  *
  ****************************************************************************/

static int picocalc_lcd_sendgram(struct ili9341_lcd_s *lcd,
    const uint16_t *wd, uint32_t nwords)
{
struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

lcdinfo("lcd:%p, wd=%p, nwords=%" PRIu32 "\n", lcd, wd, nwords);

SPI_SETBITS(priv->spi, 16);
SPI_SNDBLOCK(priv->spi, wd, nwords);

return OK;
}
// lcdinfo("lcd:%p, wd=%p, nwords=%" PRIu32 "\n", lcd, wd, nwords);

// SPI_SETBITS(priv->spi, 8);  // Set SPI word length to 8 bits (1 byte)

// for (uint32_t i = 0; i < nwords; ++i)
// {
//   uint16_t color = wd[i];  // Get the current pixel color
//   uint8_t q[3];  // Buffer to hold 3 bytes per pixel (RGB)

//   // Extract each component
//   uint8_t r5 = (color >> 11) & 0x1F;
//   uint8_t g6 = (color >> 5)  & 0x3F;
//   uint8_t b5 = color & 0x1F;

//   q[0] = (r5 << 3) | (r5 >> 2);  // Red
//   q[1] = (g6 << 2) | (g6 >> 4);  // Green
//   q[2] = (b5 << 3) | (b5 >> 2);  // Blue
//   // Send the RGB888 components (3 bytes per pixel)
//   SPI_SNDBLOCK(priv->spi, q, 3);  // Send the 3 bytes (RGB) at once
//  return OK;
// }


 /****************************************************************************
  * Name: picocalc_lcd_recvparam
  *
  * Description:
  *   Receive a parameter from the lcd driver.
  *
  * Input Parameters:
  *   lcd    - Reference to the ili9341_lcd_s driver structure
  *   param  - Reference to where parameter is received
  *
  * Returned Value:
  *   OK - On Success
  *
  ****************************************************************************/

 static int picocalc_lcd_recvparam(struct ili9341_lcd_s *lcd,
                                       uint8_t *param)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   SPI_SETBITS(priv->spi, 8);

   LCD_CMDDATA(priv->spi, SPIDEV_DISPLAY(0), false);

   *param = (uint8_t)(SPI_SEND(priv->spi, (uintptr_t)param) & 0xff);

   return OK;
 }

 /****************************************************************************
  * Name: picocalc_lcd_recvgram
  *
  * Description:
  *   Receive pixel words from the lcd driver gram.
  *
  * Input Parameters:
  *   lcd    - Reference to the public driver structure
  *   wd     - Reference to where the pixel words are received
  *   nwords - Number of pixel words to receive
  *
  * Returned Value:
  *   OK - On Success
  *
  ****************************************************************************/

 static int picocalc_lcd_recvgram(struct ili9341_lcd_s *lcd,
                                      uint16_t *wd, uint32_t nwords)
 {
   struct picocalc_lcd_s *priv = (struct picocalc_lcd_s *)lcd;

   lcdinfo("wd=%p, nwords=%" PRIu32 "\n", wd, nwords);

   SPI_SETBITS(priv->spi, 16);
   SPI_RECVBLOCK(priv->spi, wd, nwords);

   return OK;
 };

int lcd_spi_cmddata(struct spi_dev_s *dev, uint32_t devid, bool cmd)
 {
   if (devid == SPIDEV_DISPLAY(0))
     {
       /*  This is the Data/Command control pad which determines whether the
        *  data bits are data or a command.
        */

       rp23xx_gpio_put(DISPLAY_DC ,!cmd);

       return OK;
     }
     spiinfo("devid: %" PRIu32 " CMD: %s\n", devid, cmd ? "command" :
      "data");

return -ENODEV;
}

 /****************************************************************************
  * Public Functions
  ****************************************************************************/

 /****************************************************************************
  * Name: board_lcd_initialize
  *
  * Description:
  *   Initialize the LCD video hardware. The initial state of the LCD is fully
  *   initialized, display memory cleared, and the LCD ready to use, but with
  *   the power setting at 0 (full off).
  *
  ****************************************************************************/

 int board_lcd_initialize(void)
 {
   struct picocalc_lcd_s *priv = &g_lcddev;
   struct spi_dev_s *spi;
   lcdinfo("Initializing LCD\n");

   if (g_lcd == NULL)
     {
       spi = rp23xx_spibus_initialize(DISPLAY_SPI);
       if (!spi)
         {
           lcderr("Failed to initialize SPI bus.\n");
           return -ENODEV;
         }

       priv->spi = spi;

       /* Initialize non-SPI GPIOs */

       rp23xx_gpio_init(DISPLAY_DC);
       rp23xx_gpio_setdir(DISPLAY_DC, true);
      //  rp23xx_gpio_put(DISPLAY_DC, false);

       rp23xx_gpio_init(DISPLAY_RST);
       rp23xx_gpio_setdir(DISPLAY_RST, true);
       rp23xx_gpio_put(DISPLAY_RST, true);


       /* Reset ILI9341 */

       pin_set_bit(DISPLAY_RST, LATSET);
       up_mdelay(10);
       pin_set_bit(DISPLAY_RST, LATCLR);
       up_mdelay(10);
       pin_set_bit(DISPLAY_RST, LATSET);
       up_mdelay(200);


       /* Configure SPI */

       SPI_SETMODE(priv->spi, SPIDEV_MODE0);
       SPI_SETBITS(priv->spi, 8);
       SPI_HWFEATURES(priv->spi, 0);
       SPI_SETFREQUENCY(priv->spi, DISPLAY_SPI_FREQ);

       /* Initialize ILI9341 driver with necessary methods */

       priv->dev.select      = picocalc_lcd_select;
       priv->dev.deselect    = picocalc_lcd_deselect;
       priv->dev.sendcmd     = picocalc_lcd_sendcmd;
       priv->dev.sendparam   = picocalc_lcd_sendparam;
       priv->dev.recvparam   = picocalc_lcd_recvparam;
       priv->dev.sendgram    = picocalc_lcd_sendgram;
       priv->dev.recvgram    = picocalc_lcd_recvgram;
       priv->dev.backlight   = picocalc_lcd_backlight;

       g_lcd = ili9341_initialize(&priv->dev, 0);

       if (g_lcd != NULL)
         {
           /* Turn the LCD on at 100% power */


           for (int i = 0; i < nitems(g_lcd_config); i++)
             {
              picocalc_lcd_configure(&priv->dev,
                g_lcd_config[i].cmd,
                g_lcd_config[i].data_bytes,
                g_lcd_config[i].data);
              }

            priv->dev.select(&priv->dev);
            priv->dev.sendcmd(&priv->dev, ILI9488_CMD_DISP_INVERSION_ON);
            up_mdelay(50);
            // priv->dev.sendcmd(&priv->dev, ILI9488_CMD_SLEEP_OUT);
            // up_mdelay(120);
            // priv->dev.sendcmd(&priv->dev, ILI9488_CMD_DISPLAY_ON);
            // up_mdelay(120);

            // priv->dev.sendcmd(&priv->dev, ILI9488_CMD_MEMORY_ACCESS_CONTROL);
            // priv->dev.sendparam(&priv->dev, 0x40);
            priv->dev.deselect(&priv->dev);

            g_lcd->setpower(g_lcd, CONFIG_LCD_MAXPOWER);
            up_mdelay(50);

            ili9341_clear(g_lcd,RGBTO16(0x0,0xaa,0xff));

          #if defined(CONFIG_VIDEO_FB) && defined(CONFIG_LCD_FRAMEBUFFER)

          /* Initialize and register the simulated framebuffer driver */

          int ret = fb_register(0, 0);
          if (ret < 0)
            {
              syslog(LOG_ERR, "ERROR: fb_register() failed: %d\n", ret);
            }
          #endif
          }
     }

   return OK;
 }

 /****************************************************************************
  * Name: board_lcd_getdev
  *
  * Description:
  *   Return a reference to the LCD object for the specified LCD. This allows
  *   support for multiple LCD devices.
  *
  ****************************************************************************/

 struct lcd_dev_s *board_lcd_getdev(int lcddev)
 {
   if (lcddev == 0)
     {
       return g_lcd;
     }

   return NULL;
 }

 /****************************************************************************
  * Name: board_lcd_uninitialize
  *
  * Description:
  *   Uninitialize the LCD support.
  *
  ****************************************************************************/

 void board_lcd_uninitialize(void)
 {
   lcdinfo("Terminating LCD\n");

   if (g_lcd != NULL)
     {
       /* Turn the display off */

       g_lcd->setpower(g_lcd, 0);

       g_lcd = NULL;
     }
 }
