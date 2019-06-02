#include <graphics.h>      // 引用图形库头文件
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#pragma comment(lib, "pthreadVC2.lib")//预编译引入库文件

#define winw 1000 //窗口宽度
#define winh 500 //窗口高度
#define background RGB(0xE0, 0xEE, 0xE0) //背景色
//#define background BLACK //背景色
#define titlecolor1 RED//标题颜色1
#define titlecolor2 RGB(0xff, 0x00, 0x00)//标题颜色2
#define menutextcolor RGB(0x8b, 0x2e, 0x3f)//菜单字体颜色
#define tipstextcolor RGB(0xff, 0x00, 0x00) //tips颜色
#define scoretextcolor RGB(255, 99, 71) //排行榜字体颜色
#define edittextcolor RGB(0xbf, 0xbf, 0xbf)//输入框颜色
#define finfo "userinfo.txt" //用户信息保存文件
#define key "0xE4s0n"//加解密密匙

//保存用户信息
typedef struct USER {
	char username[11];
	char password[17];
	unsigned int playcount;
	unsigned int maxscore;
	unsigned int timeh, timem;
	unsigned int order;
	struct USER *next;
}user;

//蛇的每节身体
typedef struct SNAKE {
	int x;
	int y;
	int w;
	int h;
	struct SNAKE *next;
}snake;

//食物
typedef struct FOOD {
	int x;
	int y;
	int w;
	int h;
}food;

//字符串图形
typedef struct STRING {
	const char *tstr;
	int x, y;//中心坐标
	int h, w;
	int pianx, piany;
}string;

//输入框
typedef struct EDITTEXT {
	int x, y, w, h;//左上角的坐标
}input;

bool menu(void);
void setfont(void);
int mousehandle(string *str, int num);
string inittext(const char *str, int pianx, int piany);
void about(void);
void score(void);
user *readinfo(void);
char *myintcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, unsigned int iInt);
char *mystrcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, const char *cstr);
void login(void);
void *inputthread(void *args);
int checkpassword(char **input);
char *encode(const char *buf, const long size);
int regest(char **input);
void play(void);
void move(void);

RECT windows;
bool islogin = false;
bool exitinputthread = false;
user usr;
//base64表
static const char *ALPHA_BASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main(void)
{
	initgraph(winw, winh);	// 创建绘图窗口
	windows = { 0, 0, 1000, 500 }; //保存窗口信息
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

/*
函数功能：绘制菜单
函数参数：无
函数返回值：是否退出程序
*/
bool menu(void)
{
	string str[5], title[2], slogin;
	int ichoise = -1, i;


	setbkcolor(background);//设置背景
	cleardevice();//刷新背景
	setfont();	//设置字体

	//标题1
	settextstyle(80, 0, "微软雅黑");
	settextcolor(titlecolor1);
	title[0] = inittext("贪吃蛇", -90, -200);
	outtextxy(title[0].x, title[0].y, title[0].tstr);
	//标题2
	settextstyle(40, 0, "宋体");
	settextcolor(titlecolor2);
	title[1] = inittext("—无尽版", 90, -190);
	outtextxy(title[1].x, title[1].y, title[1].tstr);
	//菜单
	settextstyle(50, 0, "微软雅黑");
	settextcolor(menutextcolor);
	str[0] = inittext("开始游戏", 0, -100);
	str[1] = inittext("排行榜", 0, -20);
	str[2] = inittext("关于作者", 0, 60);
	str[3] = inittext("退出游戏", 0, 140);
	for (i = 0; i < 4; i++)
	{
		outtextxy(str[i].x, str[i].y, str[i].tstr);
	}
	if (!islogin)//未登录则显示游客 并提示登录
	{
		slogin = inittext("游客", winw / 2 - 150, winh / 2 - 100);
		str[4] = inittext("请登录", winw / 2 - 150, winh / 2 - 50);
		outtextxy(slogin.x, slogin.y, slogin.tstr);
		outtextxy(str[4].x, str[4].y, str[4].tstr);
		ichoise = mousehandle(str, 5); //菜单选择事件
	}
	else//登录则显示用户名并欢迎
	{
		slogin = inittext(usr.username, winw / 2 - 150, winh / 2 - 100);
		str[4] = inittext("欢迎你", winw / 2 - 150, winh / 2 - 50);
		outtextxy(slogin.x, slogin.y, slogin.tstr);
		outtextxy(str[4].x, str[4].y, str[4].tstr);
		ichoise = mousehandle(str, 4); //菜单选择事件
	}
	switch (ichoise)
	{
	case 0:
		play();//开始游戏
		break;
	case 1:
		score();//排行榜
		break;
	case 2:
		about();//关于作者
		break;
	case 3:
		return true;//退出游戏
		break;
	case 4:
		login();//登录注册
		break;
	default:
		break;
	}
	return false;
}

/*
函数功能：初始化输出的字符串图形的字体
函数参数：无
函数返回值：无
*/
void setfont(void)
{
	LOGFONT font;
	gettextstyle(&font);                     // 获取当前字体设置
	font.lfHeight = 50;                      // 设置字体高度
	font.lfWeight = 30;						// 设置字体宽度
	_tcscpy_s(font.lfFaceName, "微软雅黑");    // 设置字体
	font.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&font);					// 设置字体样式
}

