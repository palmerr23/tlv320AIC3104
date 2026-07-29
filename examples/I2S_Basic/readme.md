## Demonstrate the basic functions of the 8x8 Audio Board

This example uses the standard Arduino-Pico I2S drivers to produce a 500Hz tone from a static buffer.

Sample rate 48kHz, 16 bit samples

### Pico Audio Board (TLV320AIC3104)

CODEC  |	Pico
-----|----------------
VCC  |	3.3V
GND  |	GND
BCK  |	GPIO18
DIN  |	GPIO20
DOUT |  GPIO21
WCK  |	GPIO19
MCLK |	GPIO17
SDA  |  GPIO4 (Wire)
SCL  |  GPIO5