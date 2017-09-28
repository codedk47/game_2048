#include "2048.h"
//初始化矩阵大小
void rect_init(GAMERECT *rect, BYTE width, USHORT win)
{
	memset(rect, 0, sizeof(GAMERECT));
	rect->width = width % 16;
	rect->size = rect->width * rect->width;
	int move;
	for (int i = 0;i < rect->size;++i)
	{
		//up
		move = i - rect->width;
		rect->to[i][0] = move < 0 ? 255 : move;
		//left
		move = i % rect->width;
		rect->to[i][1] = move ? i - 1 : 255;
		//right
		move += 1;
		rect->to[i][2] = move < rect->width ? i + 1 : 255;
		//down
		move = i + rect->width;
		rect->to[i][3] = move < rect->size ? move : 255;
		//check move point
		//printf("%03d[%3d,%3d,%3d,%3d]%s", i,
		//	rect->to[i][0],
		//	rect->to[i][1],
		//	rect->to[i][2],
		//	rect->to[i][3],
		//	i % rect->width == rect->width - 1 ? "\n" : " "
		//);
	}
	rect_random_ushort(rect);
	rect_random_ushort(rect);
	rect->win = win;
}
//获得一个随机字节
static BYTE rect_random_byte()
{
	HCRYPTPROV h;
	BYTE r;
	CryptAcquireContext(&h, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	CryptGenRandom(h, 1, &r);
	CryptReleaseContext(h, 0);
	return r;
}
//取得一个随机坐标
static int rect_random_coord(GAMERECT *rect)
{
	BYTE index = 0;
	for (BYTE i = 0;i < rect->size;++i)
	{
		if (rect->arr[i] == 0)
		{
			rect->rc[index++] = i;
		}
	}
	return index > 0 ? rect->rc[rect_random_byte() % index] : -1;
}
//在坐标随机位置生成一个2或者4，2比4几率大一点
static int rect_random_ushort(GAMERECT *rect)
{
	int coord = rect_random_coord(rect);
	if (coord != -1)
	{
		rect->arr[coord] = rect_random_byte() > 150 ? 4 : 2;
	}
	return coord;
}
//递归合并
static void rect_merge(GAMERECT *rect, BYTE index, BYTE to, UINT64 *move_count)
{
	USHORT num = rect->arr[index];
	BYTE move = rect->to[index][to];
	if (num && move != 255)
	{
		USHORT dst = rect->arr[move];
		if (dst == num || dst == 0)
		{
			rect->arr[move] += num;
			rect->arr[index] = 0;
			*move_count += 1;
			if (dst == 0)
			{
				rect_merge(rect, move, to, move_count);
			}
		}
	}
}
//顺向
static UINT64 rect_each0_callback(GAMERECT *rect, BYTE to)
{
	UINT64 move_count = 0;
	for (BYTE i = 0;i < rect->size;++i)
	{
		rect_merge(rect, i, to, &move_count);
	}
	return move_count;
}
//逆向
static UINT64 rect_each1_callback(GAMERECT *rect, BYTE to)
{
	UINT64 move_count = 0;
	for (BYTE i = rect->size;--i != 255;)
	{
		rect_merge(rect, i, to, &move_count);
	}
	return move_count;
}
//移动操作控制
static int rect_move_callback(GAMERECT *rect, BYTE to, UINT64(*each)(GAMERECT *rect, BYTE to))
{
	if (each(rect, to))
	{
		for (BYTE i = 0;i < rect->size;++i)
		{
			if (rect->arr[i] >= rect->win)
			{
				return 1;
			}
		}
		rect_random_ushort(rect);
		return 0;
	}
	if (rect_random_coord(rect) != -1)
	{
		return 0;
	}
	USHORT num;
	for (BYTE i = 0;i < rect->size;++i)
	{
		num = rect->arr[i];
		for (BYTE c = 0;c < 4;++c)
		{
			if (rect->to[i][c] != 255 && rect->arr[rect->to[i][c]] == num)
			{
				return 0;
			}
		}
	}
	return -1;
}
int rect_keypress_up(GAMERECT *rect)
{
	return rect_move_callback(rect, 0, rect_each0_callback);
}
int rect_keypress_left(GAMERECT *rect)
{
	return rect_move_callback(rect, 1, rect_each0_callback);
}
int rect_keypress_right(GAMERECT *rect)
{
	return rect_move_callback(rect, 2, rect_each1_callback);
}
int rect_keypress_down(GAMERECT *rect)
{
	return rect_move_callback(rect, 3, rect_each1_callback);
}
//显示图像有点简陋
void rect_display(GAMERECT *rect)
{
	COORD pos;
	pos.X = 0;
	pos.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	for (BYTE i = 0;i < rect->size;++i)
	{
		printf("% 4d%s", rect->arr[i], i % rect->width == rect->width - 1 ? "\n\n" : " ");
	}
}

int main()
{
	GAMERECT rect;
	int status;
	rect_init(&rect, 4, 2048);
	rect_display(&rect);
	while (1) {
		
		switch (getch())//取得按键
		{
		case 72://上
			status = rect_keypress_up(&rect);
			break;
		case 75://左
			status = rect_keypress_left(&rect);
			break;
		case 77://右
			status = rect_keypress_right(&rect);
			break;
		case 80://下
			status = rect_keypress_down(&rect);
			break;
		}
		rect_display(&rect);
		if (status != 0)
		{
			printf("\n--  %s  --\n", status == 1 ? "You win!" : "Game over!");
			break;
		}
	}
	system("pause");
	return 0;
}
