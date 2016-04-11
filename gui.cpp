/*************************************************************************
	> File Name: gui.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-04-10 日 14:22:54 CST
 ************************************************************************/

#include "slidepuzzle.h"


SDL_Window *win;
SDL_Renderer *ren;
TTF_Font *font;
int WIN_W = WIN_WIDTH;
int WIN_H = WIN_HEIGHT;

struct {
	int stat;
	int curStep;
	int bestStep;
	int H;
	double bestScore;
	int startTicks;
	int times;
	double s;
	bool has_solution;
	bool time_update;
	bool isTarget;
} score;

void init_gui() {
	srand(time(NULL));
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		cout << "SDL_INIT error:" << SDL_GetError() << endl;
		exit(1);
	}
	win = SDL_CreateWindow("SlidePuzzle powered by netcan", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_RESIZABLE);
	if(win == NULL) {
		cout << "SDL_CreateWindow error:" << SDL_GetError() << endl;
		exit(1);
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == NULL) {
		cout << "SDL_CreateRenderer error:" << SDL_GetError() << endl;
		SDL_DestroyWindow(win);
		SDL_Quit();
		exit(1);
	}
	if(TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
		exit(1);
	}
	font = TTF_OpenFont(FONT, FONTSIZE);
	if(!font) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}
}

void set_window_icon() {
	SDL_Surface *icon;
	icon = SDL_CreateRGBSurfaceFrom((void*)win_icon, 32, 32, 24, 96, 255, 65280, 16711680, 0);
	SDL_SetWindowIcon(win, icon);
}

void quit_gui() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}

int getpos(int i, int j) {
	return i*boardsize + j;
}

void clip(SDL_Texture *board, SDL_Rect *tiles,Tile *tiles_pos) { // tiles是原图里的尺寸以及位置，tiles_pos是绘制的尺寸与位置
	if(board == NULL) {
		cout << "IMG_LoadTexture error:" << IMG_GetError() << endl;
		quit_gui();
		exit(1);
	}
	SDL_Rect image_size;
	SDL_QueryTexture(board, NULL, NULL, &image_size.w, &image_size.h);
	int clip_lenght = min(image_size.w, image_size.h) / boardsize;
	int tar_length = WIN_H / boardsize;
	for(int i=0; i<boardsize; ++i)
		for(int j=0; j<boardsize; ++j) {
			tiles_pos[getpos(i, j)].pos_size.w = tiles_pos[getpos(i, j)].pos_size.h = tar_length;
			tiles_pos[getpos(i, j)].pos_size.y = i*tar_length;
			tiles_pos[getpos(i, j)].pos_size.x = j*tar_length;
			tiles_pos[getpos(i, j)].id = (getpos(i, j) + 1) % (boardsize * boardsize);
			// printf("pos: 0 x: %d y: %d id: %d\n", tiles_pos[0].pos_size.x, tiles_pos[0].pos_size.y, tiles_pos[0].id);
			// tiles可能越界
			tiles[(getpos(i, j)+1)%9].w = tiles[getpos(i, j)].h = clip_lenght;
			tiles[(getpos(i, j)+1)%9].y = i*clip_lenght;
			tiles[(getpos(i, j)+1)%9].x = j*clip_lenght;
		}
	printf("pos: 0 x: %d y: %d id: %d\n", tiles_pos[0].pos_size.x, tiles_pos[0].pos_size.y, tiles_pos[0].id);
	debug(tiles_pos);
	printf("233\n");
}

void win_size_changed(Tile *tiles_pos) {
	int tar_length = WIN_H / boardsize;
	for(int i=0; i<boardsize; ++i)
		for(int j=0; j<boardsize; ++j) {
			tiles_pos[getpos(i, j)].pos_size.w = tiles_pos[getpos(i, j)].pos_size.h = tar_length;
			tiles_pos[getpos(i, j)].pos_size.y = i*tar_length;
			tiles_pos[getpos(i, j)].pos_size.x = j*tar_length;
		}
}

void draw_board(SDL_Texture *board, SDL_Rect *tiles, Tile *tiles_pos) {
	SDL_RenderClear(ren);
	msg();
	for(int i=0; i<boardsize; ++i)
		for(int j=0; j<boardsize; ++j)
			if(tiles_pos[getpos(i, j)].id != 0) SDL_RenderCopy(ren, board, &tiles[tiles_pos[getpos(i, j)].id], &tiles_pos[getpos(i, j)].pos_size);
	SDL_RenderPresent(ren);
};

int get_tile_pos(int id, Tile *tiles_pos) {
	int pos = -1;
	for(int i=0; i<boardsize; ++i) {
		if(pos != -1) break;
		for(int j=0; j<boardsize; ++j)
			if(tiles_pos[getpos(i, j)].id == id) {
				pos = getpos(i, j);
				break;
			}
	}
	return pos;
}


