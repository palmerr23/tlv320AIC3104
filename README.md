# Arduino TLV320AIC3104 CODEC control library for Pico 

This code is ALPHA so use it at your own risk!

A generic Arduino control library for the Texas Instruments TLV320AIC3104 stereo channel CODEC. This CODEC is TDM-compatible as it can be programmed to offset its read/write slots to anywhere in a 256 slot TDM cycle and put the DO line in a Hi-Z state when not transmitting.

The library was created specifically for the associated boards:

* Four-CODEC Teensy Audio board (https://github.com/palmerr23/Teensy8x8AudioBoard) with a Pico to Teensy Adapter board (see Pico Audio Board repo).
* Pico Audio Board(single CODEC) (https://github.com/palmerr23/PicoAudioBoard/)

For Teensy Audio applications use the companion library control_TLV320AIC3104 (https://github.com/palmerr23/control_TLV320AIC3104)

As the AIC320 only supports a single I2C address, there is some code to ensure the correct CODEC is being programmed via a multiplexed I2C bus when the multi-codec option is selected.

The library contains drivers for the PCA9546 I2C multiplexer.

It can also support a single AIC3104 in I2S mode with no multiplexer.

Page references are to the TLV320AIC3104 datasheet, Feb 2021.

V1.0 January 2026

## Compatibility
Tested with Arduino Pico and Pico 2 with Arduino-pico 5.30 or later.

Tested with the Pico Audio Board.

May work with other architectures, however I2S commands will likely differ.


## Single CODEC I2S operation 
I2S mode is the default for single codecs, however TDM may be selected using the i2sMode argument.

Avoid the more complex forms of the various function calls, accepting the default arguments where they are available, as some combinations of arguments may not be compatible.

## Available Hardware

The companion hardware is described at https://github.com/palmerr23/TLV320AIC_module, https://github.com/palmerr23/PicoAudioBoard/

# Compiling

When using an audio board with a single CODEC (no multiplexer) compile with the following option:
```
 #define SINGLE_CODEC
```
Comment it out for multi-codec boards.

# Function Reference

This library has been tested with with a the Arduino-Pico 5.30  6-bit I2S (left justified) driver in output-only and duplex modes.

### Constructor

```
TLV320AIC3104 aic(I2C address); 
```

### Wire

Defining and initialising the Wire library is the responsibility of the user application. 

This must be completed before begin( ) is called.


### enable( )

Called without arguments, enable( ) scans for and configures all attached CODECs.

enable(codec) may be useful when debugging hardware.

By default, only INPUT1 is enabled. Used pad2() to enable INPUT2.

### Default arguments
For most commands, the channel and codec arguments may be omitted if all codecs are to be configured.

See tlv320aic3104.h for more details

## ADC

### inputMode(inputModes mode, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
### inputMode(inputModes mode, int8_t codec) {both channels set}

Set single-ended (AIC_SINGLE) or differential (AIC_DIFF) input mode for INPUT 1.

When called before enable( ), inputMode sets the default input mode and the second argument is ignored.

When called after enable( ), all CODECS (codec = -1) or a single CODEC may be affected.

Channel = 0 -> left; channel == 1 -> right; channel > 1 -> both

In differential mode '-' inputs should be grounded for unbalanced signals to reduce noise.

When differential input signals are connected in single-ended mode, crosstalk will occur between channels due to the CODEC's internal multiplexing (see Fig 10-13, p36).

The library defaults to differential mode to avoid this issue, at a small cost to noise performance.


### inputSelect(int level, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)

The balanced inputs (L1) are used for both mic and line inputs. PGA gain is adjustable between 0 and 59.5 dB.

Compliant with the Teensy audioControl standard and the SGTL5000 implementation, this sets an input to either MIC (-59.5 dB [-62.5 dBm]) or LINE (0 dB [-2.5 dBm]).

For finer gain control, use inputLevel( ).

### inputLevel(float gainVal, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS) 

and

### gain(float gainVal, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)

The two functions are equivalent, setting the maximum input level / gain of an input channel. 

The CODEC's balanced inputs (LINE1Lx and LINE1Rx) are used for both mic and line inputs. PGA gain is adjustable between 0 and 59.5 dB.

For inputLevel( ) the range is -59.9 to 0 dB.

For gain( ) the range is 0 to 59 dB.
 
Values outside these ranges are constrained. 

When called without channel and codec arguments, all codecs and channels are affected. 

### pad1(float pad, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Set the ADC INPUT1 input level control value (pad).

Value from 0 to -12 in 1.5 dB steps.

Does not change the maximum input voltage range.

### pad2(float pad, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Enable INPUT2 and set the input level control value (pad). 

Value from 0 to -12 in 1.5 dB steps.

Does not change the maximum input voltage range.

### adcHPF(int frequency, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
HPF frequencies may be set between 1Hz and 5kHz.

Less than 1 Hz will turn the HPF off.

The effectiveness of HPF frequencies less than 10Hz is untested.

When called without channel and codec arguments, all codecs and channels are affected. 

channel is a bit map with 1 = left, 2 = right, 3 = both channels.

The function should be called after the CODEC is enabled.

### setHPF(uint8_t option, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS) -- DEPRECATED
Input channel DC removal filter. These standard digital filter settings are not very useful for audio use, due to the high corner frequencies.

```
0 = off	- power on default
1 = 0.0045 Fs (198 Hz @ Fs = 44.1kHz) 
2 = 0.0125 Fs (551 Hz)
3 = 0.025  Fs (1102 Hz)
```
## DAC

### volume(float value, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Sets the volume of an main output channel (LOP/ROP). The range is 0.0 .. 1.0 
 
When called without channel and codec arguments, all codecs and channels are affected.

For most applications, using other means to control the output level is preferable to changing the default volume level using this function, due to the increase in digital noise.

Muting the DAC will produce lower output noise than setting the volume to zero (-63.5dB), however both sets of outputs (xOP/HPxOUT) are affected.

### HPvolume(float value, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Sets the volume of an main output channel (HPLOUT/HPROUT). The range is 0.0 .. 1.0 

HPLCOM and HPRCOM are configured as differential outputs.
 
When called without channel and codec arguments, all codecs and channels are affected.

For most applications, using other means to control the output level is preferable to changing the default volume level using this function, due to the increase in digital noise.

Muting the DAC will produce lower output noise than setting the volume to zero (-63.5dB), however both sets of outputs (xOP/HPxOUT) are affected.

### DACmute(bool mute, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Mute the DAC for both sets of outputs  (xOP/HPxOUT). 

When called without channel and codec arguments, all codecs and channels are affected.

Muting will produce lower output noise than setting the volume to zero (-63.5dB).

## DAC effects filters
The LTV320AIC3104 has two cascaded BiQuad filters available for each DAC channel.

The filters below follow the Teensy Audio Library BiQuad filter format, wherever appropriate.

Both stages are enabled or disabled together for each channel - see p32.

When called without channel and codec arguments, all codecs and channels are affected. 

The channel argument is a bit map with 1 = left, 2 = right, 3 or -1 = both channels.

The functions should be called after the CODEC is enabled.

Filters with gain must have their input signals attenuated, so the signal does not exceed 1.0

This object implements up to 2 cascaded stages. As both cascaded BiQuad filters are enabled together, the parameters of both sections should be set before enabling. The hardware default settings may have strange results. 

Biquad filters with low corner frequencies (under about 400 Hz) can run into trouble with limited numerical precision, causing the filter to perform poorly. For very low corner frequency, use the Audio Library's State Variable (Chamberlin) filter.

Q > 2 can be problematic for some filter types. For greater depth, cascade two filters.

### setFlat(int stage, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS)
Set one stage of the filter (0 or 1) with flat (all-pass) response.

This is the same as the default value when the CODEC is reset.

### setHighpass(int stage, float frequency, float q = 0.7071, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS);
Configure a stage of the filter (0 or 1) with high pass response, with the specified corner frequency and Q shape. If Q is higher that 0.7071, be careful of filter gain (see above).

### setLowpass(int stage, float frequency, float q = 0.7071f, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS);
Configure a stage of the filter (0 or 1) with low pass response, with the specified corner frequency and Q shape. If Q is higher that 0.7071, be careful of filter gain (see above).

### setBandpass(int stage, float frequency, float q = 1.0, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS);
Configure a stage of the filter (0 or 1) with band pass response. The filter has unity gain at the specified frequency. 

Q controls the width of frequencies allowed to pass. 

### setNotch(int stage, float frequency, float q = 1.0, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS);
Configure a stage of the filter (0 or 1) with band reject (notch) response. Q controls the width of rejected frequencies. Lower Q = wider frequency range, deeper notch.

### setLowShelf(int stage, float frequency, float gain, float slope = 1.0f, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS); 
Configure a stage of the filter (0 or 1) with low shelf response. A low shelf filter attenuates or amplifies signals below the specified frequency. 

Frequency controls the slope midpoint, gain is in dB and can be both positive or negative. 

The slope parameter controls steepness of gain transition. A slope of 1 yields maximum steepness without overshoot, lower values yield a less steep slope. 

See the picture below for a visualization of the slope parameter's effect. 

Be careful with positive gains and slopes higher than 1 as they introduce gain (see warning below).

### setHighShelf(int stage, float frequency, float gain, float slope = 1.0f, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS);

Configure a stage of the filter (0 or 1) with high shelf response. A high shelf filter attenuates or amplifies signals above the specified frequency. 

Frequency controls the slope midpoint, gain is in dB and can be both positive or negative. 

The slope parameter controls steepness of gain transition. A slope of 1 yields maximum steepness without overshoot, lower values yield a less steep slope. 

See the picture below for a visualization of the slope parameter's effect. 

Be careful with positive gains and slopes higher than 1 as they introduce gain (see warning below).

![Shelf filter characteristics](images/shelf_filter.png)

### setCustomFilter(int stage, const int *coefx, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS) 
A custom  biquad filter: which should be scaled to int16 in an int array.

The order is N0, N1, N2, D1, D2 (D0 set in hardware)

### setTIBQFilter(int stage, const int16_t *coefx, int8_t channel = AIC_ALL_CHANNELS, int8_t codec = AIC_ALL_CODECS) 
A custom  biquad filter as generated by TIBQ.

The order is N0, N1, N2, D1, D2 (D0 set in hardware).

## Hardware validation and debugging

### setVerbose(int verbosity)
Sets the level of messages on stderr. 

Set Tools > Debug Port to Serial in Arduino Pico

```
0: turns messages off.

1: will provide some error messages on startup if the hardware doesn't match the supplied number of CODECs, etc.

2: provides additional messages as hardware is enabled or values changed 
```

Should be left at the default (0) for production, as the writes may block execution if USB isn't connected.

### listMuxes( )
Useful for checking that the board jumpers are set as required.

Should be called after begin( ), where the muxes are probed and recorded.

## Examples
- Basic operation I2S mode. Sine output only or full duplex.




