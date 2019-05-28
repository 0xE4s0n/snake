#include <graphics.h>      // 引用图形库头文件
#include <stdio.h>
#include <tchar.h>

#define winw 1000
#define winh 500
#define background RGB(0xE0, 0xEE, 0xE0)
#define finfo "userinfo.txt"

typedef struct USER {
	TCHAR username[11];
	TCHAR password[11];
	int playcount;
	int maxscore;
	int timeh, timem, times;
	int order;
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
	const wchar_t *tstr;
	int x, y;
	int h, w;
	int pianx, piany;
}string;

bool menu(void);
void setfont(void);
int mousehandle(string *str, int num);
string inittext(const wchar_t *str, int pianx, int piany);
void about(void);
void score(void);
user *readinfo(void);
void move(void);

RECT windows;

int main(void)
{
	initgraph(winw, winh);	// 创建绘图窗口
	//setbkcolor(RGB(0xE0, 0xEE, 0xE0)); //设置背景
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
	string str[4];
	int ichoise, i;

	cleardevice();
	str[0] = inittext(_T("开始游戏"), 0, -120);//初始化字体
	str[1] = inittext(_T("排行榜"), 0, -40);
	str[2] = inittext(_T("关于作者"), 0, 40);
	str[3] = inittext(_T("退出游戏"), 0, 120);
	for (i = 0; i < 4; i++)
	{
		outtextxy(str[i].x, str[i].y, str[i].tstr);//生成字体
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
	if (InputBox(usr.username, 11, _T("请输入用户名"), _T("请登录"), 0, 0, 0, false)) //创建inputbox输入用户信息
	{
		InputBox(usr.password, 11, _T("请输入密码"), _T("请登录"), 0, 0, 0, false);
	}
	return false;
}

void setfont(void)
{
	LOGFONT font;
	gettextstyle(&font);                     // 获取当前字体设置
	font.lfHeight = 50;                      // 设置字体高度
	font.lfWeight = 30;						// 设置字体宽度
	_tcscpy_s(font.lfFaceName, _T("微软雅黑"));    // 设置字体
	font.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&font);					// 设置字体样式
	settextcolor(RGB(0x8b, 0x2e, 0x3f));	//设置字体颜色
}

int mousehandle(string *str, int num)
{
	MOUSEMSG mouse;
	int i;

	while (true)
	{
		for (i = 0; i < num; i++)
		{
			mouse = GetMouseMsg();
			//如果鼠标在文字上面则加大字号
			if (mouse.x >= str[i].x && mouse.x <= str[i].x + str[i].w && mouse.y >= str[i].y && mouse.y <= str[i].y + str[i].h)
			{
				settextstyle(60, 0, _T("微软雅黑"));
				str[i] = inittext(str[i].tstr, str[i].pianx, str[i].piany);
				outtextxy(str[i].x, str[i].y, str[i].tstr);
				while ((mouse.x >= str[i].x && mouse.x <= str[i].x + str[i].w && mouse.y >= str[i].y && mouse.y <= str[i].y + str[i].h))
				{
					mouse = GetMouseMsg();
					if (mouse.mkLButton)//点击鼠标左键返回id
					{
						settextstyle(50, 0, _T("微软雅黑"));
						return i;
					}
				}
				//鼠标移出文字区域，刷新背景
				clearrectangle(str[i].x, str[i].y, str[i].x + str[i].w, str[i].y + str[i].h);
			}
			else
			{
				//重设为默认字体
				settextstyle(50, 0, _T("微软雅黑"));
				str[i] = inittext(str[i].tstr, str[i].pianx, str[i].piany);
				outtextxy(str[i].x, str[i].y, str[i].tstr);
			}
		}
	}
}

string inittext(const wchar_t *str, int pianx, int piany)
{
	string sstr;

	sstr.tstr = (TCHAR *)malloc(sizeof(*str));
	sstr.tstr = str;
	sstr.x = (winw - textwidth(str)) / 2 + pianx;
	sstr.y = (winh - textheight(str)) / 2 + piany;
	sstr.w = textwidth(str);
	sstr.h = textheight(str);
	sstr.pianx = pianx;
	sstr.piany = piany;
	return sstr;
}

void about(void)
{
	string about[3];
	IMAGE alipay, wenxinpay;

	cleardevice();
	while (true)
	{
		loadimage(&alipay, _T("C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\alipay.jpg"), 240, 360);
		loadimage(&wenxinpay, _T("C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\weixinpay.png"), 240, 360);
		putimage(40, 100, &alipay);
		putimage(winw - 40 - 240, 100, &wenxinpay);
		about[0] = inittext(_T("支持作者"), 0, -120);
		outtextxy(about[0].x, about[0].y, about[0].tstr);
		about[1] = inittext(_T("一个吃土的程序猿"), 0, -40);
		outtextxy(about[1].x, about[1].y, about[1].tstr);
		about[2] = inittext(_T("返回"), 0, 120);
		outtextxy(about[2].x, about[2].y, about[2].tstr);
		if (mousehandle(about + 2, 1) == 0)
		{
			break;
		}
	}
}

