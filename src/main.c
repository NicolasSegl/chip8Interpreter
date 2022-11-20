#define SDL_MAIN_HANDLED

#include "SDL.h"

#include "chip8.h"

int main(int argc, char** argv)
{
    chip8 chip8;
    initChip8(&chip8);

    // make it so you can use this through the command line???
    SDL_Window* window; 

    return 0;
}