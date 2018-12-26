#include <iostream>
#include "Chip8.h"

Chip8 c8;

int main(int argc, char** argv) {
	c8.init();
	c8.load("app.ch8");
	while(c8.getRunState())
	{
		c8.cpuCycle();
		if(c8.getDrawStatus())
		{
			c8.scrClear();
			c8.draw();
		}
		c8.onKeyPressed();
	}
	return 0;
}
