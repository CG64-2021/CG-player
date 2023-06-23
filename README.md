# CG-player
You can use this program to create and play .cg files.

.cg files are binary files that carries the following information:
char[4] id //'CGSA'
uint32_t LE num_events //There are 'num_events' events in this file
uint16_t LE evoffset //First event's offset relative to the first point of the file
uint8_t events[num_events] //a event is a set of 8 musical notes (Do, Re, Mi, Fa, Sol, La, Si, Octave-Do)

I just created that program for learning purposes, so its code is messy and you can store only 8 notes per event.
When a .cg file is played, you can hear piano sounds being played. If you want to record or play a .cg file, just open CGplayer.exe and follow the instructions displayed in screen.

OBS: When writing a name to create a .cg file DON'T PUT THE ".cg" STRING IN THE END because the program will do it for you and
DON'T GIVE NAMES GREATER THAN 11 CHARACTERS!

Key Controls:
A - Do note
S - Re note
D - Mi note
F - Fa note
G - Sol note
H - La note
J - Si note
K - Octave-Do note

Good luck.
