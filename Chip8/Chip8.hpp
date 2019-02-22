//
//  Chip8.hpp
//  Chip8
//
//  Created by Admin on 16/02/2019.
//  Copyright © 2019 Admin. All rights reserved.
//

#ifndef Chip8_hpp
#define Chip8_hpp
#define HEIGHT 32
#define WIDTH 64

#include <stdio.h>
#include <curses.h>
#include <unistd.h>

#endif /* Chip8_hpp */

class Chip8
{
private:
    unsigned short stack[16];
    unsigned short opcode;
    unsigned short stackPointer;
    unsigned short counter;
    unsigned short index;
    unsigned short delayTimer;
    unsigned short soundTimer;
    unsigned short keyStatus[16];
    unsigned short V[16];
    unsigned char memory[4096];
    bool drawStatus;
    bool run;
    char mtx[HEIGHT * WIDTH];
public:
    Chip8();
    void init();
    void load(char* filename);
    void draw();
    void drawSprite();
    void cpuCycle();
    void memClear();
    void scrClear();
    void onKeyPressed();
    void stop(short opcode);
    bool getDrawStatus();
    bool getRunState();
    ~Chip8();
};
