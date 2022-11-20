#define SDL_MAIN_HANDLED

#include <stdbool.h>

#include "SDL.h"

#include "chip8.h"

const int SDL_SCREEN_WIDTH  = 1024;
const int SDL_SCREEN_HEIGHT = 512;

Byte chip8keys[16] =
{
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage is: chip8 <ROM file>");
        return 1;
    }

    chip8 chip8;
    initChip8(&chip8);

    if (!loadChip8(argv[1], &chip8))
    {
        printf("Failed to load chip, closing program\n");
        return 1;
    }

    // initialize sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL2 failed to initialize!");
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("chip8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_SCREEN_WIDTH, SDL_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("SDL2 window failed to be created!");
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("SDL2 renderer failed to be created!");
        return 1;
    }

    // clear the screen black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    bool running = true;

    // define the variables used for timing the frames
    int timeSinceLastCycle = SDL_GetTicks();

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
        
            // when a key is pressed down
            else if (e.type == SDL_KEYDOWN)
            {
                for (int key = 0; key < 16; key++)
                    // if the key that was pressed down is one of the keys that chip8 uses
                    if (e.key.keysym.sym == chip8keys[key])
                        chip8.keys[key] = true;
            }

            else if (e.type == SDL_KEYUP)
            {
                for (int key = 0; key < 16; key++)
                    if (e.key.keysym.sym == chip8keys[key])
                        chip8.keys[key] = false;
            }
        }

        // emulate a chip8 cycle every 10 milliseconds
        if (SDL_GetTicks() - timeSinceLastCycle >= 10)
        {
            emulateChip8Cycle(&chip8);
            timeSinceLastCycle = SDL_GetTicks();
        }

        // when an opcode has come in that has indicated we need to update the screen
        if (chip8.drawFlag)
        {
            // clear the screen black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // set the render colour to white so that the pixels are drawn white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);


            // draw all the pixels
            SDL_Rect pixelRect;
            pixelRect.w = SDL_SCREEN_WIDTH / 64;
            pixelRect.h = SDL_SCREEN_HEIGHT / 32;

            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 64; x++)
                {
                    // if the pixel is set 
                    if (chip8.pixels[x + y * 64] == 1)
                    {
                        pixelRect.x = x * pixelRect.w;
                        pixelRect.y = y * pixelRect.h;
                        SDL_RenderFillRect(renderer, &pixelRect);
                    }
                }
            }

            // reset the draw flag
            chip8.drawFlag = false;

            // update the sdl screen
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}