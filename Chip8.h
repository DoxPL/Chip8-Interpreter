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
		unsigned char V[16];
		unsigned char memory[4096];
		bool drawStatus;
		bool run;
		int x, y;
		char mtx[64 * 32];
	public:
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