/*
函数功能：处理鼠标事件
函数参数：待点击的字符串头指针 以及字符串数目
函数返回值：点击的字符串的ID
*/
int mousehandle(string *str, int num)
{
	MOUSEMSG mouse;
	int i;

	while (true)
	{
		for (i = 0; i < num; i++)
		{
			if (!MouseHit())
			{
				continue;
			}
			mouse = GetMouseMsg();
			//如果鼠标在文字上面则加大字号
			if (mouse.x >= str[i].x && mouse.x <= str[i].x + str[i].w && mouse.y >= str[i].y && mouse.y <= str[i].y + str[i].h)
			{
				settextstyle(60, 0, "微软雅黑");
				setbkcolor(background);
				clearrectangle(str[i].x, str[i].y, str[i].x + str[i].w, str[i].y + str[i].h);
				settextstyle(60, 0, "微软雅黑");
				setbkcolor(background);
				str[i] = inittext(str[i].tstr, str[i].pianx, str[i].piany);
				outtextxy(str[i].x, str[i].y, str[i].tstr);
				while ((mouse.x >= str[i].x && mouse.x <= str[i].x + str[i].w && mouse.y >= str[i].y && mouse.y <= str[i].y + str[i].h))
				{
					if (!MouseHit())
					{
						continue;
					}
					mouse = GetMouseMsg();
					if (mouse.mkLButton)//点击鼠标左键返回id
					{
						settextstyle(60, 0, "微软雅黑");
						setbkcolor(background);
						clearrectangle(str[i].x, str[i].y, str[i].x + str[i].w, str[i].y + str[i].h);
						settextstyle(50, 0, "微软雅黑");
						setbkcolor(background);
						str[i] = inittext(str[i].tstr, str[i].pianx, str[i].piany);
						outtextxy(str[i].x, str[i].y, str[i].tstr);
						return i;
					}
				}
				//鼠标移出文字区域，刷新背景
				settextstyle(60, 0, "微软雅黑");
				setbkcolor(background);
				clearrectangle(str[i].x, str[i].y, str[i].x + str[i].w, str[i].y + str[i].h);
			}
			else
			{
				//重设为默认字体
				settextstyle(50, 0, "微软雅黑");
				setbkcolor(background);
				str[i] = inittext(str[i].tstr, str[i].pianx, str[i].piany);
				outtextxy(str[i].x, str[i].y, str[i].tstr);
			}
		}
	}
}

