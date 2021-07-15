# VCAgnosticMute

Repository for the source for https://hackaday.io/project/180695-multi-vc-mute-button - a video conferencing software mic and video macro button that is software agnostic.

While this was designed with a Seeeduino Xiao in mind, the Arduino code can be put on any microcontroller with USB-Serial and USB-HID (i.e. QT-Py, Leonardo (or other 32u4 based board), and STM or Pi Pico with some modifications) and if solely using the companion app, USB-HID is not required (but changes need to be made to the arduino code).

Demo video: https://youtu.be/qabKWWq_SRY

At the moment the companion app works with Zoom, Teams, Webex, Skype and Starleaf.  That said, its only been tested on 3 computers with varying versions of Windows 10 so I cant guarantee it will work for everyone (working solo here so dont expect a polished product).

Give it a go and best of luck.

Libraries (and Licenses) included:

hid-project (I'm not sure what license this is released under) https://github.com/NicoHood/HID

Adafruit NeoPixel (LGPL v3) https://github.com/adafruit/Adafruit_NeoPixel/blob/master/COPYING

Simple Rotary (GPLv3) https://github.com/mprograms/SimpleRotary

AudioSwitcher (Ms-PL) https://github.com/xenolightning/AudioSwitcher/blob/master/LICENSE

INIFileParser (MIT) https://github.com/rickyah/ini-parser/blob/master/LICENSE

Input Simulator (MIT) https://github.com/michaelnoonan/inputsimulator/blob/master/LICENSE

And I'm releasing this under GNU GPL v3

https://github.com/Collie147/VCAgnosticMute/blob/main/LICENSE
