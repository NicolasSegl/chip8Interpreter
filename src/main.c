

#include <stdbool.h>
#include <string.h>
#include <Windows.h>

#include "SDL.h"
#include "SDL_mixer.h"

#include "chip8.h"

// width and height of the SDL window in pixels
const int SDL_SCREEN_WIDTH  = 1024;
const int SDL_SCREEN_HEIGHT = 512;

// the SDL2 variables that we will use to draw to the screen, receive input, etc
SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;

// our instance of the chip8 structure object which will contain all the game's memory, registers, etc
chip8 chip8Emulator;

// these colours will define the colour scheme of the pixels
SDL_Color bgColour, pixelColour;

// the sound effect that is played when the sound timer goes off
Mix_Chunk* soundEffect = NULL;

// the frequency at which the chip8 should emulate a cycle at. the default recommended frequency is 500hz
float secondsPerEmulationCycle = 1000.0f / 500.0f; // 1000 milliseconds divided by 500Hz = 1 cycle per 2 milliseconds

// this array defines which keys on the keyboard will map to which keys on chip8's hex based keypad
Byte chip8keys[16] =
{
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    SDLK_z,
    SDLK_x,
    SDLK_c,
    SDLK_v,
};

// clears the screen to black
void clearScreen()
{
    SDL_SetRenderDrawColor(renderer, bgColour.r, bgColour.g, bgColour.b, bgColour.a);
    SDL_RenderClear(renderer);
}

// initializes SDL2 for our purposes
void initSDL(const char* nameOfWindow)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL2 failed to initialize!");
        exit(1);
    }

    // initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL2_mixer failed to initialize!");
        exit(1);
    }

    // create the window and check if there was any error in doing so
    char nameBuffer[256] = "CHIP-8 Emulator: ";
    strcat(nameBuffer, nameOfWindow);
    window = SDL_CreateWindow(nameBuffer, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_SCREEN_WIDTH, SDL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL2 window failed to be created!");
        exit(1);
    }

    // create the renderer and check if there was any error in doing so
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("SDL2 renderer failed to be created!");
        exit(1);
    }
}

// defines the colour scheme that will be used based on user input (or the lack thereof)
void setColourScheme(const char* scheme)
{
    bgColour.a    = 255;
    pixelColour.a = 255;

    if (strcmp(scheme, "retro") == 0)
    {
        bgColour.r = 0;
        bgColour.g = 0;
        bgColour.b = 0;

        pixelColour.r = 0;
        pixelColour.g = 255;
        pixelColour.b = 0;
    }
    else if (strcmp(scheme, "old") == 0)
    {
        bgColour.r = 46;
        bgColour.g = 20;
        bgColour.b = 1;

        pixelColour.r = 184;
        pixelColour.g = 116;
        pixelColour.b = 0;
    }
    else if (strcmp(scheme, "pink") == 0)
    {
        bgColour.r = 255;
        bgColour.g = 255;
        bgColour.b = 255;

        pixelColour.r = 206;
        pixelColour.g = 0;
        pixelColour.b = 209;
    }
    else // if it's none of the above, then use the default colour scheme
    {
        bgColour.r = 0;
        bgColour.g = 0;
        bgColour.b = 0;

        pixelColour.r = 255;
        pixelColour.g = 255;
        pixelColour.b = 255;
    }
}

