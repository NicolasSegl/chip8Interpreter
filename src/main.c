

#include <stdbool.h>
#include "SDL.h"

#include "chip8.h"

// width and height of the SDL window in pixels
const int SDL_SCREEN_WIDTH  = 1024;
const int SDL_SCREEN_HEIGHT = 512;

// the SDL2 variables that we will use to draw to the screen, receive input, etc
SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;

// our instance of the chip8 structure object which will contain all the game's memory, registers, etc
chip8 chip8Emulator;

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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

// initializes SDL2 for our purposes
void initSDL()
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL2 failed to initialize!");
        return 1;
    }

    // create the window and check if there was any error in doing so
    window = SDL_CreateWindow("chip8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_SCREEN_WIDTH, SDL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL2 window failed to be created!");
        return 1;
    }

    // create the renderer and check if there was any error in doing so
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("SDL2 renderer failed to be created!");
        return 1;
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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
    if (argc != 2)
    {
        printf("Usage is: chip8 <ROM file>");
        return 1;
    }

    // initialize our instance of the chip8 object
    initChip8(&chip8Emulator);

    // load the ROM file into the chip8 instance's memory
    if (!loadChip8(argv[1], &chip8Emulator))
    {
        printf("Failed to load chip, closing program\n");
        return 1;
    }
    
    initSDL();

    // clear the screen and update it immediately after the window opens
    clearScreen();
    SDL_RenderPresent(renderer);

    // define the variables used for timing the frames
    int timeSinceLastCycle = SDL_GetTicks();

    bool running = true;
    while (running)
    {
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

        // emulate a chip8 cycle every 17 millisecond (so roughly 60 times a second)
        if (SDL_GetTicks() - timeSinceLastCycle >= 17)
        {
            emulateChip8Cycle(&chip8Emulator);
            timeSinceLastCycle = SDL_GetTicks();
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
    SDL_Quit();

    return 0;
}