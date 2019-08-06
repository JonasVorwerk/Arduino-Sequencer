/* MIDI INSTRUMENT LIBRARY: 

MELODIC INSTRUMENTS (GM1) 
When using the Melodic bank (0x79 - same as default), open chooses an instrument and the octave to map 
To use these instruments below change "number" in talkMIDI(0xC0, number, 0) in setupMidi()


0   Acoustic Grand Piano       32  Acoustic Bass             64  Soprano Sax           96   Rain (FX 1)
1   Bright Acoustic Piano      33  Electric Bass (finger)    65  Alto Sax              97   Sound Track (FX 2)
2   Electric Grand Piano       34  Electric Bass (pick)      66  Tenor Sax             98   Crystal (FX 3)
3   Honky-tonk Piano           35  Fretless Bass             67  Baritone Sax          99   Atmosphere (FX 4)
4   Electric Piano 1           36  Slap Bass 1               68  Oboe                  100  Brigthness (FX 5)
5   Electric Piano 2           37  Slap Bass 2               69  English Horn          101  Goblins (FX 6)
6   Harpsichord                38  Synth Bass 1              70  Bassoon               102  Echoes (FX 7)
7   Clavi                      39  Synth Bass 2              71  Clarinet              103  Sci-fi (FX 8) 
8   Celesta                    40  Violin                    72  Piccolo               104  Sitar
9   Glockenspiel               41  Viola                     73  Flute                 105  Banjo
10  Music Box                  42  Cello                     74  Recorder              106  Shamisen
11  Vibraphone                 43  Contrabass                75  Pan Flute             107  Koto
12  Marimba                    44  Tremolo Strings           76  Blown Bottle          108  Kalimba
13  Xylophone                  45  Pizzicato Strings         77  Shakuhachi            109  Bag Pipe
14  Tubular Bells              46  Orchestral Harp           78  Whistle               110  Fiddle
15  Dulcimer                   47  Trimpani                  79  Ocarina               111  Shanai
16  Drawbar Organ              48  String Ensembles 1        80  Square Lead (Lead 1)  112  Tinkle Bell
17  Percussive Organ           49  String Ensembles 2        81  Saw Lead (Lead)       113  Agogo
18  Rock Organ                 50  Synth Strings 1           82  Calliope (Lead 3)     114  Pitched Percussion
19  Church Organ               51  Synth Strings 2           83  Chiff Lead (Lead 4)   115  Woodblock
20  Reed Organ                 52  Choir Aahs                84  Charang Lead (Lead 5) 116  Taiko
21  Accordion                  53  Voice oohs                85  Voice Lead (Lead)     117  Melodic Tom
22  Harmonica                  54  Synth Voice               86  Fifths Lead (Lead 7)  118  Synth Drum
23  Tango Accordion            55  Orchestra Hit             87  Bass + Lead (Lead 8)  119  Reverse Cymbal
24  Acoustic Guitar (nylon)    56  Trumpet                   88  New Age (Pad 1)       120  Guitar Fret Noise
25  Acoutstic Guitar (steel)   57  Trombone                  89  Warm Pad (Pad 2)      121  Breath Noise
26  Electric Guitar (jazz)     58  Tuba                      90  Polysynth (Pad 3)     122  Seashore 
27  Electric Guitar (clean)    59  Muted Trumpet             91  Choir (Pad 4)         123  Bird Tweet
28  Electric Guitar (muted)    60  French Horn               92  Bowed (Pad 5)         124  Telephone Ring
29  Overdriven Guitar          61  Brass Section             93  Metallic (Pad 6)      125  Helicopter
30  Distortion Guitar          62  Synth Brass 1             94  Halo (Pad 7)          126  Applause
31  Guitar Harmonics           63  Synth Brass 2             95  Sweep (Pad 8)         127  Gunshot  

PERCUSSION INSTRUMENTS (GM1 + GM2)

When in the drum bank (0x78), there are not different instruments, only different notes.
To play the different sounds, select an instrument # like 5, then play notes 27 to 87.
 
27  High Q                     43  High Floor Tom            59  Ride Cymbal 2         75  Claves 
28  Slap                       44  Pedal Hi-hat [EXC 1]      60  High Bongo            76  Hi Wood Block
29  Scratch Push [EXC 7]       45  Low Tom                   61  Low Bongo             77  Low Wood Block
30  Srcatch Pull [EXC 7]       46  Open Hi-hat [EXC 1]       62  Mute Hi Conga         78  Mute Cuica [EXC 4] 
31  Sticks                     47  Low-Mid Tom               63  Open Hi Conga         79  Open Cuica [EXC 4]
32  Square Click               48  High Mid Tom              64  Low Conga             80  Mute Triangle [EXC 5]
33  Metronome Click            49  Crash Cymbal 1            65  High Timbale          81  Open Triangle [EXC 5]
34  Metronome Bell             50  High Tom                  66  Low Timbale           82  Shaker
35  Acoustic Bass Drum         51  Ride Cymbal 1             67  High Agogo            83  Jingle bell
36  Bass Drum 1                52  Chinese Cymbal            68  Low Agogo             84  Bell tree
37  Side Stick                 53  Ride Bell                 69  Casbasa               85  Castanets
38  Acoustic Snare             54  Tambourine                70  Maracas               86  Mute Surdo [EXC 6] 
39  Hand Clap                  55  Splash Cymbal             71  Short Whistle [EXC 2] 87  Open Surdo [EXC 6]
40  Electric Snare             56  Cow bell                  72  Long Whistle [EXC 2]  
41  Low Floor Tom              57  Crash Cymbal 2            73  Short Guiro [EXC 3]
42  Closed Hi-hat [EXC 1]      58  Vibra-slap                74  Long Guiro [EXC 3]

*/
