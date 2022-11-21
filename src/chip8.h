typedef unsigned short DoubleByte;
typedef unsigned char Byte;

struct chip8
{
    /*
        buffer for the memory (chip8 uses 4kb of memory)

        (0x000-0x200) memory used for the emulator itself (storing the graphics, variables for registers, etc)
        (0x200-0xFFF) are the addresses usable by a program
    */
    Byte memory[4096];

    // to contain the values that get pushed onto the stack. chip8 has a maximum of 16 levels of stack
    DoubleByte stack[16];

    // points the item at the top of the stack
    DoubleByte stackPointer;

    // used to store the instruction fetched
    DoubleByte opcode;

    // an array with each index representing each of chip8's 15 registers (excluding the index and carry register)
    Byte registers[15];

    // represents the carry register (used for arithmetic)
    Byte carryRegister;

    // represents the index register (used for iterating through arrays and strings)
    DoubleByte indexRegister;

    // a pointer to the current instruction
    DoubleByte programCounter;

    // an array, with each element representing one of the 2048 pixels that chip8 is able to draw to
    bool pixels[64 * 32]; // 64 wide, 32 high

    // each of these are timers which count down when they contain a value higher than 0, and they do so at 60Hz
    Byte delayTimer;
    Byte soundTimer;

    // array representing chip8's hex based keypad with values 0x0-0xF (so 16 total keys to keep track of)
    bool keys[16];

    // a flag set to true when we need to update the screen
    bool drawFlag;

    // a flag set to true when the sound timer has went off
    bool soundFlag;

}; typedef struct chip8 chip8;

void initChip8(chip8* chip8ptr);
bool loadChip8(const char* romdir, chip8* chip8ptr);
void emulateChip8Cycle(chip8* chip8ptr);
void updateChip8Timers(chip8* chip8ptr);