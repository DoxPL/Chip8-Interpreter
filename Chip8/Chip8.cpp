//
//  Chip8.cpp
//  Chip8
//
//  Created by DoxPL on 16/02/2019.
//
#define BLOCK " "
#define FRAMES_PER_SEC 10
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Chip8.hpp"
#include <ncurses.h>

using namespace std;
unsigned char fontSet[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

Chip8::Chip8()
{
    //empty constructor
}

void Chip8::init()
{
    srand(static_cast<unsigned int>(time(NULL)));
    memClear();
    this->delayTimer = 0;
    this->soundTimer = 0;
    this->run = true;
    this->counter = 0x200;
    this->opcode = 0;
    this->index = 0;
    this->stackPointer = 0;
    this->drawStatus = false;
    for(int i=0; i < 16; i++)
    {
        V[i] = 0;
        stack[i] = 0;
    }
    
    memcpy(this->memory, fontSet, sizeof(fontSet));
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    printf("\e[8;32;64t"); // To set window size
    this->lastTime = clock();
}

void Chip8::load(char* filename)
{
    fstream file;
    file.open(filename, ios::in | ios::binary | ios::ate);
    
    if(file.is_open())
    {
        ifstream _file(filename, ios::binary);
        char tmp;
        for(int i = 0x200; static_cast<void>(!_file.eof()), i < 4096; i++)
        {
            _file.get(tmp);
            this->memory[i] = tmp;
        }
        _file.close();
        mvprintw(0, 0, "\nROM file loaded. Press any key to start");
        refresh();
        getch();
        scrClear();
        file.close();
    }
    else
    {
        mvprintw(0, 0, "\nCannot load selected file. Select another and try again.");
        refresh();
        getch();
        this->run = false;
    }
}

void Chip8::cpuCycle()
{
    opcode = this->memory[this->counter] << 8 | this->memory[this->counter + 1];
    int nnn = opcode & 0x0FFF;
    int nn = opcode & 0x00FF;
    int x = this->V[opcode] & 0x0F00 >> 8;
    int y = this->V[opcode] & 0x00F0 >> 4;
    this->counter += 2;
    
    /* Check opcode value in current cycle
    printw("Opcode: 0x%08x\n", opcode);
    refresh();
    getch(); */
     
    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    scrClear();
                    draw();
                    break;
                case 0x000E:
                    this->counter = this->stack[this->stackPointer--];
                    break;
                default:
                    stop(opcode);
                    break;
            }
            break;
        case 0x1000:
            this->counter = nnn;
            break;
        case 0x2000:
            this->stack[++this->stackPointer] = this->counter;
            this->counter = nnn;
            break;
        case 0x3000:
            if(this->V[x] == nn)
            {
                this->counter += 2;
            }
            break;
        case 0x4000:
            if(this->V[x] != nn)
            {
                this->counter += 2;
            }
            break;
        case 0x5000:
            if(this->V[x] == this->V[y])
            {
                this->counter += 2;
            }
            break;
        case 0x6000:
            this->V[x] = nn;
            break;
        case 0x7000:
            this->V[x] += nn;
            break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    this->V[x] = this->V[y];
                    break;
                case 0x0001:
                    this->V[x] = this->V[x] | this->V[y];
                    break;
                case 0x0002:
                    this->V[x] = this->V[x] & this->V[y];
                    break;
                case 0x0003:
                    this->V[x] = this->V[x] ^ this->V[y];
                    break;
                case 0x0004:
                    if((this->V[x] + this->V[y]) > 255)
                    {
                        this->V[0xF] = 1;
                    }
                    else
                    {
                        this->V[x] += this->V[y];
                        this->V[0xF] = 0;
                    }
                    break;
                case 0x0005:
                    this->V[x] -= this->V[y];
                    if(this->V[x] < 0)
                        this->V[x] += 256;
                    this->V[0xF] = (this->V[x] > this->V[y]) ? 1 : 0;
                    break;
                case 0x0006:
                    this->V[0xF] = ((this->V[x] & 0x1) == 1) ? 1 : 0;
                    this->V[x] >>= 1;
                    break;
                case 0x0007:
                    this->V[x] = this->V[y] - this->V[x];
                    if(this->V[x] < 0)
                        this->V[x] += 256;
                    this->V[0xF] = (this->V[y] > this->V[x]) ? 1 : 0;
                    break;
                case 0x000E:
                    this->V[0xF] = this->V[x] & 0x80; //sprawdzic
                    this->V[x] <<= 1;
                    break;
                default:
                    stop(opcode);
                    break;
            }
            break;
            
        case 0x9000:
            if(this->V[x] != this->V[y])
            {
                this->counter += 2;
            }
            break;
            
        case 0xA000:
            this->index = nnn;
            break;
        case 0xB000:
            this->counter = this->V[0] + nnn;
            break;
        case 0xC000:
            this->V[x] = (rand() % 256) & nn;
            break;
        case 0xD000:
            drawSprite();
            break;
        case 0xE000:
            switch(opcode & 0x00FF)
            {
                case 0x009E:
                    if(this->keyStatus[x] == 1)
                        this->counter += 2;
                    break;
                case 0x00A1:
                    if(this->keyStatus[x] != 1)
                        this->counter += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007:
                    this->V[x] = this->delayTimer;
                    break;
                case 0x000A:
                {
                    bool keyPressed = false;
                    for(int i = 0; i < 16; i++)
                    {
                        if(this->keyStatus[i] == 1)
                        {
                            keyPressed = true;
                            this->V[x] = i;
                        }
                    }
                    break;
                }
                case 0x0015:
                    this->delayTimer = this->V[x];
                    break;
                case 0x0018:
                    this->soundTimer = this->V[x];
                    break;
                case 0x001E:
                    this->index += this->V[x];
                    break;
                case 0x0029:
                    this->index = this->V[x] * 5;
                    break;
                case 0x0033:
                    
                    for(int i = 0; i < 3; i++)
                    {
                        this->memory[this->index + 2 - i] = this->V[x] % 10;
                        this->V[x] /= 10;
                    }
                    break;
                    
                case 0x0055:
                {
                    int v_i = 0;
                    for(int i=this->index; i < x; i++)
                    {
                        this->memory[i] = V[v_i];
                        v_i++;
                    }
                    break;
                }
                
                case 0x0065:
                {
                    int v_i = 0;
                    for(int i=this->index; i < x; i++)
                    {
                        this->V[v_i] = this->memory[i];
                        v_i++;
                    }
                    break;
                }
                default:
                    stop(opcode);
                    break;
            }
            break;
        default:
            stop(opcode);
            break;
    }
    if(this->delayTimer > 0)
    {
        this->delayTimer--;
    }
    
    if(this->soundTimer > 0)
    {
        if(this->soundTimer == 1)
        {
        }
        this->soundTimer--;
    }
}

