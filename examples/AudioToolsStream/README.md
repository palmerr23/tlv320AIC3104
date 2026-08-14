# I2S Digital Output Test

A basic test for a synthesised sinewave to both main and HP I2S outputs using the AudioTools library https://github.com/pschatzmann/arduino-audio-tools

44.1kHz, 16-bit mode, 1kHz tone, 2V p-p

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