/*
函数功能：将字符串转化为图像信息
函数参数：待转字符串 相对于屏幕中央的偏移
函数返回值：字符串图形结构体
*/
string inittext(const char *str, int pianx, int piany)
{
	string sstr;

	sstr.tstr = str;
	sstr.x = (winw - textwidth(str)) / 2 + pianx;
	sstr.y = (winh - textheight(str)) / 2 + piany;
	sstr.w = textwidth(str);
	sstr.h = textheight(str);
	sstr.pianx = pianx;
	sstr.piany = piany;
	return sstr;
}

/*
函数功能：绘制关于界面
函数参数：无
函数返回值：无
*/
void about(void)
{
	string about[3];
	IMAGE alipay, wenxinpay;

	setbkcolor(background);
	cleardevice();
	while (true)
	{
		//输出图片
		loadimage(&alipay, "C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\alipay.jpg", 240, 360);
		loadimage(&wenxinpay, "C:\\Users\\0xEASONs\\Pictures\\Saved Pictures\\weixinpay.png", 240, 360);
		putimage(40, 100, &alipay);
		putimage(winw - 40 - 240, 100, &wenxinpay);

		settextstyle(50, 0, "微软雅黑");
		setfillcolor(background);
		about[0] = inittext("支持作者", 0, -120);
		outtextxy(about[0].x, about[0].y, about[0].tstr);
		about[1] = inittext("一个吃土的程序猿", 0, -40);
		outtextxy(about[1].x, about[1].y, about[1].tstr);

		about[2] = inittext("返回", 0, 120);
		outtextxy(about[2].x, about[2].y, about[2].tstr);
		if (mousehandle(about + 2, 1) == 0)//点击返回
		{
			break;
		}
	}
}

/*
函数功能：输出排行榜
函数参数：无
函数返回值：无
*/
void score(void)
{
	user *head = NULL, *pp = NULL, *pn = NULL;
	char *tmp = NULL;
	unsigned int totlesize = 0, usedsize = 0;
	string title[5];//抬头
	string data[10][5];//十个用户的信息
	string tips, back;
	int i, j, ilen = 0;

	setbkcolor(background);
	cleardevice();
	settextcolor(menutextcolor);
	title[0] = inittext("用户名", -winw / 2 + 10, -winh / 2);
	title[1] = inittext("游玩次数", -winw / 2 + 200 + 10, -winh / 2);
	title[2] = inittext("游玩时间", -winw / 2 + 400 + 10, -winh / 2);
	title[3] = inittext("最高成绩", -winw / 2 + 600 + 10, -winh / 2);
	title[4] = inittext("名次", -winw / 2 + 800 + 10, -winh / 2);
	for (i = 0; i < 5; i++)
	{
		outtextxy(title[i].x + title[i].w / 2, title[i].y + title[i].h / 2, title[i].tstr);
	}
	head = readinfo();
	pp = head;

	settextstyle(40, 0, "微软雅黑");
	settextcolor(scoretextcolor);
	while (pp != NULL)
	{
		if (pp->order <= 10)//前十输出
		{
			//账号
			data[i][0] = inittext(pp->username, -winw / 2 + 10, -winh / 2 + 80 + (pp->order - 1) * 40);
			//游玩次数
			tmp = NULL;
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->playcount);
			data[i][1] = inittext(tmp, -winw / 2 + 200 + 10, -winh / 2 + 80 + (pp->order - 1) * 40);
			//游玩时间
			tmp = NULL;
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->timeh);
			tmp = mystrcat(tmp, &usedsize, &totlesize, "时");
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->timem);
			tmp = mystrcat(tmp, &usedsize, &totlesize, "分");
			data[i][2] = inittext(tmp, -winw / 2 + 400 + 10, -winh / 2 + 80 + (pp->order - 1) * 40);
			//最高成绩
			tmp = NULL;
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->maxscore);
			data[i][3] = inittext(tmp, -winw / 2 + 600 + 10, -winh / 2 + 80 + (pp->order - 1) * 40);
			//排名
			tmp = NULL;
			totlesize = 0;
			usedsize = 0;
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->order);
			data[i][4] = inittext(tmp, -winw / 2 + 800 + 10, -winh / 2 + 80 + (pp->order - 1) * 40);
			for (j = 0; j < 5; j++)
			{
				outtextxy(data[i][j].x + data[i][j].w / 2, data[i][j].y, data[i][j].tstr);
			}
		}
		pp = pp->next;
	}
	settextcolor(tipstextcolor);
	tips = inittext("Tips:只显示前十名", 0, -winh / 2 + 80 + 400);
	outtextxy(tips.x, tips.y, tips.tstr);

	settextstyle(50, 0, "微软雅黑");
	settextcolor(menutextcolor);
	back = inittext("返回", -winw / 2 + 920, -winh / 2 + 80 + 9 * 40);
	outtextxy(back.x, back.y, back.tstr);
	mousehandle(&back, 1);
	pp = head;
	while (pp != NULL)
	{
		pn = pp->next;
		free(pp);
		pp = pn;
	}
}

