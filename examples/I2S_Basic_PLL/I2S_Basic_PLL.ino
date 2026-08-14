/*
 * TLV320AIC3104 I2S module
 * Arduino-Pico V5.3.0

 * Single CODEC in I2S duplex mode
 * Multiplexer is disabled
 * Normal mode:
	 * Channel 0 output = channel 0 input
	 * Channel 1 output = sine
 * Output_only mode:
	 * Both channels sine, left is 180 deg out of phase (inverted)
 */

 // comment out next line for L channel pass-though
#define OUTPUT_ONLY // 2 channels of sine output, left is inverted
#define CODECS 1

uint32_t sampleRate = 48000; // 44100 is Teensy standard
int sampleLength = 16;
uint16_t MCLKmult = 256; // works for all 16-bit sample rates

static const int I2SSYSCLK_44_1 = 135600; // 22.05, 44.1 kHz sample rates
static const int I2SSYSCLK_8 = 147600;  // 8k, 16, 32, 48, 96, 192 kHz

#include <Wire.h>
#include <I2S.h>
#ifdef OUTPUT_ONLY
  I2S i2s(OUTPUT); 
#else
  I2S i2s(INPUT_PULLUP); //  bi-directional
#endif

#include "tlv320aic3104.h"
TLV320AIC3104 aic(CODECS, false, AICMODE_I2S, sampleRate, sampleLength); // use BCLK and PLL for master clock generation (no MCLK required)

// CODEC PINS - for the Pico Audio Board
#define DOUT 20
#define DIN  21
#define WCLK 19
#define BCLK 18
#define MCLK 17
#define RST  16

// Teensy Audio Board
#define SDAPIN 4 // Wire1 (6) on V1, Wire (4) on V2
#define SCLPIN 5 // Wire1 (7) on V1, Wire (5) on V2

#define BUFLEN 2048
int16_t outBuf[BUFLEN]; // stereo
volatile int bytesAvail;

void i2sCallback()
{
  bytesAvail = i2s.availableForWrite();
}

int muxesFound = 0;
#define PRINT_EVERY 10000
uint32_t timer;
void setup() 
{
  Serial.begin(115200);
	while(!Serial)
			delay(10);
  Serial.println("\n\nArduino Pico Duplex I2S TLV320AIC3104 PLL example");
	
	Wire.setSDA(SDAPIN);
	Wire.setSCL(SCLPIN);
  Wire.begin();
  Wire.setClock(100000);

	fillSineBuffer(500, 32000); // 500Hz, ~32000 max value (16 bits)
  set_sys_clock_khz(I2SSYSCLK_8, false);


#ifdef OUTPUT_ONLY
  i2s.setDATA(DOUT);
#else
  i2s.setDOUT(DOUT);
	i2s.setDIN(DIN);
#endif
  i2s.setBCLK(BCLK); // LRCLK (WCLK) pin = BCLK + 1
	//i2s.setMCLK(MCLK);
  //i2s.setMCLKmult(MCLKmult);
  i2s.setBitsPerSample(sampleLength);
  i2s.setFrequency(sampleRate);
  i2s.onTransmit(i2sCallback);
  i2s.begin();
  
  int8_t testCodec = 0;
  if (CODECS == 1)
    testCodec = 0;

  delay(100);
  aic.i2cBus(&Wire);
  aic.setVerbose(2);

  aic.begin(RST);
  i2cScan(&Wire); // need to scan after MCLK is present and chip is reset
   // command when issued before enable(): all inputs are affected
  aic.inputMode(AIC_SINGLE); // or AIC_DIFF

  aic.setPllClkIn(sampleRate);

  if(!aic.enable(testCodec)) // After enable() DAC and ADC are muted
    Serial.println("Failed to initialise codec");

  aic.volume(1, -1, testCodec);  
  aic.HPvolume(1, -1, testCodec);  
  aic.DACmute(false);   // outputs muted on startup
  aic.inputLevel(0, -1, testCodec); // level in dB: 0 = line level, ~-50 = mic level
  aic.pad2(0,-1,testCodec); // enable IPUT 2

  Serial.println("Done setup");
  timer = millis();
}

int bufIndx = 0;
void loop() 
{
	int16_t l, r;
#ifdef OUTPUT_ONLY
	i2s.write16(-outBuf[bufIndx], outBuf[bufIndx]); // left channel is inverted
#else
	i2s.read16(&l, &r);
  i2s.write16(l, outBuf[bufIndx]); // left channel pass-through
#endif

	bufIndx = (bufIndx + 1) % BUFLEN;  
  if(millis() > timer + PRINT_EVERY)
  {  
    Serial.printf("Bytes avail to write %i\n", bytesAvail);
    timer = millis();
  }
}

// frequency should be limited to < 1/2 sampleRate
int fillSineBuffer(uint16_t freqTarget, int countMax)
{
  int cycsTable = 0.5 + 1.0 * freqTarget * BUFLEN / sampleRate; // round
  int freq = cycsTable * sampleRate / BUFLEN;
	for(int i = 0; i < BUFLEN; i++)
		outBuf[i] = countMax * sin(i * 2.0 * 3.14159 * cycsTable / BUFLEN);
	return freq;
}
void i2cScan(TwoWire *i2c) {
  int count = 0;   
 // genReset();
     
  Serial.print ("Scan ");

  for (uint8_t i = 8; i < 120; i++)
  {     
    i2c->beginTransmission (i);
   // MyWire.write(0x10);
    if (i2c->endTransmission () == 0)
      {
      Serial.print ("- Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      delayMicroseconds(50);  // maybe unneeded? Easy to see "found" devices on Logic Analyser
      count++; 
      } // end of good response  
      delayMicroseconds(10);  
      
  } // end of for loop

  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s)\n");
}

/*
void printPll()
{
  aic_pll pll;
  pll = aic.getPll();
  Serial.printf("Pll P 0x%02x, R 0x%02x, J 0x%02x, D 0x%04x, Q 0x%02x\n", pll.p, pll.r, pll.j, pll.d, pll.q);
}
*/
