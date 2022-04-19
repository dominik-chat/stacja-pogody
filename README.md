# Stacja pogody

![LICENSE](https://img.shields.io/github/license/dominik-chat/stacja-pogody?style=flat-square)

Stacja pogody is a project incorporating external sensor board with internal server via bluetooth.

Stacja pogody uses bmp280 sensor and NRF DK board as external measurment device.
The internal device is bluetooth capable Raspberry Pi.

## Compilation

The program requires [ncs](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html).

The compilation and programming itself can be performed by executing the following commands in the root of the repository:
```
west build -b nrf52dk_nrf52840
west flash
```
