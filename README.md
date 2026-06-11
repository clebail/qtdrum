# 🥁 qtdrum

> A tiny, no-nonsense drum machine for **Linux**, built with Qt & RtMidi — because sometimes a guitarist just needs a robot to keep the beat.

![qtdrum in action](https://github.com/clebail/qtdrum/raw/master/resources/images/screen_shot_1.png "qtdrum")

## What is this thing?

I wanted a metronome with attitude. Something I could point at while practicing guitar that wouldn't judge my timing, wouldn't crash, and wouldn't need a 4 GB DAW just to go *boom-tss-boom-tss*.

So `qtdrum` was born: a featherweight loop sequencer that fires MIDI notes at your favorite soft synth (FluidSynth, Timidity, whatever you've got plugged in) with rock-solid timing courtesy of **POSIX timers**. No fluff. No subscriptions. Just groove.

## ✨ Features

- 🎛️ **WYSIWYG loop editor** — click the grid, build your beat, watch it play. What you punch in is what you get.
- 🔢 **Fully tweakable grid** — choose how many beats and how many subdivisions per beat. From a lazy 4/4 to a math-rock nightmare.
- 🗣️ **Spoken bar count** — a human voice counts every minute up to 20 so you never lose track (and yes, you can shut it up).
- 🤫 **Smart muting** — silence *X measures out of N* on a loop. Perfect for practicing those "leave space for the singer" moments.
- 🦶 **Footswitch-friendly** — start/stop with a single key (defaults to `B`) so your hands stay on the guitar. Bump the tempo up/down or reset on the fly too.
- ⏱️ **POSIX timers under the hood** — tight, drift-resistant timing instead of flaky GUI timers.
- 💾 **Save & load your grooves** — store your patterns as plain-text `.qdr` files and bring them back any time.

## 🎹 How it works

`qtdrum` doesn't make a single sound on its own — it's a conductor, not an orchestra. It sends **MIDI** to whatever synth you point it at:

```bash
# Pick your poison:
./startFluidsynth.sh   # FluidSynth
./startTimidity.sh     # Timidity
```

Then launch `qtdrum`, choose the MIDI port, paint a pattern, and hit play. 🎸

## 🔨 Building

You'll need **Qt5** (core, gui, widgets, multimedia) and **RtMidi**.

```bash
qmake
make
./qtdrum
```

## ⌨️ Default shortcuts

| Key   | Action              |
|-------|---------------------|
| `B`   | Play / Stop         |
| `+`   | Tempo up (+10 BPM)  |
| `-`   | Tempo down (-10 BPM)|
| `0`   | Reset the loop      |

All remappable from the Options dialog — set it up for your trusty footswitch.

---

Made with 🥁 and a guitar that needed a drummer. Happy jamming!
