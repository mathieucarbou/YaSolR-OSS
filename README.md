[![YaSolR (Yet another Solar Router)](./docs/assets/img/logo.png?raw=1)](https://yasolr.carbou.me/)

👉 Website: [https://yasolr.carbou.me/](https://yasolr.carbou.me/)

[![GPLv3 license](https://img.shields.io/badge/License-GPLv3-blue.svg)](http://perso.crans.org/besson/LICENSE.html)

[![Download](https://img.shields.io/badge/Download-bin-green.svg)](https://yasolr.carbou.me/download)
[![Doc](https://img.shields.io/badge/Doc-html-green.svg)](https://yasolr.carbou.me/)

[![YaSolR OSS Sources](https://img.shields.io/badge/YaSolR%20OSS-sources-green.svg)](https://github.com/mathieucarbou/YaSolR-OSS/) [![YaSolR OSS Build](https://github.com/mathieucarbou/YaSolR-OSS/actions/workflows/build-branch.yml/badge.svg)](https://github.com/mathieucarbou/YaSolR-OSS/actions/workflows/build-branch.yml)

[![YaSolR Pro Sources](https://img.shields.io/badge/YaSolR%20Pro-sources-green.svg)](https://github.com/mathieucarbou/YaSolR/) [![YaSolR Pro Build](https://github.com/mathieucarbou/YaSolR/actions/workflows/build-branch.yml/badge.svg)](https://github.com/mathieucarbou/YaSolR/actions/workflows/build-branch.yml)

# Developper guide

## Bulding the firmware

1. Configure `platformio_override.ini` to your needs
2. Build the file system and the firmware:

```bash
./build.sh
```

## Flashing

First time, flash the entire firmware which includes the parition table and all paritions:

```bash
esptool.py --port /dev/ttyUSB0 \
  --chip esp32 \
  --before default_reset \
  --after hard_reset \
  write_flash \
  --flash_mode dout \
  --flash_freq 40m \
  --flash_size detect \
  0x0 YaSolR-VERSION-MODEL-CHIP.factory.bin
```

Next time, juts upload the parition you modify

```bash
./upload.sh
```
