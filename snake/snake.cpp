#include <graphics.h>      // 引用图形库头文件
#include <stdio.h>
#include <math.h>

#define winw 1000
#define winh 500
#define background RGB(0xE0, 0xEE, 0xE0)
#define finfo "userinfo.txt"

typedef struct USER {
	char username[11];
	char password[11];
	unsigned int playcount;
	unsigned int maxscore;
	unsigned int timeh, timem;
	unsigned int order;
	struct USER *next;
}user;
user usr;

typedef struct SNAKE {
	int x;
	int y;
}snake;
snake snak;

typedef struct FOOD {
	int x;
	int y;
}food;
food fod;

typedef struct STRING {
	const char *tstr;
	int x, y;
	int h, w;
	int pianx, piany;
}string;

bool menu(void);
void setfont(void);
int mousehandle(string *str[], int num);
string *inittext(const char *str, int pianx, int piany);
void about(void);
void score(void);
user *readinfo(void);
char *myintcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, unsigned int iInt);
char *mystrcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, const char *cstr);
void move(void);

RECT windows;

int main(void)
{
	initgraph(winw, winh);	// 创建绘图窗口
	setbkcolor(RGB(0xE0, 0xEE, 0xE0)); //设置背景
	cleardevice();	//刷新绘图窗口
	RECT windows = { 0, 0, 1000, 500 }; //保存窗口信息
	setfont();	//设置字体
	while (true)//菜单
	{
		if (menu())
		{
			break;
		}
	}
	closegraph();		//关闭绘图窗口
	return 0;
}

bool menu(void)
{
	string *str[4];
	int ichoise, i;

	cleardevice();
	str[0] = inittext("开始游戏", 0, -120);//初始化字体
	str[1] = inittext("排行榜", 0, -40);
	str[2] = inittext("关于作者", 0, 40);
	str[3] = inittext("退出游戏", 0, 120);
	for (i = 0; i < 4; i++)
	{
		outtextxy(str[i]->x, str[i]->y, str[i]->tstr);//生成字体
	}
	ichoise = mousehandle(str, 4); //鼠标事件
	switch (ichoise)
	{
	case 0:
		break;
	case 1:
		score();
		break;
	case 2:
		about();
		break;
	case 3:
		return true;
		break;
	default:
		break;
	}
	return false;
}

bool login(void)
{
	if (InputBox(usr.username, 11, "请输入用户名", "请登录", 0, 0, 0, false)) //创建inputbox输入用户信息
	{
		InputBox(usr.password, 11, "请输入密码", "请登录", 0, 0, 0, false);
	}
	return false;
}

void setfont(void)
{
	LOGFONT font;
	gettextstyle(&font);                     // 获取当前字体设置
	font.lfHeight = 50;                      // 设置字体高度
	font.lfWeight = 30;						// 设置字体宽度
	_tcscpy_s(font.lfFaceName, "微软雅黑");    // 设置字体
	font.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&font);					// 设置字体样式
	settextcolor(RGB(0x8b, 0x2e, 0x3f));	//设置字体颜色
}

int mousehandle(string *str[], int num)
{
	MOUSEMSG mouse;
	int i;

	while (true)
	{
		for (i = 0; i < num; i++)
		{
			mouse = GetMouseMsg();
			//如果鼠标在文字上面则加大字号
			if (mouse.x >= str[i]->x && mouse.x <= str[i]->x + str[i]->w && mouse.y >= str[i]->y && mouse.y <= str[i]->y + str[i]->h)
			{
				settextstyle(60, 0, "微软雅黑");
				str[i] = inittext(str[i]->tstr, str[i]->pianx, str[i]->piany);
				outtextxy(str[i]->x, str[i]->y, str[i]->tstr);
				while ((mouse.x >= str[i]->x && mouse.x <= str[i]->x + str[i]->w && mouse.y >= str[i]->y && mouse.y <= str[i]->y + str[i]->h))
				{
					mouse = GetMouseMsg();
					if (mouse.mkLButton)//点击鼠标左键返回id
					{
						settextstyle(50, 0, "微软雅黑");
						return i;
					}
				}
				//鼠标移出文字区域，刷新背景
				clearrectangle(str[i]->x, str[i]->y, str[i]->x + str[i]->w, str[i]->y + str[i]->h);
			}
			else
			{
				//重设为默认字体
				settextstyle(50, 0, "微软雅黑");
				str[i] = inittext(str[i]->tstr, str[i]->pianx, str[i]->piany);
				outtextxy(str[i]->x, str[i]->y, str[i]->tstr);
			}
		}
	}
}

string *inittext(const char *str, int pianx, int piany)
{
	string *sstr;

	sstr->tstr = str;
	sstr->x = (winw - textwidth(str)) / 2 + pianx;
	sstr->y = (winh - textheight(str)) / 2 + piany;
	sstr->w = textwidth(str);
	sstr->h = textheight(str);
	sstr->pianx = pianx;
	sstr->piany = piany;
	return sstr;
}

