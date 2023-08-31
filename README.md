# chip8Interpreter
A fully functional CHIP-8 interpreter written in C for Windows systems.

## Features
* All of CHIP-8's 35 opcodes are properly processed
* Provides graphics and input support with SDL2
* Various colour schemes
* Ability to increase or decrease the frequency of emulation cycles (so that all programs can run as intended)
* Sound support (provided that "sound.wav" is provided in the same directory as the ROM file)

## Screenshots
### Space Invaders<br />
![Space Invaders](screenshots/spaceinvaders.PNG)

### Connect 4<br />
![Connect 4](screenshots/connect4.PNG)

### Tetris<br />
![Tetris](screenshots/tetris.png)

## Running the interpreter
First, note that SDL2 and SDL2_mixer are dependencies for this interpreter. Sometimes, SDL2 can have difficulties working well with cmake, especially on windows computers. Downloads for both can be found online.

Furthermore, I had some issues using certain compilers. Linking with GCC 5.1.0 was giving me file format errors, though GCC 6.3.0 mingw32 compiles and links just fine with no errors.

Build the project by running the following commands (after cloning the repository):
>mkdir build<br/>
>cmake ..<br/>
>make<br/>

Then the following command can be run in the shell: 
>.\\\<executable-name> \<ROM-file> <optional: colour scheme> <optional: milliseconds per emulation cycle>
