# music4xxx
## Hybrid Music 4000 - Midi adapter 

This is an adapter to allow a MIDI keyboard (or other MIDI device) to appear to the Hybrid Music System as a Hybrid Music4000 keyboard. 

The original Music4000 is a fully polyphonic four octave keyboard which connects to the BBC Microcomputer's user port. The keys are only switched, so there is no velocity or aftertouch. However these can be added after the fact in the Music5000 software.

The adapter takes Midi Note-On and Note-Off events and uses eight shift registers to replicate the original Music4000 keyboard matrix. The shift registers are latched automatically by the host BBC Micro's user port. It uses all through-hole parts, so is easy to build, and as the BBC Micro's userport is handled using hardware, the code on the microcontroller is simplified.. The ATMega328 microcontroller is programmed using the Arduino environment. A simple 3d printed enclosure is provided.


![music4xxx boxed](https://github.com/jlbeebprojects/music4xxx-AtMega/raw/master/images/music4xxx-enclosure-open.jpg)


![music4xxx PCB render](https://github.com/jlbeebprojects/music4xxx-AtMega/raw/master/images/music4xxx-pcb-render.png)


## Resources

* Retro-kit's archive of information of the original Music 4000 keyboard, including manuals and brochures: https://www.retro-kit.co.uk/page.cfm/content/Hybrid-Music-4000-Keyboard/

* Here's the Stardot thread about this project: https://stardot.org.uk/forums/viewtopic.php?f=3&t=16895

* The Stardot 8bit hardware forum often has posts about the Hybrid Music system: https://stardot.org.uk/forums/viewforum.php?f=3

## Other options

There are a couple of independently developed alternatives:

* Hideaway Studio's commercially available adapter: http://www.hideawaystudio.net/ details: https://stardot.org.uk/forums/viewtopic.php?f=3&t=16893

* Daniel J's open source STM32 and Blue Pill based solutions: Github: https://github.com/drdpj/MidiKeyboardBluePill Stardot thread: https://stardot.org.uk/forums/viewtopic.php?f=3&t=17146

## Licence

Unless otherwise marked, work in this repository is licenced under [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/)  ![Creative Commons Licence](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)

