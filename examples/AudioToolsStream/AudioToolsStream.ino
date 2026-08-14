/**
 * @file streams-generator-i2s.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-generator-i2s/README.md 
 * @copyright GPLv3
 June 2026 release or later
 */
/*
 
    Change MCLK multiplier to 256
   // uses std RP2040 and I2S config - different pins below.  
  Need to set SYSCLK

  Issues below seem to be fixed in June 2026 release
  ** works with gaps in waveform - missing buffers.
    Somewhat Worse on CPU1
    setting USE_AUDIO_LOGGING to false in AudioToolsConfig.h doesn't fix it
    Free RTOS - slightly worse
    decreasing I2S_BUFFER_SIZE to 256 makes gaps closer together, increasing to 1024 makes them further apart.
    Increasing buffer count makes no fifference
    ---> is DMA multiple buffering used?
*/
#include "AudioTools.h"

//static const int I2SSYSCLK_44_1 = 135600; // 22.05, 44.1 kHz sample rates
static const int I2SSYSCLK_44_1 = 158400; // 22.05, 44.1 kHz sample rates
//static const int I2SSYSCLK_44_1 = 158000; // 22.05, 44.1 kHz sample rates
static const int I2SSYSCLK_8 = 153600;  // 24, 48, 96 kHz sample rates

uint32_t sampleRate = 44100; // 48000
int sampleLength = 16;
uint16_t MCLKmult = 256; // works for all 16-bit sample rates (see I2SConfigStd.h)

#define CODECS 1
#include "tlv320aic3104.h"
TLV320AIC3104 aic(CODECS, true, AICMODE_I2S, sampleRate, sampleLength);

// CODEC PINS - for the Pico Audio Board
#define DOUT 20
#define DIN  21
#define WCLK 19
#define BCLK 18
#define MCLK 17
#define RST  16

#define SDAPIN 4 // Wire 
#define SCLPIN 5 // Wire 

AudioInfo info(sampleRate, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<int16_t> sound(sineWave);             // Stream generated from sine wave
I2SStream out; 
StreamCopy copier(out, sound);                             // copies sound into i2s

// Arduino Setup
void setup(void) {     
  int8_t testCodec = 0; 
  // Open Serial 
  Serial.begin(38400);
    while(!Serial);
  if(sampleRate == 44100)
    set_sys_clock_khz(I2SSYSCLK_44_1, false);
  else
    set_sys_clock_khz(I2SSYSCLK_8, false);

  //AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning); // Info

  // start I2S
  Serial.println("starting I2S...");
  auto config = out.defaultConfig(TX_MODE);
  config.copyFrom(info); 
  config.pin_ws = 19;
  config.pin_bck = 18;
  config.pin_data = 20;
  config.pin_mck = 17; // tx in duplex mode
  config.pin_data_rx = 21;
  config.mck_multiplier = MCLKmult;
  out.begin(config);

// Rev A uses Wire
	Wire.setSDA(SDAPIN);
	Wire.setSCL(SCLPIN);
  Wire.begin();
  Wire.setClock(100000);

  aic.i2cBus(&Wire);
  aic.setVerbose(2);

  aic.begin(RST);
  // when issued before enable(): all inputs are affected
  aic.inputMode(AIC_SINGLE); // or AIC_DIFF

  if(!aic.enable(testCodec)) // After enable() DAC and ADC are muted
    Serial.println("Failed to initialise codec");
  else
    Serial.println("CODEC OK");

  aic.volume(1, -1, testCodec);  
  aic.HPvolume(1, -1, testCodec);  
  aic.DACmute(false);   // outputs muted on startup
  aic.inputLevel(0, -1, testCodec); // level in dB: 0 = line level, ~-50 = mic level
  aic.pad2(0,-1,testCodec); // enable IPUT 2

  // Setup sine wave
  sineWave.begin(info, N_B5); // ~1kHz
  Serial.println("started...");
}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}


