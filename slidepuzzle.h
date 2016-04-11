/*************************************************************************
	> File Name: slidepuzzle.h
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-04-10 日 21:58:37 CST
 ************************************************************************/

#ifndef SLIDEPUZZLE
#define SLIDEPUZZLE
#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <map>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

// Public Api
typedef int board[4][4]; // 二维数组，下标从1开始，用于存放八数码的状态
struct Status;
struct Tile;

bool success(const int &s, const int &e); // 判断两个状态是否相等
void ser2board(int s, board t);
void input(board s);
void output(const int &s);
int H(int cur, int e);
int F(const Status &s);
bool success(const int &cur, const int &e);
int serialization(const board &cur);
void ser2board(int s, board t);
bool checkvalid(const int &s, const int &e);
int run(const int &s,const int &e);
void outpath(int pid, int ss, int step);
bool get_solution(int start, int end);

void init_gui();
void quit_gui();
int getpos(int i, int j);
void clip(SDL_Texture *board, SDL_Rect *tiles,Tile *tiles_pos);
void draw_board(SDL_Texture *board, SDL_Rect *tiles, Tile *tiles_pos);
int get_tile_pos(int id, Tile *tiles_pos);
void debug(Tile *tiles_pos);
bool pos_valid(int pos);
bool move_tile(int id, SDL_Texture *board, SDL_Rect *tiles, Tile *tiles_pos);
int keydown_target_pos(int direction, Tile *tiles_pos);
int get_solution_step();
int serialization(Tile *tiles_pos);
bool isTarget(Tile *tiles_pos);
void game();

// Core
struct Status {
    int s; // 利用一维数据存储当前状态
    int g, h; // f = g + h
    int cid, pid; // 当前状态id，其父节点id
    Status(const int s, int g, int h, int cid, int pid): s(s), g(g), h(h), cid(cid), pid(pid) {}
    bool operator<(const Status &b) const { // 根据状态排序
        return s < b.s;
    }
    bool operator==(const Status &b) const { // 重载==，find查找元素需要
        return success(s, b.s);
    }
};

// Core end
// GUI
#define WIN_WIDTH 640
#define WIN_HEIGHT 480
#define boardsize 3
#define move_speed 200
#define move_step 10
#define IMAGE "./image.png"
#define TARGET 123456780
#define EXIT 0
#define SOLUTION 1
#define SOLUTIONING 2
#define MOVING 3
#define MOVELEFT 4
#define MOVERIGHT 5
#define MOVEUP 6
#define MOVEDOWN 7
#define IDLE 8
#define RANDOM 9

struct Tile {
	SDL_Rect pos_size;
	int id;
};

// Public var
extern queue<int> solution;

#endif
