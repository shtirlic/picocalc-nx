# NuttX for PicoCalc

This repository contains the board support package for the [PicoCalc][picocalc] on [NuttX][nuttx].

## Work In Progress

Only Raspberry Pi Pico 2/2 W RISC-V Arch are supported for now. Other configurations may be added later.

The PicoCalc is an device based Rapberry Pi Pico family (Pico, Pico 2) portbale MCU termninal with several peripherals.

This board support package provides support for some of these peripherals.

- [x] Rasbberry Pico 2 RISC-V Arch
- [x] NuttX ostest passed (some issues https://github.com/apache/nuttx/issues/16133)
- [x] SD card access (FAT, automount to `/mnt/sd0`)
- [x] SPI
- [x] I2C Master
- [x] Serial console
- [x] PWM 2-channels (A+B) for stereo speakers (PWM5)
- [x] Speakers output
- [x] ILI9488 SPI Display 320x320
- [x] Color 16-bit RGB565
- [ ] Color 24-bit RGB888
- [x] Custom UI Shell atomSHELL
- [x] Optional framebuffer device
- [ ] External PSRAM (not enabled yet)
- [x] Custom I2C keyboard as '/dev/kbd' device
- [ ] Special keys handling
- [ ] Power unit and battery
- [ ] SMP multicore (https://github.com/apache/nuttx/issues/16133)
- [x] RISC-V cores
- [ ] Rasbberry Pico 2 W wireless support


## Usage and Testing pre-release firmware builds

Current pre-release firmware uf2 files are in https://github.com/shtirlic/picocalc-nx/releases

## USB-C Serial connection to PicoCalc

After connecting the PicoCalc USB-C to your PC, fire up a serial terminal and you should see the NuttX `nsh` prompt.

Use `help` for available apps and commands.

```console
minicom -con -D /dev/ttyUSB0 -b 115200
```

```console
picocalc> _
```
Use `reboot` or `reboot bootloader` to reboot or go to bootsel mode to load firmware.


## Manual Build

Once you have installed all NuttX dependencies and set up the NuttX build environment according to the [NuttX guide][nx-install], you can clone this repository inside your `nuttx-space` dir.

**Note:** you should also clone the 2.2.0 version of the [Pico SDK][pico-sdk] in your `nuttx-space` too.

Your folder structure should look like this:

```console
$ ls
nuttx
nuttx-apps
picocalc-nx
pico-sdk
```

Then you can set the `PICO_SDK_PATH` build environment variable:

```console
export PICO_SDK_PATH="/absolute/path/to/nuttx-space/pico-sdk"
```

And then from within the `nuttx` folder, you can configure and build one of the PicoCalc configurations. In this case, the
`atomshell` configuration.

```console
cd nuttx
make distclean
./tools/configure.sh ../picocalc-nx/configs/atomshell
make -j
```

You should now have a UF2 `nuttx.uf2` that you can upload to the PicoCalc Pico board via Micro-USB port!

[picocalc]: https://github.com/clockworkpi/PicoCalc
[pico-sdk]: https://github.com/raspberrypi/pico-sdk
[nx-install]: https://nuttx.apache.org/docs/latest/quickstart/install.html
[nuttx]: https://nuttx.apache.org/
