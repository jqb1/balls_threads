#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <mutex>
#include <vector>
#include <chrono>
#include <utility> 

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

std::mutex mtx;
direction define_direction(direction);
void move_one_step(int &row, int &col, direction ball_direction);
void move_ball(int,int,int,int);
void wait_for_end();
void rectangle(int, int, int, int);
void bounce(int &, int &, int, int,int, int, direction &);

bool end_animation = false;
bool stop_rectangle = false;
int rect_left_up_x, rect_left_up_y;
int rect_right_down_x,rect_right_down_y ;
int main()
{
 
    std::srand(time(0));
    int row,col;
    initscr();
    curs_set(0);
    getmaxyx(stdscr,row,col);

    std::thread t_wait(wait_for_end);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread draw_rectangle(rectangle,row/2,col/2,row,col); 

    std::vector<std::thread> balls_vector; 
    int counter = 0;
    while(!end_animation){
        sleep(rand()%3+1);
        if (counter <=20){
            balls_vector.push_back(std::thread(move_ball,row/2,col/2,row,col));
            counter++;
        }
    }

    t_wait.join();
    draw_rectangle.join();

    for (int i=0; i<balls_vector.size(); ++i){
        balls_vector.at(i).join();
    }

    endwin();
    return 0;
}

void move_ball(int row, int col, int maxy, int maxx){
    int r = std::rand()%8;
    int speed = std::rand()%80+40;
    direction ball_direction = (direction)r;
    ball_direction = right;
    int minx, miny = 0;
    row+=1;
    col+=1;
    while(!end_animation){
        mtx.lock();
        mvprintw(row,col,"o"); 
        refresh();      
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mtx.lock();
        mvprintw(row,col," ");
        mtx.unlock();

        if(row > rect_left_up_y && row < rect_right_down_y && col > rect_left_up_x  && col<rect_right_down_x){ 
            maxy = rect_right_down_y;
            maxx = rect_right_down_x;
            minx = rect_left_up_x;
            miny = rect_left_up_y;
            stop_rectangle = true;
        }
        bounce(row,col,minx,miny, maxy, maxx, ball_direction);
        move_one_step(row,col,ball_direction);

   }
}
void bounce(int &row,int &col,int minx, int miny, int maxy, int maxx, direction &ball_direction){
        // mtx.lock();
        // mvprintw(10,10,"max x:%d max y:%d",maxx, maxy);
        // mtx.unlock();
        if(row<= miny || row>=maxy){
            ball_direction = define_direction(ball_direction);
        }
        if(col<=minx){
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
void wait_for_end(){
    getch();
    end_animation = true;
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
void rectangle(int row, int col, int maxy, int maxx)
{   
    direction direction = right;
    while(!end_animation){
        mtx.lock();
        mvhline(row, col,'x', 10);
        mvhline(row+5, col, 'x', 10);
        mvvline(row, col, 'x', 5);
        mvvline(row, col+10, 'x', 6);
        rect_left_up_x = col;
        rect_left_up_y = row;
        rect_right_down_x = col+10;
        rect_right_down_y = row+4;
        refresh();
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mtx.lock();
        //clean before redraw
        mvhline(row, col,' ', 10);
        mvhline(row+5, col, ' ', 10);
        mvvline(row, col, ' ', 5);
        mvvline(row, col+10, ' ', 6);
        mtx.unlock();
        if(!stop_rectangle){
            bounce(row,col, 0, 0,maxy,maxx, direction);
            move_one_step(row,col, direction);
        }
    }
}