/*
函数功能：从文件读取用户信息
函数参数：无
函数返回值：用户信息结构体
*/
user *readinfo(void)
{
	FILE *fp;
	user *head = NULL, *pp, *pf = NULL;

	fopen_s(&fp, finfo, "rb");

	if (fp == NULL)
	{
		MessageBox(GetHWnd(), "没有找到用户信息文件", "错误", MB_ICONERROR);
		exit(-1);
	}
	pp = (user *)malloc(sizeof(user));
	while (fscanf_s(fp, "%s %s %u %u %u %u %u", pp->username, _countof(pp->username), pp->password, _countof(pp->password), &pp->playcount, &pp->timeh, &pp->timem, &pp->maxscore, &pp->order) == 7)
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
	fclose(fp);
	return head;
}

/*
函数功能：写字符串到指定地址
函数参数：待写入内存首地址 已使用空间 总空间 待加入字符串
函数返回值：
*/
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

/*
函数功能：将int转为字符串并写入到指定地址
函数参数：待写入内存首地址 已使用空间 总空间 待加入int
函数返回值：
*/
char *myintcat(char *tmp, unsigned int *usedsize, unsigned int *totlesize, unsigned int iInt)
{
	int ilen;

	for (ilen = 1; (iInt / (int)pow(10, ilen) != 0); ilen++) { ; }//数字的位数
	if (*usedsize + ilen + 1 > *totlesize) //内存不够
	{
		*totlesize = *usedsize + ilen + 1;
		tmp = (char *)realloc(tmp, *totlesize);
	}
	_itot_s(iInt, tmp + *usedsize, *totlesize - *usedsize, 10);//转为char存入目标内存
	*usedsize += ilen;
	return tmp;
}