void score(void)
{
	user *head, *pp;
	TCHAR *tmp = NULL;
	int totlesize, usedsize;
	string title[5];//抬头
	string data[10][5];//十个用户的信息
	int i, j;

	cleardevice();
	title[0] = inittext(_T("用户名"), -winw / 2, -winh / 2);
	title[1] = inittext(_T("游玩次数"), -winw / 2 + 200, -winh / 2);
	title[2] = inittext(_T("游玩时间"), -winw / 2 + 400, -winh / 2);
	title[3] = inittext(_T("最高成绩"), -winw / 2 + 600, -winh / 2);
	title[4] = inittext(_T("名次"), -winw / 2 + 800, -winh / 2);
	for (i = 0; i < 5; i++)
	{
		outtextxy(title[i].x + title[i].w / 2, title[i].y + title[i].h / 2, title[i].tstr);
	}
	head = readinfo();
	pp = head;

	while (pp != NULL)
	{
		for (i = 0; i < 1; i++)
		{	//账号
			data[i][0] = inittext(pp->username, -winw / 2, 0);
			//游玩次数
			usedsize = totlesize = sizeof(pp->playcount);
			tmp = (TCHAR *)malloc(totlesize);
			_itot_s(pp->playcount, tmp, sizeof(pp->playcount), 10);
			data[i][1] = inittext(tmp, -winw / 2, 100);
			memset(tmp, 0, totlesize);
			usedsize = 0;
			//游玩时间
			if ( usedsize + sizeof(pp->timeh) > totlesize)//时
			{
				totlesize = usedsize + sizeof(pp->timeh);
				realloc(tmp, totlesize);
			}
			_itot_s(pp->timeh, tmp + usedsize, sizeof(pp->timeh), 10);
			usedsize += sizeof(pp->timeh);
			if (usedsize + sizeof(_T("时")) > totlesize)
			{
				totlesize = usedsize + sizeof(_T("时"));
				realloc(tmp, totlesize);
			}
			_tcscat_s(tmp + usedsize, sizeof(_T("时")), _T("时"));
			usedsize += sizeof(_T("时"));
			if (usedsize + sizeof(pp->timem) > totlesize)//分
			{
				totlesize = usedsize + sizeof(pp->timem);
				realloc(tmp, totlesize);
			}
			_itot_s(pp->timem, tmp + usedsize, sizeof(pp->timem), 10);
			usedsize += sizeof(pp->timem);
			if (usedsize + sizeof(_T("分")) > totlesize)
			{
				totlesize = usedsize + sizeof(_T("分"));
				realloc(tmp, totlesize);
			}
			_tcscat_s(tmp + usedsize, sizeof(_T("分")), _T("分"));
			usedsize += sizeof(_T("分"));
			if (usedsize + sizeof(pp->times) > totlesize)//秒
			{
				totlesize = usedsize + sizeof(pp->times);
				realloc(tmp, totlesize);
			}
			_itot_s(pp->times, tmp + usedsize, sizeof(pp->times), 10);
			usedsize += sizeof(pp->times);
			if (usedsize + sizeof(_T("秒")) > totlesize)
			{
				totlesize = usedsize + sizeof(_T("秒"));
				realloc(tmp, totlesize);
			}
			_tcscat_s(tmp + usedsize, sizeof(_T("秒")), _T("秒"));
			usedsize += sizeof(_T("秒"));
			data[i][2] = inittext(tmp, -winw / 2, 200);
			//最高成绩
			_itot_s(pp->maxscore,tmp,sizeof(pp->maxscore),10);
			data[i][3] = inittext(tmp, 0, 0);
			//排名
			_itot_s(pp->order, tmp, sizeof(pp->maxscore), 10);
			data[i][4] = inittext(tmp, 0, 0);
			for (j = 0; j < 3; j++)
			{
				outtextxy(data[i][j].x + data[i][j].w / 2, data[i][j].y, data[i][j].tstr);
			}
			pp = pp->next;
		}
		//pp->, pp->order
	}
	getchar();
}

user *readinfo(void)
{
	FILE *fp;
	user *head = NULL, *pp, *pf = NULL;

	fopen_s(&fp, finfo, "r");

	pp = (user *)malloc(sizeof(user));
	while (fscanf_s(fp, "%ws %d %d %d %d %d %d", pp->username, _countof(pp->username), &pp->playcount, &pp->timeh, &pp->timem, &pp->times, &pp->maxscore, &pp->order) == 7)
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