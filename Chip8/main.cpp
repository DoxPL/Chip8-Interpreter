#include "Chip8.hpp"

Chip8 c8;

int main(int argc, const char * argv[]) {
    initscr();
    curs_set(false);
    keypad(stdscr, true);
    noecho();
    c8.init();
    //Example: "/Users/Admin/Desktop/C8/Chip8/Build/Products/Debug/app.ch8"
    c8.load( "/Users/Admin/Desktop/C8/Chip8/Build/Products/Debug/app.ch8" );
    while(c8.getRunState())
    {
        c8.cpuCycle();
        if(c8.getDrawStatus())
        {
            c8.scrClear();
            c8.draw();
        }
        usleep(250000);
        //c8.onKeyPressed();
    } 
    endwin();
    return 0;
}
