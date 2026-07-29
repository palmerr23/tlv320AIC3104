/*
Endpoints
  Input or output. Single channel stream (e.g. 2 eandpoints for stereo)

Routing graph management
	Do we allow dynamic routing connections and disconnections?
	output to input
	1 : many is OK
	Each output object channel gets assigned a fixed buffer (from a pool?) if there is a connection to it. (How is this managed with dynamic routing?)
	Each routed input gets the buffer reference.
	NULL reference, skip processing.

	register routes or simply tell each end which buffer to use? 
	Are routings handled by a central process (list...) or are they simply a handover of the output buffer reference to the input object? 
	Ie input object calls io->subscribe(oo, num) to get the correct buffer? 
	If so we might only need to record output objects.

 Each connection is stored when created in a processing (linked) list which is executed, in order, each cycle. 

 Audio buffers are alloc'd as a pool or as needed? 

Processing
 The output buffer is overwritten on each cycle.
 Objects may take a buffer copy for later use (e.g. FFT that needs to process multiple input buffers). These should be returned to the pool once processed.

 only one object should initiate a processing cycle, so the first that has cyclic interrupts should claim the privilege.

 Interrupts should occur at the same rate for each device, so DMA buffer sizes will differ for each combination of sample width and channels.
 Sample size conversion is the responsibility of the object. All transmitted samples are SI32.

 Balance between audio latency and buffer size needs to be found.
 Assuming 128-sample audio-buffers

 48kHz:
 256 byte DMA buffers @ mono, 16-bit samples
 512 byte DMA buffers @ I2S stereo, 16-bit samples
 1024 byte DMA buffers @ I2S stereo, 32-bit samples
 2048 byte DMA buffers @ TDM 8-channel, 16-bit samples 
 4096 byte DMA buffers @ TDM 8-channel, 32-bit samples 
*/	

#define SAMPLES_CYCLE 128	// a reasonable compromise at 44.1 and 48kHz (~2.5mS), a bit short for 96kHz (1.3mS).

enum sampleRate_t {SR44_1, SR48, SR96};
enum sampleFormat_t {SI16, SI24, SI32, SF32};
enum objectType_t {AUDIO_INPUT, AUDIO_OUTPUT, AUDIO_DUPLEX};


// will the constructor of the object and the routing implementation use direct pointers to objects or will the connections be via buffer references?
// object keeps track of its own buffers??? How will it know if there's a connection
typedef audioObject {	
	// some form of ID???
	objectType_t type;	
	uint8_t channelsIn;
	uint8_t channelsOut;
	sampleFormat_t sampleWidth;
	sampleRate_t sampleRate;
	*procFn();
} audio_object_t;

// a pool of these, allocated at the start or just allocated as needed and somehow belonging to the output object?
// How is routing handled?
#define AUDIO_BUFLEN SAMPLES_CYCLE
typedef struct AudioBuffer {
	int32_t samples[AUDIO_BUFLEN];	
	uint16_t subscribers; // is this block in use? 
	// maybe a pool reference if we use pooled allocations

} audio_buffer_t;