void Chip8::draw()
{
    for(int yline = 0; yline < 32; ++yline)
    {
        for(int xline = 0; xline < 64; ++xline)
        {
            if(this->mtx[xline + (64 * yline)])
            {
                attron(COLOR_PAIR(1));
                mvprintw(yline, xline, BLOCK);
                refresh();
            }
        }
    }
    this->drawStatus = false;
}

void Chip8::drawSprite()
{
    unsigned short height = this->opcode & 0x000F;
    unsigned short pixel;
    unsigned short x = V[(this->opcode & 0x0F00) >> 8];
    unsigned short y = V[(this->opcode & 0x00F0) >> 4];
    
    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++)
    {
        pixel = this->memory[this->index + yline];
        for(int xline = 0; xline < 8; xline++)
        {
            if((pixel & (0x80 >> xline)) != 0)
            {
                int location = x + xline + (y + yline) * 64;
                if(mtx[location] == 1)
                    V[0xF] = 1;
                mtx[location] ^= 1;
            }
        }
    }
    this->drawStatus = true;
}

void Chip8::memClear()
{
    for(int i=0; i < 4096; i++)
    {
        this->memory[i] = 0;
    }
    
    for(int i=0; i < 16; i++)
    {
        this->V[i] = 0;
    }
}

void Chip8::scrClear()
{
    clear();
    refresh();
}

void Chip8::stop(short opcode)
{
    scrClear();
    printw("Unsupported operation: 0x%08x\n", opcode);
    this->run = false;
}

bool Chip8::getDrawStatus()
{
    return this->drawStatus;
}

bool Chip8::getRunState()
{
    return this->run;
}

void Chip8::keyPressed(int key)
{
    switch(key)
    {
        case 27: //ESC
            this->run = false;
            return;
        case 49: //1 -> 1
            this->keyStatus[0] = 1;
            break;
        case 50: //2 -> 2
            this->keyStatus[1] = 1;
            break;
        case 51: //3 -> 3
            this->keyStatus[2] = 1;
            break;
        case 52: //4 -> C
            this->keyStatus[3] = 1;
            break;
        case 113: //Q -> 4
            this->keyStatus[4] = 1;
            break;
        case 119: //W -> 5
            this->keyStatus[5] = 1;
            break;
        case 101: //E -> 6
            this->keyStatus[6] = 1;
            break;
        case 114: //R -> D
            this->keyStatus[7] = 1;
            break;
        case 97: //A -> 7
            this->keyStatus[8] = 1;
            break;
        case 115: //S -> 8
            this->keyStatus[9] = 1;
            break;
        case 100: //D -> 9
            this->keyStatus[10] = 1;
            break;
        case 102: //F -> E
            this->keyStatus[11] = 1;
            break;
        case 122: //Z -> A
            this->keyStatus[12] = 1;
            break;
        case 120: //X -> 0
            this->keyStatus[13] = 1;
            break;
        case 99: //C -> B
            this->keyStatus[14] = 1;
            break;
        case 118: //V -> F
            this->keyStatus[15] = 1;
            break;
        default:
            break;
    }
}

void Chip8::kbhit()
{
    int input = getch();
    if(input != ERR)
    {
        ungetch(input);
        keyPressed(input);
    }
}

bool Chip8::update()
{
    clock_t currentTime = clock();
    if((currentTime - this->lastTime) >= (200000 / FRAMES_PER_SEC))
    {
        cpuCycle();
        this->lastTime = currentTime;
        return true;
    }
    return false;
}


Chip8::~Chip8()
{
    printw("Application terminated. Press any key to exit");
    refresh();
    getch();
}

