## Demonstrate the basic functions of the Pico Audio Board

*`I2S_Basic`* uses the standard Arduino-Pico I2S drivers to produce a 500Hz tone from a static buffer.

* 1I2S_Basic_PLL`* uses BCLK and the internal PLL to generate the CODEC master clock. An external MCLK signal is not required.

*`AudioToolStream`* creates a synthesised sinewave to both main and HP I2S outputs using the AudioTools library https://github.com/pschatzmann/arduino-audio-tools