void about(void)
{
	string *about[3];
	IMAGE alipay, wenxinpay;

	cleardevice();
	while (true)
	{
		loadimage(&alipay, "C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\alipay.jpg", 240, 360);
		loadimage(&wenxinpay, "C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\weixinpay.png", 240, 360);
		putimage(40, 100, &alipay);
		putimage(winw - 40 - 240, 100, &wenxinpay);
		about[0] = inittext("支持作者", 0, -120);
		outtextxy(about[0]->x, about[0]->y, about[0]->tstr);
		about[1] = inittext("一个吃土的程序猿", 0, -40);
		outtextxy(about[1]->x, about[1]->y, about[1]->tstr);
		about[2] = inittext("返回", 0, 120);
		outtextxy(about[2]->x, about[2]->y, about[2]->tstr);
		if (mousehandle(about + 2, 1) == 0)
		{
			break;
		}
	}
}

void score(void)
{
	user *head = NULL, *pp = NULL;
	char *tmp = NULL;
	unsigned int totlesize = 0, usedsize = 0;
	string *title[5];//抬头
	string *data[10][5];//十个用户的信息
	int i, j, ilen = 0;

	cleardevice();
	title[0] = inittext("用户名", -winw / 2 + 10, -winh / 2);
	title[1] = inittext("游玩次数", -winw / 2 + 200 + 10, -winh / 2);
	title[2] = inittext("游玩时间", -winw / 2 + 400 + 10, -winh / 2);
	title[3] = inittext("最高成绩", -winw / 2 + 600 + 10, -winh / 2);
	title[4] = inittext("名次", -winw / 2 + 800 + 10, -winh / 2);
	for (i = 0; i < 5; i++)
	{
		outtextxy(title[i]->x + title[i]->w / 2, title[i]->y + title[i]->h / 2, title[i]->tstr);
	}
	head = readinfo();
	pp = head;

	settextstyle(40, 0, "微软雅黑");
	while (pp != NULL)
	{
		for (i = 0; i < 2; i++)
		{	//账号
			data[i][0] = inittext(pp->username, -winw / 2 + 10, -winh / 2 + 100 + i * 50);
			//游玩次数
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->playcount);
			data[i][1] = inittext(tmp, -winw / 2 + 200 + 10, -winh / 2 + 100 + i * 50);
			//游玩时间
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->timeh);
			tmp = mystrcat(tmp, &usedsize, &totlesize, "时");
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->timem);
			tmp = mystrcat(tmp, &usedsize, &totlesize, "分");
			data[i][2] = inittext(tmp, -winw / 2 + 400 +10, -winh / 2 + 100 + i * 50);
			//最高成绩
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->maxscore);
			data[i][3] = inittext(tmp, -winw / 2 + 600 + 10, -winh / 2 + 100 + i * 50);
			//排名
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->order);
			data[i][4] = inittext(tmp, -winw / 2 + 800 + 10, -winh / 2 + 100 + i * 50);
			for (j = 0; j < 5; j++)
			{
				outtextxy(data[i][j]->x + data[i][j]->w / 2, data[i][j]->y, data[i][j]->tstr);
				getchar();
			}
			pp = pp->next;
		}
		getchar();
	}
	
}

user *readinfo(void)
{
	FILE *fp;
	user *head = NULL, *pp, *pf = NULL;

	fopen_s(&fp, finfo, "r");

	pp = (user *)malloc(sizeof(user));
	while (fscanf_s(fp, "%s %u %u %u %u %u", pp->username, _countof(pp->username), &pp->playcount, &pp->timeh, &pp->timem, &pp->maxscore, &pp->order) == 6)
	{
		if (head == NULL)
		{
			head = pp;
			pf = head;
		}
		else
		{
			pf->next = pp;
			pf = pp;
		}
		pp = (user *)malloc(sizeof(user));
	}
	if (pf != NULL)
	{
		pf->next = NULL;
	}
	free(pp);
	return head;
}

char *mystrcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, const char *cstr)
{
	if (*usedsize + strlen(cstr) + 1 > *totlesize)//内存不够
	{
		*totlesize = *usedsize + strlen(cstr) + 1;
		tmp = (char *)realloc(tmp, *totlesize);
	}
	strcpy_s(tmp + *usedsize, *totlesize - *usedsize, cstr);//拷贝字符串到指定内存
	*usedsize += strlen(cstr);
	return tmp;
}

char *myintcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, unsigned int iInt)
{
	int ilen;

	for (ilen = 1; (iInt / (int)pow(10, ilen) != 0); ilen++) { ; }//数字的位数
	if (*usedsize + ilen + 1> *totlesize) //内存不够
	{
		*totlesize = *usedsize + ilen + 1;
		tmp = (char *)realloc(tmp, *totlesize);
	}
	_itot_s(iInt, tmp + *usedsize, *totlesize - *usedsize, 10);//转为char存入目标内存
	*usedsize += ilen;
	return tmp;
}