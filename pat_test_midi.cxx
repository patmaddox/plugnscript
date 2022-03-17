/** \file default.cxx
*   Default script, with no processing function.
*   Will simply do nothing (full audio and MIDI bypass) but show its description.
*/
#include "../library/Midi.hxx"
#include "../library/WaveFile.hxx"

string name = "Pat Test MIDI";
string description = "A simple MIDI plugin";

bool isPlaying = false;
WaveFileData wav1Data;
uint wav1Sample = 0;

bool initialize()
{
  print("how many channels?");
  print(formatInt(audioOutputsCount));

  string filePath = scriptDataPath + "/3p_snare.wav";
  bool ok = wav1Data.loadFile(filePath);

  return ok;
}

void processBlock(BlockData& data)
{
  for(uint eventIndex = 0; eventIndex < data.inputMidiEvents.length; eventIndex++)
  {
    MidiEvent event = data.inputMidiEvents[eventIndex];
    MidiEventType eventType = MidiEventUtils::getType(event);

    if(eventType == kMidiNoteOn)
    {
      print("Midi ON");
      isPlaying = true;
    }
    else if(eventType == kMidiNoteOff)
    {
      print("Midi OFF");
      isPlaying = false;
      wav1Sample = 0;
    }
    else
    {
      print("Unknown Midi type");
    }
  }

  for(uint sampleIndex = 0; sampleIndex < data.samplesToProcess; sampleIndex++)
  {
    if(isPlaying && wav1Sample < wav1Data.interleavedSamples.length) {
      for(uint channel = 0; channel < wav1Data.channelsCount; channel++) {
        data.samples[channel][sampleIndex] = wav1Data.interleavedSamples[wav1Sample];
        wav1Sample++;
      }
    }
    else {
      data.samples[0][sampleIndex] = 0.0;
      data.samples[1][sampleIndex] = 0.0;
    }
  }
}
