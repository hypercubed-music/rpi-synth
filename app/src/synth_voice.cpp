#include "synth_voice.h"

const float s_KeyFrequency[/* MIDI key number */] =
{
	8.17580, 8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.9783, 13.7500,
	14.5676, 15.4339, 16.3516, 17.3239, 18.3540, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997,
	25.9565, 27.5000, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535,
	46.2493, 48.9994, 51.9131, 55.0000, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817,
	82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110.000, 116.541, 123.471, 130.813, 138.591,
	146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942,
	261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000,
	466.164, 493.883, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991,
	830.609, 880.000, 932.328, 987.767, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91,
	1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02,
	2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92,
	4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
	8372.02, 8869.84, 9397.27, 9956.06, 10548.1, 11175.3, 11839.8, 12543.9
};

SynthVoice::SynthVoice(waveType type) {
    _waveType = type;
    _noteOn = false;
}

void SynthVoice::NoteOn(u8 pitch, u8 velocity) {
    _noteOn = true;
    _velocity = velocity;
    _pitch = pitch;
    _freq = s_KeyFrequency[pitch];
}

void SynthVoice::NoteOff() {
    _noteOn = false;
}

bool SynthVoice::isNoteOn() {
    return _noteOn;
}

float SynthVoice::nextSample() {
    if (_noteOn) {
        _phase += _freq / SAMPLE_RATE;
        _phase -= (int)_phase;

        float sample = 0;
        switch (_waveType)
        {
            case WAVE_SIN:
                sample = sin(_phase * 2 * M_PI);
                break;
            case WAVE_SAW:
                sample = (_phase - 0.5) * 2.0;
                break;
            case WAVE_SQUARE:
                sample = _phase > 0.5 ? 1.0 : -1.0; 
                break;
            case WAVE_TRI:
                sample = _phase < 0.5 ? ((_phase - 0.25) * 4.0) : (0 - ((_phase - 0.75) * 4.0));
                break;
        }
        float sampleVolume = (float)_velocity / 127.0;
        return sample * sampleVolume;
    } else {
        return 0;
    }
}