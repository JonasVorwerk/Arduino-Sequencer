# Arduino-Sequencer
Arduino Sequencer

A basic plug and play step sequencer using:

Music Maker FeatherWing
https://www.adafruit.com/product/3357

TFT FeatherWing - 2.4" 320x240 Touchscreen 
https://www.adafruit.com/product/3315

and a Adafruit Feather 32u4 Basic Proto, 
but I guess any other Feather will work
https://www.adafruit.com/product/2771

Close the midi jumper on the bottom of the 
music featherwing and upload the sketch and
guaranteed hours of pleasure! 

Jonas Vorwerk
22-08-2019 Rotterdam, The Netherlands
https://jonasvorwerk.nl

Changes:

22-08-2019
- Patterns added! Performance on 32u4 is not making me happy
  but you could disable some display features line SHOWSEQUENCE, 
  SHOWPATTERNS and SHOWGRID to improve timing

09-08-2019
- Now posible to change instuments using the INSTR button
- Change Sequencer size
- Separate midi channel for each track

05-08-2019
- initial release
