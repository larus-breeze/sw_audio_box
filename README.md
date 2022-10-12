Input/Output module for CAN-bus-driven sound and GPIO
======================================================

* Generate **"vario-"** and **"speed-commander" - sound** 
* Optionally: Read **Bosch BME68x** sensor and provide data over the **CAN bus**
* Optionally: **USART-drivers availale** to communicate to additional hardware

The micro-controller is run under **FreeRTOS**.

# How to use it: 
Import the project using the **STM32 CUBE IDE**

Be sure to set up the correct *.ld linker information for your hardware.

Compile software, flash it into the micro-controller.

The software is licensed under the GNU Public License V3.
