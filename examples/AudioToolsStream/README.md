# I2S Digital Output Test

44.1kHz, 16-bit mode

This is a simple basic test for sinewave to both main and HP I2S outputs 

We just send a generated sine wave and expect to hear a clean signal.
Please note the log level should be set so that there is no disturbing output!
 
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
