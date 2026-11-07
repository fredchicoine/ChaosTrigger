# Chaos Trigger by iNi

A 4-lane loop-slicing step sequencer instrument (VST3 / AU / Standalone), built with JUCE 8.

## Build

You need CMake 3.22+ and a C++17 compiler (Xcode on macOS, Visual Studio 2022 on Windows).

```
git clone --depth 1 -b 8.0.4 https://github.com/juce-framework/JUCE JUCE
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The plugins end up in `build/ChaosTrigger_artefacts/Release/` (VST3, AU on macOS, Standalone).
The AU target only builds on macOS. Copy the `.vst3` folder into your system VST3 folder.

## How the engine works

- **Lanes play in parallel.** All 4 loops run from the moment you press a key. The sequencer only
  opens each lane's ADSR gate in turn, so every step plays the matching slice of that loop in time.
- **RETRIG**: each new note restarts all loops and the sequence. **LEGATO**: a new note restarts
  only the sequence, and the loops keep running.
- **SYNC**: the step length comes from the DAW tempo and the Chaos Divider (1/128 to 1/1).
  **FREE**: the step length comes from the Free Rate knob (Hz).
- **Feel**: Straight; Triplet (×2/3); Dotted (×1.5); Swing (long-short pairs, amount knob).
- **Chaos Picker**: patterns 1–24 (every order of the 4 lanes) and 66 RANDOOM.
  Lanes that are powered off or have no sample are skipped.
- **Lane FX** (in this order): Distortion → Bitcrush (24 to 1 bit) → Smash (-6 dB 2:1 to -40 dB 20:1) →
  Juno chorus (I = 0.4 Hz triangle, II = 0.6 Hz triangle, I+II = 8 Hz shallow sine) → Haas (0–100 ms) → Pan/Vol.
- **Master FX**: the same set without chorus, plus Ring Mod (on/off + frequency) and a filter with
  LP 24 dB (Moog ladder), BP 6 dB and HP 12 dB, with cutoff and resonance.
- **Outputs**: lane 1 goes to outputs 1/2, lane 2 to 3/4, and so on. Enable the extra outputs in your DAW.
  If a lane's output isn't enabled, that lane is mixed into the main 1/2.
  The master FX run on each enabled output.
- Sample paths are saved with your project and reloaded when you open it.

## Loading samples
Pick a lane number (1–4) next to LOAD SAMPLE and press LOAD SAMPLE. You can also click a waveform, or drag a file onto it.

## Installing the downloaded builds
- **Windows:** copy `Chaos Trigger.vst3` into `C:\Program Files\Common Files\VST3`.
- **Mac:** copy `Chaos Trigger.vst3` into `~/Library/Audio/Plug-Ins/VST3` and `Chaos Trigger.component` into
  `~/Library/Audio/Plug-Ins/Components`. The plugin isn't signed by Apple, so before using it, run this
  once in Terminal: `xattr -cr ~/Library/Audio/Plug-Ins`
