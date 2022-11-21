# chip8Emulator
A fully functional CHIP-8 interpreter written in C.

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

## Running the emulator
To run the emulator, first compile it then use the following command in a shell: .\\\<executable-name> \<ROM-file>

