#include "Chip8.hpp"

Chip8 c8;

int main(int argc, const char * argv[]) {
    initscr();
    start_color();
    curs_set(false);
    keypad(stdscr, true);
    noecho();
    nodelay(stdscr, true);
    c8.init();
    //Example: "/Users/Admin/Desktop/C8/Chip8/Build/Products/Debug/app.ch8"
    c8.load( "/Users/Admin/Desktop/C8/Chip8/Build/Products/Debug/mmm.c8" );
    while(c8.getRunState())
    {
        if(c8.update())
        {
            if(c8.getDrawStatus())
            {
                c8.scrClear();
                c8.draw();
            }
        }
        c8.kbhit();
    } 
    endwin();
    return 0;
}
