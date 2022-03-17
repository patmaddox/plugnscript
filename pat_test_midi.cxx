/** \file default.cxx
*   Default script, with no processing function.
*   Will simply do nothing (full audio and MIDI bypass) but show its description.
*/
#include "../library/Midi.hxx"
#include "../library/WaveFile.hxx"

string name = "Pat Test MIDI";
string description = "A simple MIDI plugin";

bool isPlaying = false;

array<WaveFileData> wavData = {WaveFileData(), WaveFileData()};
array<int> samples = {-1, -1};
int startNote = 48;

bool initialize()
{
  print("how many channels?");
  print(formatInt(audioOutputsCount));

  string filePath;

  filePath = scriptDataPath + "/3p_kick.wav";
  if(!wavData[0].loadFile(filePath)) { return false; }

  filePath = scriptDataPath + "/3p_snare.wav";
  if(!wavData[1].loadFile(filePath)) { return false; }

  return true;
}

void processBlock(BlockData& data)
{
  for(uint eventIndex = 0; eventIndex < data.inputMidiEvents.length; eventIndex++)
  {
    MidiEvent event = data.inputMidiEvents[eventIndex];
    MidiEventType eventType = MidiEventUtils::getType(event);

    if(eventType == kMidiNoteOn)
    {
      uint note = MidiEventUtils::getNote(event);

      print("Midi ON" + formatInt(note));
      samples[note - startNote] = 0;
    }
    else if(eventType == kMidiNoteOff)
    {
      uint note = MidiEventUtils::getNote(event);

      print("Midi OFF" + formatInt(note));
      samples[note - startNote] = -1;
    }
    else
    {
      print("Unknown Midi type");
    }
  }

  for(uint sampleIndex = 0; sampleIndex < data.samplesToProcess; sampleIndex++)
  {
    double sampleValue = 0;

    for(uint i = 0; i < wavData.length; i++) {
      int sample = samples[i];

      if(sample >= 0 && uint(sample) < wavData[i].interleavedSamples.length) {
        sampleValue += wavData[i].interleavedSamples[sample];
        samples[i]++;
      }
    }

    data.samples[0][sampleIndex] = sampleValue;
    data.samples[1][sampleIndex] = sampleValue;
  }
}
