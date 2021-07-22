// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <TeensyVariablePlayback.h>

// GUItool: begin automatically generated code
AudioPlaySdResmp         playSd1;     //xy=324,457
AudioOutputI2S           i2s2;           //xy=840.8571472167969,445.5714416503906
AudioConnection          patchCord1(playSd1, 0, i2s2, 0);
AudioConnection          patchCord2(playSd1, 1, i2s2, 1);
AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

#define A14 10

const char* _filenames[] = {"CLAP.WAV", "CRASH.WAV", "FX04.WAV", "FX09.WAV"};
uint16_t _fileIndex = 0;

const int analogInPin = A14;
unsigned long lastSamplePlayed = 0;

double getPlaybackRate(int16_t analog) { //analog: 0..1023
    return  (analog - 512.0) / 512.0;
}

void setup() {
    analogReference(0);
    pinMode(analogInPin, INPUT_DISABLE);   // i.e. Analog

    Serial.begin(57600);

    if (!(SD.begin(BUILTIN_SDCARD))) {
        // stop here if no SD card, but print a message
        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
    audioShield.enable();
    audioShield.volume(0.5);

    playSd1.enableInterpolation(true);
    int newsensorValue = analogRead(analogInPin);
    playSd1.setPlaybackRate(getPlaybackRate(newsensorValue));

    AudioMemory(24);
}

void loop() {

    int newsensorValue = analogRead(analogInPin);
    playSd1.setPlaybackRate(getPlaybackRate(newsensorValue));
    
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 500) {
        if (!playSd1.isPlaying()) {
            
            if (playSd1.playWav(_filenames[_fileIndex])) {
              lastSamplePlayed = currentMillis;
              Serial.printf("playing %s\n", _filenames[_fileIndex]);
            }

            int numFiles = sizeof(_filenames)/sizeof(char*);
            _fileIndex++;
            _fileIndex %= numFiles;
            Serial.print("Memory: ");
            Serial.print(AudioMemoryUsage());
            Serial.print(",");
            Serial.print(AudioMemoryUsageMax());
            Serial.println();
        }
    }
    delay(10);
}