int serialization(Tile *tiles_pos) {
	int s = 0;
	for(int i=0; i<boardsize*boardsize; ++i)
		s = s*10 + tiles_pos[i].id;
	return s;
}

void score_reset(Tile *tiles_pos) {
	score.stat = serialization(tiles_pos);
	score.curStep = 0;
	score.times = 0;
	score.time_update = false;
	score.bestStep = run(serialization(tiles_pos), TARGET) - 1;
	score.H = H(serialization(tiles_pos), TARGET);
	score.has_solution = false;
	score.startTicks = SDL_GetTicks();
	score.isTarget = false;
}


void random_tiles(Tile *tiles_pos) {
	bool used[boardsize * boardsize];
	do {
		int id = -1;
		memset(used, false, sizeof(used));
		for(int i=0; i<boardsize * boardsize; ++i) {
			do id = rand() % 9;
			while(used[id]);
			used[id] = true;
			// printf("id: %d\n", id);
			tiles_pos[i].id = id;
		}
	} while(!checkvalid(serialization(tiles_pos), TARGET));
	score_reset(tiles_pos);

}

void set_tiles(Tile *tiles_pos, int s) {
	for(int i=boardsize*boardsize-1; i>=0; --i) {
		tiles_pos[i].id = s%10;
		s/=10;
	}
}

void debug(Tile *tiles_pos) {
	for(int i=0; i<boardsize*boardsize; ++i)
		printf("%d: id:%d x:%d y:%d\n", i, tiles_pos[i].id, tiles_pos[i].pos_size.x, tiles_pos[i].pos_size.y);
}

bool pos_valid(int pos) {
	if(pos >= 0 && pos < boardsize * boardsize) return true;
	else return false;
}
bool isTarget(Tile *tiles_pos) {
	return serialization(tiles_pos) == TARGET;
}

int get_solution_step() {
	int c = solution.front(); solution.pop();
	output(c);
	// output(n);
	if(!solution.size()) return -1;
	int n = solution.front();
	board cur, next;
	ser2board(c, cur);
	ser2board(n, next);
	int ni = -1, nj = -1;
	for(int i=1; i<=boardsize; ++i) {
		if(ni != -1) break;
		for(int j=1; j<=boardsize; ++j) {
			if(next[i][j] == 0) {
				ni = i;
				nj = j;
				break;
			}
		}
	}
	return cur[ni][nj];
}

// if move suceess, return true, else return false
bool move_tile(int id, SDL_Texture *board, SDL_Rect *tiles, Tile *tiles_pos) {
	if(id < 1 && id >= boardsize * boardsize)  return false;
	int from_pos = get_tile_pos(id, tiles_pos);
	int to_pos = get_tile_pos(0, tiles_pos);
	if(!(pos_valid(from_pos) && pos_valid(to_pos))) return false;

	// printf("from_pos: %d to_pos: %d\n", from_pos, to_pos);
	// printf("from_x: %d from_y: %d\n", tiles_pos[from_pos].pos_size.x, tiles_pos[from_pos].pos_size.y);
	// printf("to_x: %d to_y: %d\n", tiles_pos[to_pos].pos_size.x, tiles_pos[to_pos].pos_size.y);
	// printf("w: %d\n", tiles_pos[from_pos].pos_size.w);
	int delay = move_speed / tiles_pos[from_pos].pos_size.w;
	if(tiles_pos[from_pos].pos_size.y == tiles_pos[to_pos].pos_size.y && abs(from_pos-to_pos) == 1) { // left or right
		int direct = 0;
		if(from_pos > to_pos) direct = -1;
		else direct = 1;
		while(abs(tiles_pos[from_pos].pos_size.x - tiles_pos[to_pos].pos_size.x) > move_step) {
			// printf("fromx: %d, tox: %d\n", tiles_pos[from_pos].pos_size.x, tiles_pos[to_pos].pos_size.x);
			tiles_pos[from_pos].pos_size.x = tiles_pos[from_pos].pos_size.x + direct * move_step;
			draw_board(board, tiles, tiles_pos);
			SDL_Delay(delay);
		}
		tiles_pos[from_pos].pos_size.x = tiles_pos[to_pos].pos_size.x;
		draw_board(board, tiles, tiles_pos);
		tiles_pos[to_pos].pos_size.x = tiles_pos[to_pos].pos_size.x - direct * tiles_pos[0].pos_size.w;
		swap(tiles_pos[from_pos], tiles_pos[to_pos]);
		// score update
		++score.curStep;
		if(isTarget(tiles_pos)) score.isTarget = true;
		score.stat = serialization(tiles_pos);

		return true;
	}
	else if(tiles_pos[from_pos].pos_size.x == tiles_pos[to_pos].pos_size.x && abs(from_pos - to_pos) == 3) { // up or down
		int direct = 0;
		if(from_pos > to_pos) direct = -1;
		else direct = 1;
		while(abs(tiles_pos[from_pos].pos_size.y - tiles_pos[to_pos].pos_size.y) > move_step) { // right to left
			tiles_pos[from_pos].pos_size.y = tiles_pos[from_pos].pos_size.y + direct * move_step;
			draw_board(board, tiles, tiles_pos);
			SDL_Delay(delay);
		}
		tiles_pos[from_pos].pos_size.y = tiles_pos[to_pos].pos_size.y;
		draw_board(board, tiles, tiles_pos);
		tiles_pos[to_pos].pos_size.y =tiles_pos[to_pos].pos_size.y - direct * tiles_pos[0].pos_size.h;
		swap(tiles_pos[from_pos], tiles_pos[to_pos]);
		// score update
		++score.curStep;
		if(isTarget(tiles_pos)) score.isTarget = true;
		score.stat = serialization(tiles_pos);

		return true;
	}
	else return false;
}

