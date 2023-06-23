#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include "cg_player.h"

header_t cgFileHeader;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Mix_Chunk* C_notes[8] = {NULL};

int main(int argc, char** argv)
{
	FILE* file;
	
	uint8_t option;
	char name[15];
	printf("What you want to do:\nPress 1 to record a .cg file\nPress 2 to open an existig .cg file\n\n");
	scanf("%i", &option);
	
	do
	{
		switch(option)
		{
			case 1:
				//We can't create a file before recording the events
				//So, We'll just take the name of the file.
				printf("Write the name of the .cg file\n");
				scanf("%s", name);
				strncat(name, ".cg", strlen(name));
			break;
			case 2:
				//Get the name of the existing .cg file
				printf("Write the name of the existing .cg file\n");
				scanf("%s", name);
			break;
			default:
				printf("Invalid option!\nPress 1 to record a .cg file\nPress 2 to open an existing .cg file\n\n");
				scanf("%i", &option);
		}
	}while(option > 2);
	
	//Open existing file if option ==2
	if (option == 2)
	{
		file = CG_OpenFile(name, &cgFileHeader);
		if (file == NULL)
		{
			printf("Error: could not open the existing file!\n");
			system("pause");
			return 0;
		}
		printf("Num events: %i\noffset: %X\n", cgFileHeader.num_events, cgFileHeader.evoffset);
	}
	
	//Init SDL2 video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error: Could not init SDL2! %s\n", SDL_GetError());
		system("pause");
		return 0;
	}
	//Init SDL2 mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("Error: Could not init SDL2 mixer! %s\n", Mix_GetError());
		system("pause");
		return 0;
	}
	//Init window
	window = SDL_CreateWindow("CG player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Error: Could not create window! %s", SDL_GetError());
		system("pause");
		return 0;
	}
	//Init Renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Error: Could not create renderer! %s", SDL_GetError());
		system("pause");
		return 0;
	}
	
	//Load sounds
	C_notes[0] = Mix_LoadWAV("piano-sounds/316898__jaz_the_man_2__do.wav");
	C_notes[1] = Mix_LoadWAV("piano-sounds/316908__jaz_the_man_2__re.wav");
	C_notes[2] = Mix_LoadWAV("piano-sounds/316906__jaz_the_man_2__mi.wav");
	C_notes[3] = Mix_LoadWAV("piano-sounds/316904__jaz_the_man_2__fa.wav");
	C_notes[4] = Mix_LoadWAV("piano-sounds/316912__jaz_the_man_2__sol.wav");
	C_notes[5] = Mix_LoadWAV("piano-sounds/316902__jaz_the_man_2__la.wav");
	C_notes[6] = Mix_LoadWAV("piano-sounds/316913__jaz_the_man_2__si.wav");
	C_notes[7] = Mix_LoadWAV("piano-sounds/316901__jaz_the_man_2__do-octave.wav");
	
	Boolean close=0;
	Boolean recording=0;
	Boolean playing=0;
	Boolean play_notes[8] = {0};
	event_t* events = NULL;
	if (option == 1) cgFileHeader.num_events = 1;
	uint16_t buttons = 0;
	int iterator=0;
	
	//Malloc events to play them later
	if (option == 2)
	{
		events = malloc(sizeof(event_t)*cgFileHeader.num_events);
		if (events == NULL)
		{
			printf("Error: Could not malloc events!\n");
			system("pause");
			return 0;
		}
	}
	
	//Write more things to console
	if (option == 1) printf("Press R to record a music\nPress T to stop recording\n\n");
	if (option == 2) printf("Press P to play a music\nClose the program to stop lol\n\n");
	
	while(!close)
	{
		//malloc more events while recording
		if (recording)
		{
			cgFileHeader.num_events++;
			events = realloc(events, sizeof(event_t)*cgFileHeader.num_events);
			for(int i=0; i < 8; i++)
			{
				events[cgFileHeader.num_events-1].notes[i] = 0;
			}
		}
		
		//Read events while playing
		if (playing && iterator < cgFileHeader.num_events)
		{
			CG_ReadEvent(cgFileHeader.events[iterator], &events[iterator]);
			
			for(int i=0; i < 8; i++)
			{
				if (!events[iterator].notes[i] && play_notes[i]) play_notes[i] = 0;
			}
		}
		
		//Now, check the inputs to record or play the .cg file
		SDL_Event sEvent;
		while(SDL_PollEvent(&sEvent))
		{
			if (sEvent.type == SDL_QUIT)
			{
				if (cgFileHeader.num_events > 1)
				{
					CG_Close(&cgFileHeader, file);
				}
				
				close = true;
			}
			if (sEvent.type == SDL_KEYDOWN)
			{
				switch(sEvent.key.keysym.sym)
				{
					//Init recording events
					case SDLK_r:
						if (option == 2) break;
						if (!recording) recording = true;
						printf("Recording...\n");
						break;
					//Stop recording events and write all of them to the header
					case SDLK_t:
						if (option == 2) break;
						if (recording) recording = false;
						printf("Stopped recording!\n");
						printf("Close the program to finish the file edition\n");
						
						//Create a .cg file
						file = CG_CreateFile(name, &cgFileHeader);
						
						//Write all raw events
						for(int i=0; i < cgFileHeader.num_events; i++)
						{
							CG_WriteEvent(events[i], &cgFileHeader.events[i]);
						}
						fwrite(cgFileHeader.events, sizeof(uint8_t), cgFileHeader.num_events, file);
						break;
					//Play the music to read all events
					case SDLK_p:
						if (option == 1) break;
						if (!playing) playing = true;
						printf("Playing...");
						break;
					//Notes buttons
					case SDLK_a: buttons |= 0x1; if (!playing) Mix_PlayChannel(-1, C_notes[0], 0); break;
					case SDLK_s: buttons |= 0x2; if (!playing) Mix_PlayChannel(-1, C_notes[1], 0); break; 
					case SDLK_d: buttons |= 0x4; if (!playing) Mix_PlayChannel(-1, C_notes[2], 0); break;
					case SDLK_f: buttons |= 0x8; if (!playing) Mix_PlayChannel(-1, C_notes[3], 0); break;
					case SDLK_g: buttons |= 0x10; if (!playing) Mix_PlayChannel(-1, C_notes[4], 0); break;
					case SDLK_h: buttons |= 0x20; if (!playing) Mix_PlayChannel(-1, C_notes[5], 0); break;
					case SDLK_j: buttons |= 0x40; if (!playing) Mix_PlayChannel(-1, C_notes[6], 0); break;
					case SDLK_k: buttons |= 0x80; if (!playing) Mix_PlayChannel(-1, C_notes[7], 0); break;
				}
			}
			if (sEvent.type == SDL_KEYUP)
			{
				switch(sEvent.key.keysym.sym)
				{
					//Notes buttons
					case SDLK_a: buttons &= ~(0x1); break;
					case SDLK_s: buttons &= ~(0x2); break;
					case SDLK_d: buttons &= ~(0x4); break;
					case SDLK_f: buttons &= ~(0x8); break;
					case SDLK_g: buttons &= ~(0x10); break;
					case SDLK_h: buttons &= ~(0x20); break;
					case SDLK_j: buttons &= ~(0x40); break;
					case SDLK_k: buttons &= ~(0x80); break;
				}
			}
		}
		
		//Play sounds while playing music
		if (playing)
		{
			for(int i=0; i < 8; i++)
			{
				if (events[iterator].notes[i] && !play_notes[i]) {Mix_PlayChannel(-1, C_notes[i], 0); play_notes[i] = true;}
			}
		}
		
		if (events && !playing)
		{
			if (buttons & 0x1) {events[cgFileHeader.num_events-1].notes[0] = true;}
			if (buttons & 0x2) {events[cgFileHeader.num_events-1].notes[1] = true;}
			if (buttons & 0x4) {events[cgFileHeader.num_events-1].notes[2] = true;}
			if (buttons & 0x8) {events[cgFileHeader.num_events-1].notes[3] = true;}
			if (buttons & 0x10) {events[cgFileHeader.num_events-1].notes[4] = true;}
			if (buttons & 0x20) {events[cgFileHeader.num_events-1].notes[5] = true;}
			if (buttons & 0x40) {events[cgFileHeader.num_events-1].notes[6] = true;}
			if (buttons & 0x80) {events[cgFileHeader.num_events-1].notes[7] = true;}
		}
		
		//Draw notes buttons
		SDL_Rect bNotes[8] = {
			10, 10, 50, 50,
			70, 10, 50, 50,
			130, 10, 50, 50,
			190, 10, 50, 50,
			250, 10, 50, 50,
			310, 10, 50, 50,
			370, 10, 50, 50,
			430, 10, 50, 50};
		
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);
		
		if (recording && events && cgFileHeader.num_events > 1)
		{
			for(int i=0; i < 8; i++)
			{
				int red = events[cgFileHeader.num_events-1].notes[i] == 0 ? 0xff : 0;
				int blue = events[cgFileHeader.num_events-1].notes[i] == 1 ? 0xff : 0;
				SDL_SetRenderDrawColor(renderer, red, 0, blue, 0xff);
				SDL_RenderFillRect(renderer, &bNotes[i]);
			}
		}
		if (playing && events)
		{
			for(int i=0; i < 8; i++)
			{
				int red = events[iterator].notes[i] == 0 ? 0xff : 0;
				int blue = events[iterator].notes[i] == 1 ? 0xff : 0;
				SDL_SetRenderDrawColor(renderer, red, 0, blue, 0xff);
				SDL_RenderFillRect(renderer, &bNotes[i]);
			}
		}
		
		/*if (recording && events)
		{
			printf("notes: %i %i %i %i %i %i %i %i\n", events[cgFileHeader.num_events-1].notes[0],
			events[cgFileHeader.num_events-1].notes[1], events[cgFileHeader.num_events-1].notes[2],
			events[cgFileHeader.num_events-1].notes[3], events[cgFileHeader.num_events-1].notes[4],
			events[cgFileHeader.num_events-1].notes[5], events[cgFileHeader.num_events-1].notes[6],
			events[cgFileHeader.num_events-1].notes[7]);
		}*/
		if (playing && events)
		{
			printf("notes: %i %i %i %i %i %i %i %i\n", events[iterator].notes[0],
			events[iterator].notes[1], events[iterator].notes[2],
			events[iterator].notes[3], events[iterator].notes[4],
			events[iterator].notes[5], events[iterator].notes[6],
			events[iterator].notes[7]);
		}
		
		SDL_RenderPresent(renderer);
		if (playing && iterator < cgFileHeader.num_events) iterator++;
		if (iterator >= cgFileHeader.num_events) playing = false;
		SDL_Delay(1000/120);
	}
	
	return 0;
}