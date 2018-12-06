#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <ctime>
using namespace std;

void Chip8::init()
{
	srand(time(NULL));
	memClear();
	this->delayTimer = 0;
	this->soundTimer = 0;
	this->run = true;
	this->counter = 0x200;
	this->opcode = 0;
	this->index = 0;
	this->stackPointer = 0;
	this->drawStatus = false;
	this->mtx = new char*[32];
	for(int i=0; i < 32; i++)
	{
		this->mtx[i] = new char[64];
	}
	//load font
}

void Chip8::load(char* filename)
{
	fstream file;
	file.open(filename, ios::in | ios::binary | ios::ate);
	
	if(file.good())
	{
		char *buffer;
		int size = file.tellg();
		buffer = new char[size];
		file.read(buffer, size);
		
		for(int i=0; i < size; i++)
		{
			this->memory[0x200 + i] = buffer[i];
		}
		cout << "File loaded\nSize is: " << size << "B";
		cout << "\nPress any button to start";
		getch();
		scrClear();
		delete []buffer;
		file.close();
	}
	else
	{
		cout << "Cannot load selected file. Try again.";
	}
}

void Chip8::cpuCycle()
{
	opcode = this->memory[this->counter] << 8 | this->memory[this->counter + 1];
	int nnn = opcode & 0x0FFF;
	int nn = opcode & 0x00FF;
	int n = opcode & 0x000F;
	int x = this->V[opcode] & 0x0F00 >> 8;
	int y = this->V[opcode] & 0x00F0 >> 4;

	switch(opcode & 0xF000)
	{
		case 0x0000:
			switch(opcode & 0x000F)
			{
				case 0x0000:
					scrClear();
					break;
				case 0x000E:
					this->counter = this->stack[this->stackPointer--];
					break;
			}
			break;
		case 0x1000:
			this->counter = nnn;
			break;
		case 0x2000:
			this->stack[this->stackPointer++] = this->counter;
			this->counter = opcode & 0x0FFF;
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
					this->V[x] += this->V[y];
					this->V[0xF] = (this->V[x] > 0xFF) ? 1 : 0;		
					break;
				case 0x0005:
					this->V[x] -= this->V[y];
					this->V[0xF] = (this->V[x] > this->V[y]) ? 1 : 0;
					break;
				case 0x0006:
					this->V[x] >> 1;
					break;
				case 0x0007:
					this->V[x] = this->V[y] - this->V[x];
					break;
				case 0x000F:
					this->V[x] << 1;
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
			this->V[x] = rand() % 256 & nn;
			break;
		case 0xD000:
			draw();
			break;
		case 0xF000:
			switch(opcode & 0x00FF)
			{
				case 0x0007:
					this->V[x] = this->delayTimer;
					break;
				case 0x000A:
					break;
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
					break;
			}
			break;
		default:
			cout << "Unsupported operation.\n";
			this->run = false;
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
	for(int y = 0; y < 32; y++)
	{
		for(int x = 0; x < 64; x++)
		{
			cout << (char) 219;
		}
		cout << "\n";
	}
	
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
	system("cls");
}

bool Chip8::getDrawStatus()
{
	return this->drawStatus;
}

bool Chip8::getRunState()
{
	return this->run;
}

void Chip8::onKeyPressed() 
{
	if(kbhit())
	{
		char key = getch();
		
		switch(key)
		{
			case 27: //ESC
				this->run = false;
				return;
		}
	}
}

Chip8::~Chip8()
{
	for(int i=0; i < 32; i++)
		delete mtx[i];
	delete []mtx;
	cout << "Application stopped";
}