int keydown_target_pos(int direction, Tile *tiles_pos) {
	int blank_pos = get_tile_pos(0, tiles_pos);
	// ++blank_pos;
	int target_pos = 0;
	switch(direction) {
		case MOVEUP:
			target_pos = blank_pos + 3;
			break;
		case MOVEDOWN:
			target_pos = blank_pos - 3;
			break;
		case MOVERIGHT:
			target_pos = blank_pos - 1;
			break;
		case MOVELEFT:
			target_pos = blank_pos + 1;
			break;
		default:
			return -1;
	}
	// printf("blank_pos: %d target_pos: %d\n", blank_pos, target_pos);
	// printf("blank_id: %d target_id: %d\n", tiles_pos[blank_pos].id, tiles_pos[target_pos].id);
	if(pos_valid(target_pos))
		return tiles_pos[target_pos].id;
	else return -1;
}

void mputs(const char *t, const SDL_Color &color, SDL_Rect *pos) {
	SDL_Surface *mesg = TTF_RenderText_Blended(font, t, color);
	pos->w = mesg->w;
	pos->h = mesg->h;
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, mesg);
	SDL_RenderCopy(ren, tex, NULL, pos);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(mesg);
	pos->y += pos->h;
}

void msg() {
	SDL_Color color = {255, 255, 255, 0};
	char text[256];
	// Status
	SDL_Rect pos = {WIN_H, 20, 0, 0};
	mputs("Status: ", color, &pos);
	// Time
	color.r = 136;
	color.g = 255;
	color.b = 104;
	mputs("Time: ", color, &pos);

	int  t;
	if(!score.isTarget) t = (score.times + SDL_GetTicks() - score.startTicks) / 1000;
	else {
		if(!score.time_update) {
			score.time_update = true;
			score.times = score.times + SDL_GetTicks() - score.startTicks;
		}
		// printf("Istarget");
		t = score.times / 1000;
	}

	sprintf(text, " %02d m %02d s", t/60, t%60);
	mputs(text, color, &pos);

	// Steps
	sprintf(text, "Step: %d", score.curStep);
	color.r = 136;
	color.g = 255;
	color.b = 104;
	mputs(text, color, &pos);
	// Score
	color.r = 221;
	color.g = 17;
	color.b = 104;
	mputs("Score: ", color, &pos);

	if(!score.isTarget && !score.has_solution) {
		if(score.curStep == 0) score.s = 0.0;
		else score.s = score.bestStep*1.0 / score.curStep;
		score.s *= score.H;
		score.s -= ((SDL_GetTicks() - score.startTicks) / 5000.0) * 0.01;
	} else
		score.s -= 1;
	if(score.s < 0) score.s = 0;

	sprintf(text, " %.2f", score.s);
	mputs(text, color, &pos);


	color.r = 221;
	color.g = 17;
	color.b = 104;
	mputs("Best Score: ", color, &pos);
	sprintf(text, " %.2f", score.bestScore);
	mputs(text, color, &pos);

	// Tips
	color.r = 136;
	color.g = 255;
	color.b = 104;
	mputs("Astar", color, &pos);
	mputs("    [Space]", color, &pos);
	if(score.isTarget) {
		color.r = 136;
		color.g = 255;
		color.b = 104;
		mputs("Reset Key:", color, &pos);
		color.r = 136;
		color.g = 255;
		color.b = 104;
		mputs("         [R]", color, &pos);
	}

	// SDL_RenderPresent(ren);
}

