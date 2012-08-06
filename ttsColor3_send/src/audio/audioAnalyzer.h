

#pragma once



#include "ofMain.h"
#include "aubioAnalyzer.h"
#include "fft.h"
#include "fftOctaveAnalyzer.h"


#define BUFFER_SIZE 256

// I take a chunk of audio and spit out analysis 


typedef struct {
    float volume;
    float pitch;
    float fft[9];
} audioAnalysisFrame;


class audioAnaylzer {

public: 
    
    aubioAnalyzer AA;
    
    void setup(int sampleRate){
        AA.setup(sampleRate);
        
        FFTanalyzer.setup(44100, BUFFER_SIZE/2, 1);
        
        FFTanalyzer.peakHoldTime = 15; // hold longer
        FFTanalyzer.peakDecayRate = 0.95f; // decay slower
        FFTanalyzer.linearEQIntercept = 0.9f; // reduced gain at lowest frequency
        FFTanalyzer.linearEQSlope = 0.01f; // increasing gain at higher frequencies
        
        
        
    }
    
    audioAnalysisFrame analyzeFrame(float * bufferOfAudio, int bufferSize, audioAnalysisFrame & frame){
        
        AA.processAudio(bufferOfAudio, bufferSize);
        
        frame.volume = AA.amplitude;
        frame.pitch = AA.pitch;
        
        float avg_power = 0;
        
        myfft.powerSpectrum(0,(int)BUFFER_SIZE/2, bufferOfAudio,BUFFER_SIZE,&magnitude[0],&phase[0],&power[0],&avg_power);
        
        for (int i = 0; i < (int)(BUFFER_SIZE/2); i++){
            freq[i] = magnitude[i];
        }
        
        FFTanalyzer.calculate(freq);
        
        for (int i = 0; i < 9; i++){
            frame.fft[i] = FFTanalyzer.peaks[i];
        }
       //cout << FFTanalyzer.nAverages << endl;
        
        
    }
    

    int 	bufferCounter;
    fft		myfft;
    FFTOctaveAnalyzer FFTanalyzer;
    
    float magnitude[BUFFER_SIZE];
    float phase[BUFFER_SIZE];
    float power[BUFFER_SIZE];
    float freq[BUFFER_SIZE/2];
    
    
    
};