/*
函数功能：登录
函数参数：无
函数返回值：无
*/
void login(void)
{
	string guidstr[3], warning;
	char **cinput;//保存帐号密码
	pthread_t input_thread;
	int loginichoise = -1;

	cinput = (char **)malloc(sizeof(char *));
	pthread_create(&input_thread, NULL, inputthread, &cinput);
	Sleep(1);
	settextstyle(50, 0, "微软雅黑");
	guidstr[0] = inittext("登录", -100, 50);
	guidstr[1] = inittext("注册", 200, 50);
	guidstr[2] = inittext("返回", 50, -winh / 2 + 400);
	setbkcolor(background);
	outtextxy(guidstr[0].x, guidstr[0].y, guidstr[0].tstr);
	outtextxy(guidstr[1].x, guidstr[1].y, guidstr[1].tstr);
	outtextxy(guidstr[2].x, guidstr[2].y, guidstr[2].tstr);
	while (true)
	{
		loginichoise = mousehandle(guidstr, 3);
		switch (loginichoise)
		{
		case 0://登录
			switch (checkpassword(cinput))
			{
			case 0://登录成功
				exitinputthread = true;
				Sleep(1);
				exitinputthread = false;
				goto lable_exit;
				break;
			case 1://密码错误
				settextstyle(50, 0, "微软雅黑");
				setfillcolor(background);
				warning = inittext("密码错误", 300, -winh / 2 + 400);
				solidrectangle(686, 375, 705 + 228, 375 + 50);
				outtextxy(warning.x, warning.y, warning.tstr);
				break;
			case 2://用户不存在
				settextstyle(50, 0, "微软雅黑");
				setfillcolor(background);
				warning = inittext("用户不存在", 300, -winh / 2 + 400);
				solidrectangle(686, 375, 705 + 228, 375 + 50);
				outtextxy(warning.x, warning.y, warning.tstr);
				break;
			default:
				break;
			}
			break;
		case 1://注册
			switch (regest(cinput))
			{
			case 0:
				settextstyle(50, 0, "微软雅黑");
				setfillcolor(background);
				warning = inittext("密码长度不足", 300, -winh / 2 + 400);
				solidrectangle(686, 375, 705 + 228, 375 + 50);
				outtextxy(warning.x, warning.y, warning.tstr);
				break;
			case 1:
				settextstyle(50, 0, "微软雅黑");
				setfillcolor(background);
				warning = inittext("用户已存在", 300, -winh / 2 + 400);
				solidrectangle(686, 375, 705 + 228, 375 + 50);
				outtextxy(warning.x, warning.y, warning.tstr);
				break;
			case 2:
				exitinputthread = true;
				Sleep(1);
				exitinputthread = false;
				goto lable_exit;
				break;
			default:
				break;
			}
			break;
		case 2://返回
			exitinputthread = true;
			Sleep(1);
			exitinputthread = false;
			goto lable_exit;
			break;
		default:
			break;
		}
	}
lable_exit:
	;
}

