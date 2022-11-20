#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"

// constants
const DoubleByte PROGRAM_MEMORY_ADDRESS = 0x200;
const DoubleByte MEMORY_SIZE            = 4096;
const DoubleByte NUM_OF_PIXELS          = 64 * 32;

#define FONTSET_SIZE 0x50
const Byte NUM_OF_REGISTERS    = 15;
const Byte NUM_OF_STACK_LEVELS = 16;
const Byte NUM_OF_KEYS         = 16;

const Byte SCREEN_WIDTH  = 64;
const Byte SCREEN_HEIGHT = 32;

/*
    the following array represents chip8's "fontset"
    this is a construct that allows for the software to draw characters to the screen via
    drawing the pixels for numbers 0-9 and letters A-F
    each character or number is 4 pixels wide and 5 pixels high
*/
const Byte fontset[FONTSET_SIZE] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// intiailizes all of the things chip8 needs to properly operate
void initChip8(chip8* chip8ptr) 
{ 
    chip8ptr->programCounter = PROGRAM_MEMORY_ADDRESS; // set the program counter to where the program's memory starts
    chip8ptr->opcode         = 0;                      // reset the opcode
    chip8ptr->indexRegister  = 0;                      // reset the index register
    chip8ptr->stackPointer   = 0;                      // reset the stack pointer

    // clear all the pixels on the display
    for (int pixel = 0; pixel < NUM_OF_PIXELS; pixel++)
    {
        chip8ptr->pixels[pixel] = 0;
    }

    // clear the stack
    for (int stackLevel = 0; stackLevel < NUM_OF_STACK_LEVELS; stackLevel++)
    {
        chip8ptr->stack[stackLevel] = 0;
    }

    // clear the registers
    for (int r = 0; r < NUM_OF_REGISTERS; r++)
    {
        chip8ptr->registers[r] = 0;
    }
    
    chip8ptr->carryRegister = 0;

    // clear the number of keys
    for (int key = 0; key < NUM_OF_KEYS; key++)
    {
        chip8ptr->keys[key] = 0;
    }

    // clear the memory
    for (int byteAddr = 0; byteAddr < MEMORY_SIZE; byteAddr++)
    {
        chip8ptr->memory[byteAddr] = 0;
    }

    // load the fontset into memory
    for (int byteAddr = 0; byteAddr < FONTSET_SIZE; byteAddr++)
    {
        chip8ptr->memory[byteAddr] = fontset[byteAddr];
    }

    chip8ptr->soundTimer = 0; // reset sound timer
    chip8ptr->delayTimer = 0; // reset delay timer

    // seed the random function from stdlib.h
    srand(time(NULL));
}

// loads a ROM file into the memory of the chip8
void loadChip8(const char* romdir, chip8* chip8ptr)
{
    printf("Loading %s...", romdir);

    // load the romdir (by reading it as binary data, hence the "rb")
    FILE* romFile = fopen(romdir, "rb");

    // if the rom file could not be opened properly, exit the function
    if (romFile == NULL)
    {
        printf("Error loading ROM file!\n");
        return;
    }

    // find the size of the file so that we can read in the proper number of bytes
    // seek to the end of the file
    fseek(romFile, 0L, SEEK_END);

    // ask for the position when at the end of the file
    int romSize = ftell(romFile);

    // seek back to the beginning of the file
    rewind(romFile);

    // see if the size of the ROM is too big to fit into chip8's memory (of 4k)
    if (4096 - 0x200 < romSize) 
    {
        printf("ROM is too big to load into chip8's 4k memory");
        return;
    }

    // read in the data in the rom into a buffer (which will later by stored into our memory buffer)
    Byte* romBuffer = (Byte*)malloc(sizeof(Byte) * romSize);
    if (romBuffer == NULL)
    {
        printf("Error allocating the memory for the ROM buffer");
        return;
    }

    // read the ROM's data into the rom buffer
    romBuffer = fread(romBuffer, sizeof(Byte), romSize, romFile);

    // set the data in the chip8's memory
    for (int addr = 0x200; addr < romSize; addr++)
    {
        // the memory for the program starts at 0x200
        chip8ptr->memory[addr] = romBuffer[addr - 0x200];
    }
    
    // cleanup 
    fclose(romFile);
    free(romBuffer);
}

