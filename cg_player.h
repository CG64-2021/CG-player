//Code written by CG64 - 2023
//This file can be used for anything

typedef enum {false, true} Boolean;

typedef struct
{
	Boolean notes[8]; //Do Re Mi Fa Sol La Si Do-octave
} event_t;

typedef struct
{
	char id[5]; //Identifier (CGSA)
	uint32_t num_events; //There are num_events events in this file
	uint16_t evoffset; //Offset of the first event in this file
	uint8_t* events; //Events written/read to/from the file
} header_t;

//Create a .cg file after recording a music
FILE* CG_CreateFile(char* name, header_t* hp);

//Open an existing .cg file
FILE* CG_OpenFile(char* name, header_t* hp); 

//Write events to a .cg file
void CG_WriteEvent(event_t src, uint8_t* dest);  

//Read events from a .cg file
void CG_ReadEvent(uint8_t src, event_t* dest);

//free allocated memory and Close a .cg file
void CG_Close(header_t* hp, FILE* fp); 