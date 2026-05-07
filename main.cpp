#include "chip8.h"
#include "graphics.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cerr << "Usage: " << argv[0] << " Scale Delay ROM\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);

    char const* romFilename = argv[3];

    Platform platform("Chip-8 Emulator", VIDEOWIDTH * videoScale, VIDEOHEIGHT * videoScale, VIDEOWIDTH, VIDEOHEIGHT);
    Chip8 chip8;
    chip8.LoadRom(romFilename);

    int videoPitch = sizeof(chip8.video[0]) * VIDEOWIDTH;
    auto lastCycletime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit){
        quit = platform.ProcessInput(chip8.keypad);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float downtime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycletime).count();

        if(downtime > cycleDelay){
            lastCycletime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }

    }
    return 0;

}