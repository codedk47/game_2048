//By ZERONETA
#include "stdio.h"
#include "windows.h"
#include "wincrypt.h"
//游戏矩阵数据结构
typedef struct gamerect_t {
	USHORT arr[225];
	USHORT win;
	BYTE width;
	BYTE size;
	BYTE rc[225];
	BYTE to[225][4];
	UINT game_score;
	UINT move_total;
	UINT move_count;
} GAMERECT;

void rect_init(GAMERECT *rect, BYTE width, USHORT win);
int rect_keypress_up(GAMERECT *rect);
int rect_keypress_left(GAMERECT *rect);
int rect_keypress_right(GAMERECT *rect);
int rect_keypress_down(GAMERECT *rect);
void rect_display(GAMERECT *rect);
