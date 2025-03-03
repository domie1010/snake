/* 【自学去】网站收集 http://www.zixue7.com */
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <queue>

using namespace std;

#define	WIDTH		64			// 游戏区域网格宽度
#define	HEIGHT		48			// 游戏区域网格高度
#define	ITEMSIZE	10			// 游戏元素大小
#define	CMD_A_UP	0x1			// 控制命令：游戏者 A 向上
#define	CMD_A_DOWN	0x2			// 控制命令：游戏者 A 向下
#define	CMD_A_LEFT	0x4			// 控制命令：游戏者 A 向左
#define	CMD_A_RIGHT	0x8			// 控制命令：游戏者 A 向右
#define	CMD_B_UP	0x10		// 控制命令：游戏者 B 向上
#define	CMD_B_DOWN	0x20		// 控制命令：游戏者 B 向下
#define	CMD_B_LEFT	0x40		// 控制命令：游戏者 B 向左
#define	CMD_B_RIGHT	0x80		// 控制命令：游戏者 B 向右
#define	CMD_QUIT	0x100		// 控制命令：退出游戏

// 定义游戏元素
enum ITEM { EMPTY = 0, WALL, PLAYER_A, PLAYER_B, PLAYER_DEAD, PLAYER_A_NEXT, PLAYER_B_NEXT };

// 全局变量
ITEM	g_world[WIDTH][HEIGHT];	// 保存游戏区
POINT	g_player_a;				// 游戏者 A 的坐标
POINT	g_player_b;				// 游戏者 B 的坐标
POINT	g_offset_a;				// 游戏者 A 的移动偏移方向
POINT	g_offset_b;				// 游戏者 B 的移动偏移方向



// 绘制游戏元素
void DrawItem(int x, int y, ITEM item)
{
	switch(item)
	{
		case EMPTY:			setfillcolor(BLACK);		break;
		case WALL:			setfillcolor(LIGHTGRAY);	break;
		case PLAYER_A:		setfillcolor(BLUE);			break;
		case PLAYER_B:		setfillcolor(RED);			break;
		case PLAYER_DEAD:	setfillcolor(MAGENTA);		break;
	}
	bar(x * ITEMSIZE + 1, y * ITEMSIZE + 1, (x + 1) * ITEMSIZE - 2, (y + 1) * ITEMSIZE - 2);
	g_world[x][y] = item;
}



// 初始化游戏
void init()
{
	int x, y;

	// 绘制墙壁
	for(x = 0; x < WIDTH; x++)
	{
		DrawItem(x, 0, WALL);
		DrawItem(x, HEIGHT - 1, WALL);
	}
	for(y = 1; y < HEIGHT - 1; y++)
	{
		DrawItem(0, y, WALL);
		DrawItem(WIDTH - 1, y, WALL);
	}

	// 绘制游戏区域
	for (x = 1; x < WIDTH - 1; x++)
		for (y = 1; y < HEIGHT - 1; y++)
			DrawItem(x, y, EMPTY);

	// 随机产生两个游戏者的位置（至少间隔 5 格）
	do
	{
		g_player_a.x = rand() % (WIDTH - 6) + 3;	g_player_a.y = rand() % (HEIGHT - 6) + 3;
		g_player_b.x = rand() % (WIDTH - 6) + 3;	g_player_b.y = rand() % (HEIGHT - 6) + 3;
	}while (  (g_player_b.x - g_player_a.x) * (g_player_b.x - g_player_a.x)
			+ (g_player_b.y - g_player_a.y) * (g_player_b.y - g_player_b.x) <= 25);
	// 画出两个游戏者的位置
	DrawItem(g_player_a.x, g_player_a.y, PLAYER_A);
	DrawItem(g_player_b.x, g_player_b.y, PLAYER_B);

	// 随机产生两个游戏者的移动方向
	// 该方法的原理详见：http://www.easyx.cn/skills/View.aspx?id=115
	int n;
	n = (rand() % 4) * 2 + 1;	g_offset_a.x = n / 3 - 1;	g_offset_a.y = n % 3 - 1;
	n = (rand() % 4) * 2 + 1;	g_offset_b.x = n / 3 - 1;	g_offset_b.y = n % 3 - 1;

	// 绘制 Player A 空心方块提示移动方向
	int tx = g_player_a.x + g_offset_a.x;
	int ty = g_player_a.y + g_offset_a.y;
	setcolor(BLUE);
	rectangle(tx * ITEMSIZE + 1, ty * ITEMSIZE + 1, (tx + 1) * ITEMSIZE - 2, (ty + 1) * ITEMSIZE - 2);
	// 绘制 Player B 空心方块提示移动方向
	tx = g_player_b.x + g_offset_b.x;
	ty = g_player_b.y + g_offset_b.y;
	setcolor(RED);
	rectangle(tx * ITEMSIZE + 1, ty * ITEMSIZE + 1, (tx + 1) * ITEMSIZE - 2, (ty + 1) * ITEMSIZE - 2);

	// 按确定开始游戏
	MessageBox(GetHWnd(), _T("对战贪吃蛇 游戏说明：\n\n") \
		_T("游戏目标：两条蛇，先碰到墙壁或碰到任何蛇的身体就算输。\n") \
		_T("Player A 使用 A S D W 控制蓝色小蛇移动方向。\n") \
		_T("Player B 使用上下左右控制红色小蛇移动方向。\n\n") \
		_T("点“确定”按钮开始游戏。"), _T("游戏说明"), MB_OK | MB_ICONINFORMATION);
}