/*
    this function is called when the draw flag has been set by the emulator
    it takes the bits that are set in the chip8's array for graphics and 
    draws them as SDL rects, and makes them an appropriate size for the window's
    width and height
*/
void drawToWindow()
{
    clearScreen();

    // set the render colour to white so that the pixels are drawn white
    SDL_SetRenderDrawColor(renderer, pixelColour.r, pixelColour.g, pixelColour.b, pixelColour.a);

    SDL_Rect pixelRect;

    // set the width and height of each individual pixel that the chip8 wants us to draw
    pixelRect.w = SDL_SCREEN_WIDTH / 64;
    pixelRect.h = SDL_SCREEN_HEIGHT / 32;

    // iterate through the 32 rows that chip8 stores in its graphis array
    for (int y = 0; y < 32; y++)
    {
        // iterate over the 64 columns that chip8 stores in its graphics array
        for (int x = 0; x < 64; x++)
        {
            // check to see if the pixel is set 
            if (chip8Emulator.pixels[x + y * 64])
            {
                // convert the pixels coordinates from chip8 to SDL screen coordinates
                pixelRect.x = x * pixelRect.w;
                pixelRect.y = y * pixelRect.h;

                // draw the rectangle to the screen
                SDL_RenderFillRect(renderer, &pixelRect);
            }
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 4)
    {
        printf("Usage is: chip8 <ROM file> <optional: colour scheme> <optional: emulation cycle frequency (in Hz)>");
        return 1;
    }

    // if the user passed in a third argument, set the colour scheme with the option they provided
    if (argc > 2)
        setColourScheme(argv[2]);
    else
        setColourScheme("default");

    // if the user has passed in a value for the delay (in milliseconds)
    if (argc == 4)
        secondsPerEmulationCycle = strtof(argv[3], NULL);

    // initialize our instance of the chip8 object
    initChip8(&chip8Emulator);

    // load the ROM file into the chip8 instance's memory
    if (!loadChip8(argv[1], &chip8Emulator))
    {
        printf("Failed to load chip, closing program\n");
        return 1;
    }
    
    initSDL(argv[1]);

    // load the media for the sound effect
    soundEffect = Mix_LoadWAV("sound.wav");
    if (soundEffect == NULL)
    {
        printf("Failed to load sound effect! If you'd like the emulator to have sound, you'll have to save your desired sound effect as \"sound.wav\" in the directory where the ROM file is found. Sorry for the inconvienience!");
    }

    // clear the screen and update it immediately after the window opens
    clearScreen();
    SDL_RenderPresent(renderer);

    // variables for timing the code

    LARGE_INTEGER frequency;  // measures the ticks per second
    LARGE_INTEGER t1, t2;     // t1 and t2 will measure the number of ticks
    LARGE_INTEGER chip8Timer; //  will ensure that the chip8 timer's update on 60Hz

    QueryPerformanceCounter(&t2);
    QueryPerformanceCounter(&chip8Timer);

    // store the ticks per second into the frequency variable
    QueryPerformanceFrequency(&frequency);

    //Mix_PlayChannel(-1, soundEffect, 1);

    bool running = true;
    while (running)
    {
        // set the t1 variable to the current number of ticks
        QueryPerformanceCounter(&t1);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
        
            else if (e.type == SDL_KEYDOWN)
            {
                for (int key = 0; key < 16; key++)
                    // if the key that was pressed down is one of the keys that chip8 uses
                    if (e.key.keysym.sym == chip8keys[key])
                        chip8Emulator.keys[key] = true;
            }

            else if (e.type == SDL_KEYUP)
            {
                for (int key = 0; key < 16; key++)
                    // if the key that was pressed down is one of the keys that chip8 uses
                    if (e.key.keysym.sym == chip8keys[key])
                        chip8Emulator.keys[key] = false;
            }
        }

        // emulate a chip8 cycle every 2 milliseconds (so roughly 500Hz)
        if ((t1.QuadPart - t2.QuadPart) * 1000.0f / frequency.QuadPart >= secondsPerEmulationCycle)
        {
            // set the t2 variable to the current number of ticks
            QueryPerformanceCounter(&t2);
            emulateChip8Cycle(&chip8Emulator);
        }

        // update the sound and delay timers of the chip8 emulator at a frequency of 60Hz
        if ((t1.QuadPart - chip8Timer.QuadPart) * 1000.0f / frequency.QuadPart >= 1000.0f / 60.0f)
        {
            QueryPerformanceCounter(&chip8Timer);
            updateChip8Timers(&chip8Emulator);
        }

        // when the sound timer has gone off
        if (chip8Emulator.soundFlag)
        {
            Mix_PlayChannel(-1, soundEffect, 1);
            chip8Emulator.soundFlag = false;
        }

        // when an opcode has come in that has indicated we need to update the screen
        if (chip8Emulator.drawFlag)
        {
            drawToWindow();

            // reset the draw flag
            chip8Emulator.drawFlag = false;

            // update the sdl screen
            SDL_RenderPresent(renderer);
        }
    }

    // cleanup SDL2
    SDL_DestroyWindow(window);
    Mix_FreeChunk(soundEffect);
    Mix_Quit();
    SDL_Quit();

    return 0;
}