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
array<int> timestamps = {0, 0};
int startNote = 48;

array<double> prevBlock;

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
  if(prevBlock.length != data.samplesToProcess) {
    prevBlock.resize(data.samplesToProcess);
    for(uint i = 0; i < prevBlock.length; i++) {
      prevBlock[i] = 0;
    }
  }

  for(uint eventIndex = 0; eventIndex < data.inputMidiEvents.length; eventIndex++)
  {
    MidiEvent event = data.inputMidiEvents[eventIndex];
    MidiEventType eventType = MidiEventUtils::getType(event);

    if(eventType == kMidiNoteOn)
    {
      uint note = MidiEventUtils::getNote(event);

      print("Midi ON" + formatInt(note));
      samples[note - startNote] = 0;
      timestamps[note - startNote] = int(event.timeStamp);
    }
    else if(eventType == kMidiNoteOff)
    {
      uint note = MidiEventUtils::getNote(event);

      print("Midi OFF" + formatInt(note));
      samples[note - startNote] = -1;
      timestamps[note - startNote] = int(event.timeStamp);
    }
  }

  for(uint sampleIndex = 0; sampleIndex < data.samplesToProcess; sampleIndex++)
  {
    data.samples[0][sampleIndex] += prevBlock[sampleIndex];
    prevBlock[sampleIndex] = 0;

    for(uint i = 0; i < wavData.length; i++) {
      int sample = samples[i];

      if(sample >= 0 && uint(sample) < wavData[i].interleavedSamples.length) {
        uint targetSample = uint(sampleIndex + timestamps[i]);
        double sampleValue = wavData[i].interleavedSamples[sample];

        if(targetSample < data.samplesToProcess) {
          data.samples[0][targetSample] += sampleValue;
        }
        else {
          uint prevTargetSample = targetSample - data.samplesToProcess;
          prevBlock[prevTargetSample] += sampleValue;
        }
        samples[i]++;
      }
    }
  }
}