// 获取用户命令
int GetCmd()
{
	// 定义两个用户的命令队列
	static queue<int> PLAYER_A_CMD;
	static queue<int> PLAYER_B_CMD;

	// 定义每次返回的命令
	int cmd = 0;

	// 处理按键
	while(_kbhit())
	{
		switch(_getch())
		{
			case  27:				cmd  = CMD_QUIT;	break;
			case 'W':	case 'w':	if (PLAYER_A_CMD.size() < 16)	PLAYER_A_CMD.push(CMD_A_UP);	break;
			case 'S':	case 's':	if (PLAYER_A_CMD.size() < 16)	PLAYER_A_CMD.push(CMD_A_DOWN);	break;
			case 'A':	case 'a':	if (PLAYER_A_CMD.size() < 16)	PLAYER_A_CMD.push(CMD_A_LEFT);	break;
			case 'D':	case 'd':	if (PLAYER_A_CMD.size() < 16)	PLAYER_A_CMD.push(CMD_A_RIGHT);	break;
			case  0 :	case 0xE0:
				switch(_getch())
				{
					case 72:		if (PLAYER_B_CMD.size() < 16)	PLAYER_B_CMD.push(CMD_B_UP);	break;
					case 80:		if (PLAYER_B_CMD.size() < 16)	PLAYER_B_CMD.push(CMD_B_DOWN);	break;
					case 75:		if (PLAYER_B_CMD.size() < 16)	PLAYER_B_CMD.push(CMD_B_LEFT);	break;
					case 77:		if (PLAYER_B_CMD.size() < 16)	PLAYER_B_CMD.push(CMD_B_RIGHT);	break;
				}
		}
	}

	// 读取 Player A 的命令
	int c = 0;
	while(!PLAYER_A_CMD.empty())
	{
		c = PLAYER_A_CMD.front();
		PLAYER_A_CMD.pop();
		if ((c == CMD_A_UP	 || c == CMD_A_DOWN)  && g_offset_a.x != 0)	break;
		if ((c == CMD_A_LEFT || c == CMD_A_RIGHT) && g_offset_a.y != 0)	break;
	}
	if (c != 0)
		cmd |= c;

	// 读取 Player B 的命令
	c = 0;
	while(!PLAYER_B_CMD.empty())
	{
		c = PLAYER_B_CMD.front();
		PLAYER_B_CMD.pop();
		if ((c == CMD_B_UP	 || c == CMD_B_DOWN)  && g_offset_b.x != 0)	break;
		if ((c == CMD_B_LEFT || c == CMD_B_RIGHT) && g_offset_b.y != 0)	break;
	}
	if (c != 0)	cmd |= c;

	// 返回命令
	return cmd;
}



