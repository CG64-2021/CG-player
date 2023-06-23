//Code written by CG64 - 2023
//This file can be used for whatever purposes.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cg_player.h"

FILE* CG_CreateFile(char* name, header_t* hp)
{
	FILE* fp = fopen(name, "wb");
	if (fp == NULL)
	{
		return NULL;
	}
	
	//Write the identifier to header_t and file
	strncpy(hp->id, "CGSA", 4);
	hp->id[4] = '\0';
	fwrite(hp->id, sizeof(char), 4, fp);
	fwrite(&hp->num_events, sizeof(uint32_t), 1, fp);
	hp->evoffset = ftell(fp)+3;
	fwrite(&hp->evoffset, sizeof(uint16_t), 1, fp);
	
	//Alloc empty raw events
	hp->events = malloc(sizeof(uint8_t)*hp->num_events);
	if (hp->events == NULL)
	{
		printf("Error: Could not malloc raw events!\n");
		fclose(fp);
		return NULL;
	}
	
	return fp;
}

FILE* CG_OpenFile(char* name, header_t* hp)
{
	FILE* fp = fopen(name, "rb");
	if (fp == NULL)
	{
		return NULL;
	}
	
	//Read header info
	fread(hp->id, sizeof(char), 4, fp);
	hp->id[4] = '\0';
	if(strncmp(hp->id, "CGSA", 4))
	{
		printf("Error: Wrong id. You must load a valid .cg file.");
		fclose(fp);
		return NULL;
	}
	fread(&hp->num_events, sizeof(uint32_t), 1, fp);
	fread(&hp->evoffset, sizeof(uint16_t), 1, fp);
	
	//Read all raw events
	hp->events = (uint8_t*)malloc(sizeof(uint8_t)*hp->num_events);
	if (hp->events == NULL)
	{
		printf("Error: Could not malloc raw events!\n");
		fclose(fp);
		return NULL;
	}
	fseek(fp, hp->evoffset, SEEK_SET);
	fread(hp->events, sizeof(uint8_t), hp->num_events, fp);
	
	return fp;
}

void CG_WriteEvent(event_t src, uint8_t* dest)
{
	*dest = 0;
	*dest = *dest | src.notes[0];
	*dest = *dest | src.notes[1] << 1;
	*dest = *dest | src.notes[2] << 2;
	*dest = *dest | src.notes[3] << 3;
	*dest = *dest | src.notes[4] << 4;
	*dest = *dest | src.notes[5] << 5;
	*dest = *dest | src.notes[6] << 6;
	*dest = *dest | src.notes[7] << 7;
}

void CG_ReadEvent(uint8_t src, event_t* dest)
{
	dest->notes[0] = (src & 0x1) ? 1 : 0;
	dest->notes[1] = (src & 0x2) ? 1 : 0;
	dest->notes[2] = (src & 0x4) ? 1 : 0;
	dest->notes[3] = (src & 0x8) ? 1 : 0;
	dest->notes[4] = (src & 0x10)? 1 : 0;
	dest->notes[5] = (src & 0x20)? 1 : 0;
	dest->notes[6] = (src & 0x40)? 1 : 0;
	dest->notes[7] = (src & 0x80)? 1 : 0;
}

void CG_Close(header_t* hp, FILE* fp)
{
	if (hp->events) free(hp->events);
	hp->events = NULL;
	fclose(fp);
}