int max(double a, double b) {
	return a>b?a:b;
}

bool load_data() {
	FILE *fp = fopen(DATAFILE, "rb");
	if(fp == NULL) return false;
	fread(&score, sizeof(score), 1, fp);
	score.startTicks = SDL_GetTicks();
	score.time_update = false;
	printf("load stat: %d\n", score.stat);
	printf("load start ticks: %d\n", score.startTicks);
	fclose(fp);
	return true;
}

void save_data() {
	FILE *fp = fopen(DATAFILE, "wb");
	printf("save stat: %d\n", score.stat);
	printf("save start ticks: %d\n", score.startTicks);
	if(!score.time_update)
		score.times = score.times + SDL_GetTicks() - score.startTicks;
	fwrite(&score, sizeof(score), 1, fp);
	fclose(fp);
}



void game() {
	SDL_Rect tiles[boardsize * boardsize];
	Tile tiles_pos[boardsize * boardsize];
	SDL_Texture *board = IMG_LoadTexture(ren, IMAGE);
	clip(board, tiles, tiles_pos);
	debug(tiles_pos);
	SDL_Event event;
	int status = -1;
	int direction = -1;
	if(load_data())
		set_tiles(tiles_pos, score.stat);
	else random_tiles(tiles_pos);
	cout << serialization(tiles_pos) << endl;

	while(status != EXIT) { // main loop
		draw_board(board, tiles, tiles_pos);
		while(SDL_PollEvent(&event)) { // key handle
			switch(event.type) {
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_DOWN:
						case SDLK_s:
							status = MOVING;
							direction = MOVEDOWN;
							break;
						case SDLK_UP:
						case SDLK_w:
							status = MOVING;
							direction = MOVEUP;
							break;
						case SDLK_LEFT:
						case SDLK_a:
							status = MOVING;
							direction = MOVELEFT;
							break;
						case SDLK_RIGHT:
						case SDLK_d:
							status = MOVING;
							direction = MOVERIGHT;
							break;
						case SDLK_ESCAPE:
							status = EXIT;
							break;
						case SDLK_SPACE:
							status = SOLUTION;
							break;
						case SDLK_r:
							status = RANDOM;
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					switch(event.window.event) {
						case SDL_WINDOWEVENT_CLOSE:
							status = EXIT;
						break;
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							SDL_GetWindowSize(win, &WIN_W, &WIN_H);
							status = WINSIZECHANGED;
							printf("w: %d h:%d\n", WIN_W, WIN_H);
						break;
					}

			}
			// printf("%s key\n", SDL_GetKeyName(event.key.keysym.sym));
		}
		// app handle
		// printf("status: %d\n", status);
		switch(status) {
			case RANDOM:
				if(score.isTarget) {
					score.bestScore = max(max(score.bestScore, score.s), 0);
					random_tiles(tiles_pos);
					cout << serialization(tiles_pos) << endl;
					draw_board(board, tiles, tiles_pos);
				}
				status = IDLE;
				break;
			case SOLUTION:
				// printf("serialization: %d\n", serialization(tiles_pos));
				if (get_solution(serialization(tiles_pos), TARGET)) {
					score.has_solution = true;
					status = SOLUTIONING;
				}
				else status = IDLE;
				break;
			case MOVING:
				if(!score.isTarget) {
					move_tile(keydown_target_pos(direction, tiles_pos), board, tiles, tiles_pos);
					printf("direction: %d\n", direction);
					printf("target_id: %d\n", keydown_target_pos(direction, tiles_pos));
					status = IDLE;
					direction = -1;
				}
				else status = IDLE;
				break;
			case WINSIZECHANGED:
				win_size_changed(tiles_pos);
				// draw_board(board, tiles, tiles_pos);
				status = IDLE;
				break;
			case SOLUTIONING:
				int next = get_solution_step();
				printf("nextid: %d\n", next);
				if(next != -1)
					move_tile(next, board, tiles, tiles_pos);
				else  status = IDLE;
				break;
		}
	}
	save_data();


	// debug(tiles);
	// draw_board(board, tiles, tiles_pos);
	// move_tile(8, board, tiles, tiles_pos);
	// move_tile(8, board, tiles, tiles_pos);
	// move_tile(6, board, tiles, tiles_pos);
	// move_tile(6, board, tiles, tiles_pos);

	// SDL_RenderClear(ren);
	// SDL_RenderCopy(ren, board, NULL, NULL);
	// SDL_RenderPresent(ren);
	SDL_DestroyTexture(board);
}


int main(int argc, char **argv) {
	// freopen("debug.log", "w", stdout);
	init_gui();
	set_window_icon();
	game();
	quit_gui();
	return 0;
}