// 处理命令
bool DealCmd(int cmd)
{
	if ((cmd & CMD_A_UP)	&& g_offset_a.x != 0)	{ g_offset_a.x = 0;		g_offset_a.y = -1;	}
	if ((cmd & CMD_A_DOWN)	&& g_offset_a.x != 0)	{ g_offset_a.x = 0;		g_offset_a.y = 1;	}
	if ((cmd & CMD_A_LEFT)	&& g_offset_a.y != 0)	{ g_offset_a.x = -1;	g_offset_a.y = 0;	}
	if ((cmd & CMD_A_RIGHT) && g_offset_a.y != 0)	{ g_offset_a.x = 1;		g_offset_a.y = 0;	}
	if ((cmd & CMD_B_UP)	&& g_offset_b.x != 0)	{ g_offset_b.x = 0;		g_offset_b.y = -1;	}
	if ((cmd & CMD_B_DOWN)	&& g_offset_b.x != 0)	{ g_offset_b.x = 0;		g_offset_b.y = 1;	}
	if ((cmd & CMD_B_LEFT)	&& g_offset_b.y != 0)	{ g_offset_b.x = -1;	g_offset_b.y = 0;	}
	if ((cmd & CMD_B_RIGHT)	&& g_offset_b.y != 0)	{ g_offset_b.x = 1;		g_offset_b.y = 0;	}
	if (cmd & CMD_QUIT)
		if (MessageBox(GetHWnd(), _T("您要退出游戏吗？"), _T("游戏暂停"), MB_OKCANCEL) == IDOK)
			return false;

	return true;
}



// 处理游戏
bool DealGame()
{
	// Player A、B 前进
	g_player_a.x += g_offset_a.x;
	g_player_a.y += g_offset_a.y;
	g_player_b.x += g_offset_b.x;
	g_player_b.y += g_offset_b.y;

	// 判断 Player A、B 的生死状态
	bool dead_a = false, dead_b = false, dead_ab = false;

	if (g_player_a.x == g_player_b.x && g_player_a.y == g_player_b.y)
	{
		DrawItem(g_player_a.x, g_player_a.y, PLAYER_DEAD);
		dead_ab = true;
	}
	else if (g_world[g_player_a.x][g_player_a.y] != EMPTY)
	{
		DrawItem(g_player_a.x, g_player_a.y, PLAYER_DEAD);
		dead_a = true;
	}
	else if (g_world[g_player_b.x][g_player_b.y] != EMPTY)
	{
		DrawItem(g_player_b.x, g_player_b.y, PLAYER_DEAD);
		dead_b = true;
	}
	else
	{
		DrawItem(g_player_a.x, g_player_a.y, PLAYER_A);
		DrawItem(g_player_b.x, g_player_b.y, PLAYER_B);
		return true;
	}

	// 判断是否要重新开始
	bool restart = false;

	if (dead_ab || (dead_a && dead_b))
		restart = MessageBox(GetHWnd(), _T("Player A 和 Player B 都死了。\n要再来一局吗？"),
							_T("GAME OVER"), MB_YESNO | MB_ICONINFORMATION) == IDYES;
	else if (dead_a)
		restart = MessageBox(GetHWnd(), _T("Player A 死了。\n要再来一局吗？"),
							_T("GAME OVER"), MB_YESNO | MB_ICONINFORMATION) == IDYES;
	else if(dead_b)
		restart = MessageBox(GetHWnd(), _T("Player B 死了。\n要再来一局吗？"),
							_T("GAME OVER"), MB_YESNO | MB_ICONINFORMATION) == IDYES;

	if (restart)
	{
		init();
		return true;
	}
	else
		return false;
}



// 主函数
void main()
{
	initgraph(640, 480);
	srand((unsigned)time(NULL));

	// 初始化
	init();

	// 游戏主循环
	while(true)
	{
		int cmd = GetCmd();					// 获取用户命令
		if (!DealCmd(cmd))	break;			// 处理命令
		if (!DealGame())	break;			// 处理游戏
		Sleep(200);							// 延时
	}

	// 关闭绘图窗口
	closegraph();
}