/*
函数功能：输入框处理
函数参数：无
函数返回值：无
*/
void *inputthread(void *args)
{
	char **cinput, *password_hide;//输入的帐号，密码的明文，隐藏的密码;
	string tstr, data[2];
	int postion = 0, id = 0;
	bool linevisalable = false;//光标可见
	input iInput[2];

	cinput = *(char ***)args;
	cleardevice();
	settextstyle(50, 0, "微软雅黑");
	data[0] = inittext("帐号", -250, -50 - 100);
	data[1] = inittext("密码", -250, 50 - 100);
	outtextxy(data[0].x, data[0].y, data[0].tstr);
	outtextxy(data[1].x, data[1].y, data[1].tstr);
	iInput[0].x = iInput[1].x = data[0].x + 100;
	iInput[0].y = data[0].y;
	iInput[1].y = data[1].y;
	iInput[0].w = iInput[1].w = 500;
	iInput[0].h = iInput[1].h = data[0].h;
	//输入框背景
	setfillcolor(edittextcolor);
	solidrectangle(iInput[0].x - 2, iInput[0].y - 2, iInput[0].x + iInput[0].w + 2, iInput[0].y + iInput[0].h + 2);
	solidrectangle(iInput[1].x - 2, iInput[1].y - 2, iInput[1].x + iInput[1].w + 2, iInput[1].y + iInput[1].h + 2);
	//输入的帐号密码的内存
	cinput[0] = (char *)malloc(11 * sizeof(char));
	cinput[1] = (char *)malloc(11 * sizeof(char));
	password_hide = (char *)malloc(11 * sizeof(char));
	//设置内存为0
	memset(cinput[0], 0, 11 * sizeof(char));
	memset(cinput[1], 0, 11 * sizeof(char));
	memset(password_hide, 0, 11 * sizeof(char));

	while (true)
	{
		if (exitinputthread)
		{
			break;
		}
		//0.5秒的偶数倍且光标不可见
		if ((clock() / (CLOCKS_PER_SEC / 2) % 2) == 0 && !linevisalable)
		{
			setlinecolor(BLACK);
			if (id == 0)
			{
				settextstyle(50, 0, "微软雅黑");
				line(iInput[id].x + textwidth(cinput[id]) + 2, iInput[id].y, iInput[id].x + textwidth(cinput[id]) + 2, iInput[id].y + iInput[id].h);
			}
			else
			{
				settextstyle(50, 0, "微软雅黑");
				line(iInput[id].x + textwidth(password_hide) + 2, iInput[id].y, iInput[id].x + textwidth(password_hide) + 2, iInput[id].y + iInput[id].h);
			}
			linevisalable = true;
		}
		//0.5秒的奇数倍且光标可见
		else if ((clock() / (CLOCKS_PER_SEC / 2) % 2) == 1 && linevisalable)
		{
			setlinecolor(edittextcolor);
			if (id == 0)
			{
				settextstyle(50, 0, "微软雅黑");
				line(iInput[id].x + textwidth(cinput[id]) + 2, iInput[id].y, iInput[id].x + textwidth(cinput[id]) + 2, iInput[id].y + iInput[id].h);
			}
			else
			{
				settextstyle(50, 0, "微软雅黑");
				line(iInput[id].x + textwidth(password_hide) + 2, iInput[id].y, iInput[id].x + textwidth(password_hide) + 2, iInput[id].y + iInput[id].h);
			}
			linevisalable = false;
		}
		//按下按键
		if (_kbhit())
		{
			*(cinput[id] + postion) = _getch();
			//等于退格
			if (*(cinput[id] + postion) == 8)
			{
				if (postion >= 0)//输入的字数大于1
				{
					memset(cinput[id] + postion - 1, 0, 2 * sizeof(char));
					if (postion > 0)
					{
						postion--;
					}
					else if (id == 1 && postion == 0)//密码第一位
					{
						id--;
						memset(cinput[1], 0, sizeof(char));
						memset(cinput[0] + 9, 0, sizeof(char));
						postion = 9;
						setlinecolor(edittextcolor);//消除密码框的光标
						line(iInput[1].x + textwidth(cinput[1]) + 2, iInput[1].y, iInput[1].x + textwidth(cinput[1]) + 2, iInput[1].y + iInput[1].h);
						linevisalable = false;
					}
				}
			}
			//帐号密码输入完毕不执行以下语句
			if (!(id == 1 && postion == 9))
			{
				//非字母数字
				if (!((*(cinput[id] + postion) >= '0' && *(cinput[id] + postion) <= '9') || (*(cinput[id] + postion) >= 'a' && *(cinput[id] + postion) <= 'z') || (*(cinput[id] + postion) >= 'A' && *(cinput[id] + postion) <= 'Z')))
				{
					memset(cinput[id] + postion, 0, sizeof(char));
				}
				//字母数字
				else
				{
					postion++;
				}
				setbkcolor(edittextcolor);
				solidrectangle(iInput[id].x - 2, iInput[id].y - 2, iInput[id].x + iInput[id].w + 2, iInput[id].y + iInput[id].h + 2);
				tstr.x = iInput[id].x;
				tstr.y = iInput[id].y;
				tstr.w = textwidth(cinput[id]);
				tstr.h = textheight(cinput[id]);
			}
			//回显到屏幕
			if (id == 0)
			{
				setfillcolor(edittextcolor);
				solidrectangle(iInput[0].x - 2, iInput[0].y - 2, iInput[0].x + iInput[0].w + 2, iInput[0].y + iInput[0].h + 2);
				tstr.tstr = cinput[0];
				settextstyle(50, 0, "微软雅黑");
				setbkcolor(edittextcolor);
				outtextxy(tstr.x, tstr.y, tstr.tstr);
			}
			else
			{
				setfillcolor(edittextcolor);
				solidrectangle(iInput[1].x - 2, iInput[1].y - 2, iInput[1].x + iInput[1].w + 2, iInput[1].y + iInput[1].h + 2);
				memset(password_hide, '*', strlen(cinput[1]) * sizeof(char));
				memset(password_hide + strlen(cinput[1]), 0, sizeof(char) * (10 - strlen(cinput[1])));
				settextstyle(50, 0, "微软雅黑");
				setbkcolor(edittextcolor);
				outtextxy(tstr.x, tstr.y, password_hide);
			}
			//帐号输入完毕
			if (postion == 10 && id == 0)
			{
				id++;
				postion = 0;
			}

		}
	}
	return NULL;
}

