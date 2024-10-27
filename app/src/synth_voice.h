#include <circle/types.h>
#include <math.h>

#define SAMPLE_RATE	48000
#define CHUNK_SIZE	2048
#define M_PI 3.1415926536

enum waveType {
    WAVE_SIN,
    WAVE_SAW,
    WAVE_SQUARE,
    WAVE_TRI
};

class SynthVoice {
    public:
        SynthVoice(waveType type);
        void NoteOn(u8 pitch, u8 velocity);
        void NoteOff();
        float nextSample();
        bool isNoteOn();

    private:
        bool _noteOn;
        u8 _pitch;
        float _freq;
        u8 _velocity;
        float _phase;
        waveType _waveType;
};