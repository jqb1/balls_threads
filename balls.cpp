#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <vector>

enum direction{
    up,
    down,
    right,
    left,
    up_right,
    up_left,
    down_right,
    down_left
};

direction define_direction(direction);
void move_one_step(int &row, int &col, direction ball_direction);
void move_ball(int,int,int,int);

int main()
{
    
    std::srand(time(0));
    int row,col;
    initscr();			/* Start curses mode 		  */
    getmaxyx(stdscr,row,col);
    std::thread t[9];
    for(int i=0; i<10; i++){
        t[i] = std::thread(move_ball,row/2,col/2,row,col);
        sleep(2);
    }
    for(int i = 0; i<10; i++){
        t[i].join();
    }
    refresh();			/* Print it on to the real screen */
    getch();			/* Wait for user input */
    endwin();			/* End curses mode		  */
    return 0;
}

void move_ball(int row, int col, int maxy, int maxx){
    direction ball_direction = down_right;
    while(true){
        mvprintw(row,col,"o");        
        refresh();
        usleep(80000);
        mvprintw(row,col," ");
        move_one_step(row,col,ball_direction);

        if(row<=0 || row>=maxy){
            ball_direction = define_direction(ball_direction);
        }
        if(col<=0){
           int r = std::rand()%2;
           if(r == 0)
                ball_direction = down_right;
           else
                ball_direction = up_right;
        }
        if(col>=maxx){
            int r = std::rand()%2;
            if(r == 0){
                ball_direction = down_left;
                ball_direction = define_direction(ball_direction);
            }
            else{
                ball_direction = up_left;
                ball_direction = define_direction(ball_direction);
            }
        }
    }
}
void move_one_step(int &row, int &col, direction ball_direction){
    switch(ball_direction){
         case up:
            row--;
            break;        
        case down:
            row++;
            break;
        case right:
            col++;
            break;
        case left:
            col--;
            break;
        case up_left:
            row--,col--;
            break;            
        case up_right:
            row--,col++;
            break;
        case down_right:
            row++,col++;
            break;
        case down_left:
            row++,col--;
            break;

    }
}

direction define_direction(direction direction){
    switch(direction){
        case up:
            direction = down;
            break;        
        case down:
            direction = up;
            break;
        case right:
            direction = left;
            break;
        case left:
            direction = right;
            break;
        case up_left:
            direction = down_left;
            break;            
        case up_right:
            direction = down_right;
            break;
        case down_right:
            direction = up_right;
            break;
        case down_left:
            direction = up_left;
            break;
    }
    return direction;
}