/*
函数功能：检查帐号密码
函数参数：帐号密码
函数返回值：无
*/
int checkpassword(char **input)
{
	user *head = NULL, *pp = NULL, *pn = NULL;

	pp = head = readinfo();

	while (pp != NULL)
	{
		if (strcmp(pp->username, input[0]) == 0)
		{
			if (strcmp(encode(input[1], strlen(input[1])), pp->password) == 0)//密码正确
			{
				islogin = true;
				strcpy_s(usr.username, sizeof(usr.username), pp->username);
				strcpy_s(usr.password, sizeof(usr.password), pp->password);
				usr.playcount = pp->playcount;
				usr.order = pp->order;
				usr.timeh = pp->timeh;
				usr.timem = pp->timem;
				usr.maxscore = pp->maxscore;
				pp = head;
				while (pp != NULL)
				{
					pn = pp->next;
					free(pp);
					pp = pn;
				}
				return 0;
			}
			else//密码错误
			{
				return 1;
			}
		}
		pp = pp->next;
	}
	//没有该账号
	return 2;
}

/*
函数功能：base64加密
函数参数：待加密字符串
函数返回值：无
*/
char *encode(const char *buf, const long size) {
	int a = 0;
	int i = 0;
	char *base64Char;

	base64Char = (char *)malloc(17 * sizeof(char));
	while (i < size) {
		char b0 = buf[i++];
		char b1 = (i < size) ? buf[i++] : 0;
		char b2 = (i < size) ? buf[i++] : 0;

		int int63 = 0x3F; //  00111111
		int int255 = 0xFF; // 11111111
		base64Char[a++] = ALPHA_BASE[(b0 >> 2) & int63];
		base64Char[a++] = ALPHA_BASE[((b0 << 4) | ((b1 & int255) >> 4)) & int63];
		base64Char[a++] = ALPHA_BASE[((b1 << 2) | ((b2 & int255) >> 6)) & int63];
		base64Char[a++] = ALPHA_BASE[b2 & int63];
	}
	switch (size % 3) {
	case 1:
		base64Char[--a] = '=';
	case 2:
		base64Char[--a] = '=';
	}
	base64Char[16] = 0;
	return base64Char;
}

/*
函数功能：注册
函数参数：帐号密码
函数返回值：无
*/
int regest(char **input)
{
	user *head = NULL, *pp = NULL, *pn = NULL;
	FILE *fp;

	if (strlen(input[1]) != 10)
	{
		return 0;//密码长度不足
	}
	pp = head = readinfo();
	while (pp != NULL)
	{
		if (strcmp(pp->username, input[0]) == 0)
		{
			pp = head;
			while (pp != NULL)
			{
				pn = pp->next;
				free(pp);
				pp = pn;
			}
			return 1;//用户已存在
		}
		pp = pp->next;
	}
	pp = head;
	while (pp != NULL)
	{
		pn = pp->next;
		free(pp);
		pp = pn;
	}
	fopen_s(&fp, finfo, "a");
	if (fp == NULL)
	{
		MessageBox(GetHWnd(), "没有找到用户信息文件", "错误", MB_ICONERROR);
		exit(-1);
	}
	fprintf_s(fp, "%s %s %u %u %u %u %u\n", input[0], encode(input[1], strlen(input[1])), 0, 0, 0, 0, 99);
	fclose(fp);
	islogin = true;
	strcpy_s(usr.username, sizeof(usr.username), input[0]);
	strcpy_s(usr.password, sizeof(usr.password), input[1]);
	usr.maxscore = usr.timem = usr.timeh = usr.playcount = 0;
	usr.order = 99;

	return 2;//注册成功
}

void play(void) {}

void move(void) {}