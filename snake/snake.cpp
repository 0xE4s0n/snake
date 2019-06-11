#include <graphics.h>      // 引用图形库头文件
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#pragma comment(lib, "pthreadVC2.lib")//预编译引入库文件

#define winw 1000 //窗口宽度
#define winh 500 //窗口高度
#define background RGB(224, 238, 224) //背景色
//#define background BLACK //背景色 
#define titlecolor1 RED//标题颜色1
#define titlecolor2 RGB(255, 0, 0)//标题颜色2
#define menutextcolor RGB(139, 46, 63)//菜单字体颜色
#define tipstextcolor RGB(255, 0, 0) //tips颜色
#define scoretextcolor RGB(255, 99, 71) //排行榜字体颜色
#define edittextcolor RGB(191, 191, 191)//输入框颜色
#define snakecolor RGB(3, 253, 10)//蛇的颜色
#define foodcolor RGB(255, 105, 119)//食物的颜色
#define scorecolor RGB(52, 250, 242)//游戏界面分数颜色
#define noticecolor RGB(255, 27, 29)//关卡提示文字颜色
#define finfo ".userinfo.data" //用户信息保存文件
#define pointw 15


//保存用户信息
typedef struct USER {
	char username[11];
	char password[17];
	unsigned int playcount;
	unsigned int maxscore;
	unsigned int timeh, timem,times;
	unsigned int order;
	struct USER *next;
}user;

//蛇的身体
typedef struct SNAKE {
	POINT **point;
	int r = pointw / 2;
	int num;
}snake;

//食物
typedef struct FOOD {
	int x;
	int y;
	int r = pointw / 2;
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

//墙
typedef struct WALL {
	POINT *point;
	int num;
}wall;

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
input *initplay(void);
void play(void);
bool isdeath(snake *snakehead, input playground, wall *Wall);
wall *initwall(int idifficult);
POINT *getwallpostion(int num);
void drawwall(wall *Wall);
void drawsnake(snake *snak);
food *drawfood(wall *Wall, snake *snak);
void drawscore(int score, int section, input *playground);
void movesnake(snake *snak, int *derection, int *headto, bool eat);
void *contrlthread(void *args);
bool iseat(snake *snak, food *fod);
void notice(int section, input *playground);
bool death(unsigned int score, unsigned int time);
void writeinfo(user *head);

RECT windows;
bool islogin = false;
bool exitinputthread = false;
bool exitcontrlthread = false;
user usr;
//base64表单
static const char *ALPHA_BASE = "ABFOghijkl01GHImnopDEXYZJfwKLMNqrs34567tuvPQRSTUVWC289+/abcdexyz";

int main(void)
{
	initgraph(winw, winh);	// 创建绘图窗口
	windows = { 0, 0, 1000, 500 }; //保存窗口信息
	FILE *fp;

	fopen_s(&fp, finfo, "r");

	if (fp == NULL)
	{
		fopen_s(&fp, finfo, "w");
	}
	fclose(fp);
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
	string about[5];
	IMAGE alipay, wenxinpay;

	setbkcolor(background);
	cleardevice();
	while (true)
	{
		//输出图片
		loadimage(&alipay, "alipay.jpg", 240, 360);
		loadimage(&wenxinpay, "weixinpay.png", 240, 360);
		putimage(40, 100, &alipay);
		putimage(winw - 40 - 240, 100, &wenxinpay);

		settextstyle(50, 0, "微软雅黑");
		setfillcolor(background);
		about[0] = inittext("支持作者", 0, -120);
		outtextxy(about[0].x, about[0].y, about[0].tstr);
		about[1] = inittext("一个吃土的程序猿", 0, -60);
		outtextxy(about[1].x, about[1].y, about[1].tstr);
		settextstyle(40, 0, "微软雅黑");
		setfillcolor(background);
		about[2] = inittext("重置密码联系作者", 0, 0);
		outtextxy(about[2].x, about[2].y, about[2].tstr);
		about[3] = inittext("QQ:1181499949", 0, 60);
		outtextxy(about[3].x, about[3].y, about[3].tstr);
		settextstyle(50, 0, "微软雅黑");
		setfillcolor(background);
		about[4] = inittext("返回", 0, 120);
		outtextxy(about[4].x, about[4].y, about[4].tstr);
		if (mousehandle(about + 4, 1) == 0)//点击返回
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
			tmp = myintcat(tmp, &usedsize, &totlesize, pp->times);
			tmp = mystrcat(tmp, &usedsize, &totlesize, "秒");
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

	fopen_s(&fp, finfo, "r");

	if (fp == NULL)
	{
		MessageBox(GetHWnd(), "没有找到用户信息文件", "错误", MB_ICONERROR);
		exit(-1);
	}
	pp = (user *)malloc(sizeof(user));
	while (fscanf_s(fp, "%s %s %u %u %u %u %u %u", pp->username, _countof(pp->username), pp->password, _countof(pp->password), &pp->playcount, &pp->timeh, &pp->timem, &pp->times, &pp->maxscore, &pp->order) == 8)
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
函数返回值：处理完的字符串的地址
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
函数返回值：处理完的字符串的地址
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
	free(cinput[0]);
	free(cinput[1]);
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
			free(password_hide);
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
				usr.times = pp->times;
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
		base64Char[--a] = '*';
	case 2:
		base64Char[--a] = '#';
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
	FILE *fp = NULL;

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
	fprintf_s(fp, "%s %s %u %u %u %u %u %u\n", input[0], encode(input[1], strlen(input[1])), 0, 0, 0, 0, 0, 99);
	fclose(fp);
	islogin = true;
	strcpy_s(usr.username, sizeof(usr.username), input[0]);
	strcpy_s(usr.password, sizeof(usr.password), encode(input[1],strlen(input[1])));
	usr.maxscore = usr.timem = usr.timeh = usr.times = usr.playcount = 0;
	usr.order = 99;

	return 2;//注册成功
}

/*
函数功能：完成游戏的处理
函数参数：无
函数返回值：无
*/
void play(void)
{
	input *playground;
	string difficult[5];
	wall *Wall;
	snake snak;
	int derection, headto;
	int kb1, kb2;
	int speed = 40;
	int idifficult;
	int i;
	pthread_t contrl_thread;
	void *args;
	bool ctrlflag = false;
	food *fod;
	bool beat = false;
	unsigned int score = 0, section = 1;
	unsigned int time0, time1, timesum = 0;

	if (!islogin)
	{
		if (IDNO == MessageBox(GetHWnd(), "以游客身份登录不会记载成绩是否继续?", "提示", MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
	}
lable_restart:
	cleardevice();
	settextstyle(50, 0, "微软雅黑");
	settextcolor(menutextcolor);
	difficult[0] = inittext("简单", 0, -winh / 2 + 145);
	difficult[1] = inittext("一般", 0, -winh / 2 + 215);
	difficult[2] = inittext("困难", 0, -winh / 2 + 285);
	difficult[3] = inittext("噩梦", 0, -winh / 2 + 355);
	difficult[4] = inittext("返回", 300, -winh / 2 + 450);
	for (i = 0; i < 5; i++)
	{
		outtextxy(difficult[i].x, difficult[i].y, difficult[i].tstr);
	}
	idifficult = mousehandle(difficult, 5);
	if (idifficult == 4)
	{
		return;
	}
	args = &ctrlflag;
	pthread_create(&contrl_thread, NULL, contrlthread, args);
lable_nextsection:
	derection = 3, headto = 3;
	playground = initplay();
	Wall = initwall(idifficult);
	drawwall(Wall);
	drawsnake(&snak);
	fod = drawfood(Wall, &snak);
	drawscore(score, section, playground);
	notice(section, playground);
	time0 = clock();
	//https://codeabc.cn/yangw/post/empty-the-keyboard-buffer-under-vc  清空键盘缓冲区 
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	while (true)
	{
		if (_kbhit())
		{
			kb1 = _getch();
			if (kb1 == 0xE0)
			{
				kb2 = _getch();
				switch (kb2)
				{
				case 0X48:
					headto = derection;
					derection = 0;
					break;
				case 0X50:
					headto = derection;
					derection = 1;
					break;
				case 0X4B:
					headto = derection;
					derection = 2;
					break;
				case 0X4D:
					headto = derection;
					derection = 3;
					break;
				default:
					break;
				}
			}
			else
			{
				switch (kb1)
				{
				case 'w':
					headto = derection;
					derection = 0;
					break;
				case 's':
					headto = derection;
					derection = 1;
					break;
				case 'a':
					headto = derection;
					derection = 2;
					break;
				case 'd':
					headto = derection;
					derection = 3;
					break;
				case 'W':
					headto = derection;
					derection = 0;
					break;
				case 'S':
					headto = derection;
					derection = 1;
					break;
				case 'A':
					headto = derection;
					derection = 2;
					break;
				case 'D':
					headto = derection;
					derection = 3;
					break;
				case ' ':
					while (_getch() != ' ')
					{
						Sleep(1);
					}
				default:
					break;
				}
			}
		}

		movesnake(&snak, &derection, &headto, beat);
		if ((beat = iseat(&snak, fod)) == true)
		{
			drawscore(++score, section, playground);
			if (score % 10 == 0)
			{
				time1 = clock();
				timesum += (time1 - time0);
				Sleep(1000);
				section++;
				speed += 5;
				goto lable_nextsection;
			}
			fod = drawfood(Wall, &snak);
		}
		if (isdeath(&snak, *playground, Wall))
		{
			time1 = clock();
			timesum += (time1 - time0);
			if (death(score, timesum))
			{
				speed = 40;
				score = 0, section = 1;
				timesum = 0;
				goto lable_restart;
			}
			else
			{
				exitcontrlthread = true;
				free(playground);
				if (Wall != NULL)
				{
					free(Wall->point);
					free(Wall);
				}
				free(fod);
				for (i = 0; i < snak.num; i++)
				{
					free(snak.point[i]);
				}
				Sleep(1);
				exitcontrlthread = false;
				break;
			}
		}
		if (ctrlflag)
		{
			Sleep(10000 / speed / 4);
		}
		else
		{
			Sleep(10000 / speed);
		}
	
	}
}

/*
函数功能：初始化游戏界面
函数参数：无
函数返回值：游戏区域
*/
input *initplay(void)
{
	input *playground;
	string scorebored[2], tips[3];

	cleardevice();

	playground = (input *)malloc(sizeof(input));
	playground->x = pointw;
	playground->y = pointw;
	playground->h = (winh - 2 * pointw) - (winh - 2 * pointw) % pointw;
	playground->w = (winw - pointw - 300) - (winw - pointw - 300) % pointw;

	setfillcolor(edittextcolor);
	solidrectangle(0, 0, playground->w + 2 * pointw, winh);


	setfillcolor(background);
	solidrectangle(playground->x, playground->y, playground->x + playground->w, playground->y + playground->h);

	settextstyle(50, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(menutextcolor);
	scorebored[0] = inittext("当前分数", -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 20);
	outtextxy(scorebored[0].x + scorebored[0].w / 2, scorebored[0].y + scorebored[0].h / 2, scorebored[0].tstr);
	scorebored[1] = inittext("当前关卡", -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 170);
	outtextxy(scorebored[1].x + scorebored[1].w / 2, scorebored[1].y + scorebored[1].h / 2, scorebored[1].tstr);
	settextstyle(30, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(menutextcolor);
	tips[0] = inittext("方向键移动", -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 400);
	outtextxy(tips[0].x + tips[0].w / 2, tips[0].y + tips[0].h / 2, tips[0].tstr);
	tips[1] = inittext("Ctrl加速", -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 430);
	outtextxy(tips[1].x + tips[1].w / 2, tips[1].y + tips[1].h / 2, tips[1].tstr);
	tips[2] = inittext("空格暂停", -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 460);
	outtextxy(tips[2].x + tips[2].w / 2, tips[2].y + tips[2].h / 2, tips[2].tstr);

	return playground;
}

/*
函数功能：判断蛇是否死亡
函数参数：指向蛇的结构体，游戏区域
函数返回值：死亡与否
*/
bool isdeath(snake *snakehead, input playground, wall *Wall)
{
	int i;

	for (i = 1; i < (snakehead->num - 1); i++)
	{
		if ((snakehead->point[0]->x - snakehead->point[i]->x) * (snakehead->point[0]->x - snakehead->point[i + 1]->x) <= 0)//在水平两点之间  ！！或三点重合
		{
			if (snakehead->point[0]->y == snakehead->point[i]->y)
			{
				return true;
			}
		}
		if ((snakehead->point[0]->y - snakehead->point[i]->y) * (snakehead->point[0]->y - snakehead->point[i + 1]->y) <= 0)//在竖直两点之间  ！！或三点重合
		{
			if (snakehead->point[0]->x == snakehead->point[i]->x)
			{
				return true;
			}
		}

	}
	for (i = 0; i < Wall->num; i++)
	{
		if ((snakehead->point[0]->x == Wall->point[i].x) && (snakehead->point[0]->y == Wall->point[i].y))
		{
			return true;
		}
	}
	if (snakehead->point[0]->x <= playground.x || snakehead->point[0]->y <= playground.y || snakehead->point[0]->x >= playground.x + playground.w || snakehead->point[0]->y >= playground.y + playground.h)
	{//撞墙
		return true;
	}
	return false;
}

/*
函数功能：初始化生成的障碍物
函数参数：难度
函数返回值：wall指针
*/
wall *initwall(int idifficult)
{
	wall *Wall;

	Wall = (wall *)malloc(sizeof(wall));
	switch (idifficult)
	{
	case 0:
		Wall->num = 0;
		Wall->point = NULL;
		return Wall;
	case 1:
		Wall->num = 20;
		Wall->point = getwallpostion(20);
		return Wall;
	case 2:
		Wall->num = 40;
		Wall->point = getwallpostion(40);
		return Wall;
	case 3:
		Wall->num = 60;
		Wall->point = getwallpostion(60);
		return Wall;
	default:
		Wall->num = 0;
		Wall->point = NULL;
		return Wall;
	}
}

/*
函数功能：随机生成墙的坐标
函数参数：墙的数目
函数返回值：指向墙的坐标的指针
*/
POINT *getwallpostion(int num)
{
	POINT *wall;
	int i;
	int seed;

	srand((unsigned int)time(0));
	wall = (POINT *)malloc(num * sizeof(POINT));
	for (i = 0; i < num; i++)
	{
		seed = rand();
		wall[i].x = ((seed % (700 - 2 * pointw)) / pointw) * pointw + 1 * pointw;
		srand(seed);
		seed = rand();
		if (wall[i].x <= pointw * 11)
		{
			wall[i].y = ((seed % (500 - 7 * pointw)) / pointw) * pointw + 6 * pointw;
		}
		else
		{
			wall[i].y = ((seed % (500 - 2 * pointw)) / pointw) * pointw + 1 * pointw;
		}
	}
	return wall;
}

/*
函数功能：画出墙
函数参数：指向墙的指针
函数返回值：无
*/
void drawwall(wall *Wall)
{
	int i;

	for (i = 0; i < Wall->num; i++)
	{
		setfillcolor(edittextcolor);
		solidrectangle(Wall->point[i].x, Wall->point[i].y, Wall->point[i].x + pointw, Wall->point[i].y + pointw);
	}
}

/*
函数功能：画出蛇
函数参数：指向蛇的指针
函数返回值：无
*/
void drawsnake(snake *snak)
{
	int i;

	snak->num = 2;
	snak->point = (POINT **)malloc(2 * sizeof(POINT *));
	snak->point[0] = (POINT *)malloc(sizeof(POINT));
	snak->point[1] = (POINT *)malloc(sizeof(POINT));
	snak->point[0]->y = snak->point[1]->y = 2 * pointw;
	snak->point[0]->x = pointw * 5;
	snak->point[1]->x = pointw * 1;
	for (i = snak->point[1]->x + snak->r; i <= snak->point[0]->x + snak->r; i++)
	{
		setfillcolor(snakecolor);
		solidcircle(i, snak->point[0]->y + snak->r, snak->r);
	}
}

/*
函数功能：画出食物
函数参数：指向障碍物的指针
函数返回值：无
*/
food *drawfood(wall *Wall, snake *snak)
{
	food *fod;
	static int seed = (int)time(0);
	int i;

	fod = (food *)malloc(sizeof(food));
lable_restart:
	srand(seed);
	seed = rand();
	fod->x = ((seed % (700 - 2 * pointw)) / pointw) * pointw + 1 * pointw;
	srand(seed);
	seed = rand();
	fod->y = ((seed % (500 - 3 * pointw)) / pointw) * pointw + 1 * pointw;
	fod->r = pointw / 2;

	for (i = 0; i < Wall->num; i++)//不与墙重合
	{
		if ((fod->x == Wall->point[i].x) && (fod->y == Wall->point[i].y))
		{
			goto lable_restart;
		}
	}
	for (i = 0; i < snak->num - 1; i++)//不与蛇重合
	{
		if ((fod->x - snak->point[i]->x) * (fod->x - snak->point[i + 1]->x) < 0)//在水平两点之间  ！！或三点重合
		{
			if (fod->y == snak->point[i]->y)
			{
				goto lable_restart;
			}
		}
		if ((fod->y - snak->point[i]->y) * (fod->y - snak->point[i + 1]->y) < 0)//在竖直两点之间  ！！或三点重合
		{
			if (fod->x == snak->point[i]->x)
			{
				goto lable_restart;
			}
		}
	}

	setfillcolor(foodcolor);
	solidcircle(fod->x + pointw / 2, fod->y + pointw / 2, fod->r);

	return fod;
}

/*
函数功能：画出成绩
函数参数：指向游戏区域的指针，成绩
函数返回值：无
*/
void drawscore(int score, int section, input *playground)
{
	string scorebored[2];
	char *cscore = NULL;
	char *csection = NULL;
	unsigned int usedsize = 0, totlesize = 0;

	cscore = myintcat(cscore, &usedsize, &totlesize, score);
	usedsize = 0, totlesize = 0;
	csection = myintcat(csection, &usedsize, &totlesize, section);
	scorebored[0] = inittext(cscore, -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 70);
	scorebored[1] = inittext(csection, -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 220);
	setfillcolor(background);
	solidrectangle(scorebored[0].x + scorebored[0].w / 2 + 60, scorebored[0].y + scorebored[0].h / 2, scorebored[0].x + 3 * scorebored[0].w / 2 + 60, scorebored[0].y + 3 * scorebored[0].h / 2);
	solidrectangle(scorebored[1].x + scorebored[0].w / 2 + 60, scorebored[1].y + scorebored[1].h / 2, scorebored[0].x + 3 * scorebored[1].w / 2 + 60, scorebored[1].y + 3 * scorebored[1].h / 2);
	settextstyle(50, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(scorecolor);
	outtextxy(scorebored[0].x + scorebored[0].w / 2 + 60, scorebored[0].y + scorebored[0].h / 2, scorebored[0].tstr);
	outtextxy(scorebored[1].x + scorebored[1].w / 2 + 60, scorebored[1].y + scorebored[1].h / 2, scorebored[1].tstr);
	free(csection);
	free(cscore);
}

/*
函数功能：移动蛇
函数参数：指向蛇的指针
函数返回值：无
*/
void movesnake(snake *snak, int *derection, int *headto, bool eat)
{
	int i, j;

	if (eat)
	{
		goto lable_head;
	}
	//竖直
	if (snak->point[snak->num - 1]->x == snak->point[snak->num - 2]->x)
	{
		//向下
		if (snak->point[snak->num - 2]->y > snak->point[snak->num - 1]->y)
		{
			for (i = snak->point[snak->num - 1]->y; i <= snak->point[snak->num - 1]->y + pointw; i++)
			{
				setfillcolor(background);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
			}
			snak->point[snak->num - 1]->y += pointw;
			if (snak->point[snak->num - 2]->y == snak->point[snak->num - 1]->y)
			{
				free(snak->point[snak->num - 1]);
				snak->num -= 1;
			}
			//向右
			if (snak->point[snak->num - 2]->x > snak->point[snak->num - 1]->x)
			{
				for (i = snak->point[snak->num - 1]->x; i <= snak->point[snak->num - 1]->x + pointw; i++)
				{
					setfillcolor(snakecolor);
					solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
				}
			}
			//向左
			else if (snak->point[snak->num - 2]->x < snak->point[snak->num - 1]->x)
			{
				for (i = snak->point[snak->num - 1]->x; i >= snak->point[snak->num - 1]->x - pointw; i--)
				{
					setfillcolor(snakecolor);
					solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
				}
			}
			else
			{
				setfillcolor(snakecolor);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
		}
		//向上
		else
		{
			for (i = snak->point[snak->num - 1]->y; i >= snak->point[snak->num - 1]->y - pointw; i--)
			{
				setfillcolor(background);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
			}
			snak->point[snak->num - 1]->y -= pointw;
			if (snak->point[snak->num - 2]->y == snak->point[snak->num - 1]->y)
			{
				free(snak->point[snak->num - 1]);
				snak->num -= 1;
			}
			//向右
			if (snak->point[snak->num - 2]->x > snak->point[snak->num - 1]->x)
			{
				for (i = snak->point[snak->num - 1]->x; i <= snak->point[snak->num - 1]->x + pointw; i++)
				{
					setfillcolor(snakecolor);
					solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
				}
			}
			//向左
			else if (snak->point[snak->num - 2]->x < snak->point[snak->num - 1]->x)
			{
				for (i = snak->point[snak->num - 1]->x; i >= snak->point[snak->num - 1]->x - pointw; i--)
				{
					setfillcolor(snakecolor);
					solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
				}
			}
			else
			{
				setfillcolor(snakecolor);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
		}
	}
	//水平
	else
	{
		//向右
		if (snak->point[snak->num - 2]->x > snak->point[snak->num - 1]->x)
		{
			for (i = snak->point[snak->num - 1]->x; i <= snak->point[snak->num - 1]->x + pointw; i++)
			{
				setfillcolor(background);
				solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
			snak->point[snak->num - 1]->x += pointw;
			if (snak->point[snak->num - 1]->x == snak->point[snak->num - 2]->x)
			{
				free(snak->point[snak->num - 1]);
				snak->num -= 1;
			}
			//向下
			if (snak->point[snak->num - 2]->y > snak->point[snak->num - 1]->y)
			{
				for (i = snak->point[snak->num - 1]->y; i <= snak->point[snak->num - 1]->y + pointw; i++)
				{
					setfillcolor(snakecolor);
					solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
				}
			}
			//向上
			else if (snak->point[snak->num - 2]->y < snak->point[snak->num - 1]->y)
			{
				for (i = snak->point[snak->num - 1]->y; i >= snak->point[snak->num - 1]->y - pointw; i--)
				{
					setfillcolor(snakecolor);
					solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
				}
			}
			else
			{
				setfillcolor(snakecolor);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
		}
		//向左
		else
		{
			for (i = snak->point[snak->num - 1]->x; i >= snak->point[snak->num - 1]->x - pointw; i--)
			{
				setfillcolor(background);
				solidcircle(i + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
			snak->point[snak->num - 1]->x -= pointw;
			if (snak->point[snak->num - 1]->x == snak->point[snak->num - 2]->x)
			{
				free(snak->point[snak->num - 1]);
				snak->num -= 1;
			}
			//向下
			if (snak->point[snak->num - 2]->y > snak->point[snak->num - 1]->y)
			{
				for (i = snak->point[snak->num - 1]->y; i <= snak->point[snak->num - 1]->y + pointw; i++)
				{
					setfillcolor(snakecolor);
					solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
				}
			}
			//向上
			else if (snak->point[snak->num - 2]->y < snak->point[snak->num - 1]->y)
			{
				for (i = snak->point[snak->num - 1]->y; i >= snak->point[snak->num - 1]->y - pointw; i--)
				{
					setfillcolor(snakecolor);
					solidcircle(snak->point[snak->num - 1]->x + snak->r, i + snak->r, snak->r);
				}
			}
			else
			{
				setfillcolor(snakecolor);
				solidcircle(snak->point[snak->num - 1]->x + snak->r, snak->point[snak->num - 1]->y + snak->r, snak->r);
			}
		}
	}
lable_head:
	switch (*derection)//移动
	{
	case 0://上
		if (*headto == 1)
		{
			*derection = 1;
			goto lable_head;
		}
		for (i = snak->point[0]->y; i >= snak->point[0]->y - pointw; i--)
		{
			setfillcolor(snakecolor);
			solidcircle(snak->point[0]->x + snak->r, i + snak->r, snak->r);
		}
		if (*headto == 2 || *headto == 3)
		{
			snak->num += 1;
			snak->point = (POINT **)realloc(snak->point, snak->num * sizeof(POINT *));
			for (j = snak->num - 1; j > 0; j--)
			{
				snak->point[j] = snak->point[j - 1];
			}
			snak->point[1] = (POINT *)malloc(sizeof(POINT));
			snak->point[1]->x = snak->point[0]->x;
			snak->point[1]->y = snak->point[0]->y;
		}
		snak->point[0]->y -= pointw;
		*headto = 0;
		break;
	case 1://下
		if (*headto == 0)
		{
			*derection = 0;
			goto lable_head;
		}
		for (i = snak->point[0]->y; i <= snak->point[0]->y + pointw; i++)
		{
			setfillcolor(snakecolor);
			solidcircle(snak->point[0]->x + snak->r, i + snak->r, snak->r);
		}
		if (*headto == 2 || *headto == 3)
		{
			snak->num += 1;
			snak->point = (POINT **)realloc(snak->point, snak->num * sizeof(POINT *));
			for (j = snak->num - 1; j > 0; j--)
			{
				snak->point[j] = snak->point[j - 1];
			}
			snak->point[1] = (POINT *)malloc(sizeof(POINT));
			snak->point[1]->x = snak->point[0]->x;
			snak->point[1]->y = snak->point[0]->y;
		}
		snak->point[0]->y += pointw;
		*headto = 1;
		break;
	case 2://左
		if (*headto == 3)
		{
			*derection = 3;
			goto lable_head;
		}
		for (i = snak->point[0]->x; i >= snak->point[0]->x - pointw; i--)
		{
			setfillcolor(snakecolor);
			solidcircle(i + snak->r, snak->point[0]->y + snak->r, snak->r);
		}
		if (*headto == 0 || *headto == 1)
		{
			snak->num += 1;
			snak->point = (POINT **)realloc(snak->point, snak->num * sizeof(POINT *));
			for (j = snak->num - 1; j > 0; j--)
			{
				snak->point[j] = snak->point[j - 1];
			}
			snak->point[1] = (POINT *)malloc(sizeof(POINT));
			snak->point[1]->x = snak->point[0]->x;
			snak->point[1]->y = snak->point[0]->y;
		}
		snak->point[0]->x -= pointw;
		*headto = 2;
		break;
	case 3://右
		if (*headto == 2)
		{
			*derection = 2;
			goto lable_head;
		}
		for (i = snak->point[0]->x; i <= snak->point[0]->x + pointw; i++)
		{
			setfillcolor(snakecolor);
			solidcircle(i + snak->r, snak->point[0]->y + snak->r, snak->r);
		}
		if (*headto == 0 || *headto == 1)
		{
			snak->num += 1;
			snak->point = (POINT **)realloc(snak->point, snak->num * sizeof(POINT *));
			for (j = snak->num - 1; j > 0; j--)
			{
				snak->point[j] = snak->point[j - 1];
			}
			snak->point[1] = (POINT *)malloc(sizeof(POINT));
			snak->point[1]->x = snak->point[0]->x;
			snak->point[1]->y = snak->point[0]->y;
		}
		snak->point[0]->x += pointw;
		*headto = 3;
		break;
	default:
		break;
	}

}

/*
函数功能：判断ctrl是否按下
函数参数：指向flag的指针
函数返回值：无
*/
void *contrlthread(void *args)
{
	bool *flag;

	flag = (bool *)args;

	while (true)
	{
		if (exitcontrlthread)
		{
			break;
		}
		//https://baike.so.com/doc/6058819-6271869.html
		if (GetKeyState(VK_CONTROL) < 0)		//如果ctrl被按下 
		{
			*flag = true;
		}
		else
		{
			*flag = false;
		}
	}
	return NULL;
}

/*
函数功能：判断食物是否被吃
函数参数：指向蛇的指针，指向食物的指针
函数返回值：食物是否被吃
*/
bool iseat(snake *snak, food *fod)
{
	if (snak->point[0]->x == fod->x && snak->point[0]->y == fod->y)
	{
		return true;
	}
	return false;
}

/*
函数功能：显示提示
函数参数：关卡，指向游戏区域的指针
函数返回值：无
*/
void notice(int section, input *playground)
{
	char *cnotice = NULL;
	unsigned int usedsize = 0, totlesize = 0;
	string snotice;
	int i;

	cnotice = mystrcat(cnotice, &usedsize, &totlesize, "关卡");
	cnotice = myintcat(cnotice, &usedsize, &totlesize, section);
	settextstyle(70, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	snotice = inittext(cnotice, -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 270);
	outtextxy(snotice.x + snotice.w / 2 + 30, snotice.y + snotice.h / 2, snotice.tstr);
	Sleep(1000);
	setfillcolor(background);
	solidrectangle(snotice.x + snotice.w / 2 + 30, snotice.y + snotice.h / 2, snotice.x + 3 * snotice.w / 2 + 30, snotice.y + 3 * snotice.h / 2);
	for (i = 3; i > 0; i--)
	{
		usedsize = 0, totlesize = 0;
		cnotice = cnotice = myintcat(cnotice, &usedsize, &totlesize, i);
		settextstyle(70, 0, "微软雅黑");
		setbkcolor(background);
		settextcolor(noticecolor);
		snotice = inittext(cnotice, -winw / 2 + playground->x + playground->w + 80, -winh / 2 + 270);
		outtextxy(snotice.x + snotice.w / 2 + 30, snotice.y + snotice.h / 2, snotice.tstr);
		Sleep(1000);
		setfillcolor(background);
		solidrectangle(snotice.x + snotice.w / 2 + 30, snotice.y + snotice.h / 2, snotice.x + 3 * snotice.w / 2 + 30, snotice.y + 3 * snotice.h / 2);
	}
	free(cnotice);
}

/*
函数功能：显示死亡提示与信息
函数参数：分数，时间(毫秒)
函数返回值：无
*/
bool death(unsigned int score, unsigned int time)
{
	string str[4];
	string schoise[2];
	char *cscore = NULL;
	unsigned int usedsize = 0, totlesize = 0;
	unsigned int times, timem, timeh;
	user *head, *pp;
	unsigned int count = 0;

	pp = head = readinfo();
	//画框
	setfillcolor(edittextcolor);
	solidrectangle(200, 50, winw - 200, winh - 50);
	setfillcolor(background);
	solidrectangle(200 + pointw, 50 + pointw, winw - 200 - pointw, winh - 50 - pointw);
	//Game Over
	settextstyle(70, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	str[0] = inittext("Game Over", 0, -150);
	outtextxy(str[0].x, str[0].y, str[0].tstr);
	//显示所得分数
	usedsize = totlesize = 0;
	cscore = mystrcat(cscore, &usedsize, &totlesize, "分数：");
	cscore = myintcat(cscore, &usedsize, &totlesize, score);
	str[1] = inittext(cscore, 0, -85);
	settextstyle(60, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	outtextxy(winw / 2 - 100, str[1].y, str[1].tstr);
	//更新最高分
	if (islogin)
	{
		if (score > usr.maxscore)
		{
			usr.maxscore = score;
		}
	}
	//更新排名
	while (pp != NULL)
	{
		if(islogin)
		{	//在当前排名下的排名+1
			if ((pp->maxscore < score) && (pp->order < usr.order))
			{
				pp->order += 1;
			}
		}
		//计算分数大于等于当前玩家的人数
		if (pp->maxscore >= score && pp->order != 99)
		{
			count++;
		}
		pp = pp->next;
	}
	usr.order = (usr.order > (count + 1)) ? (count + 1) : usr.order;
	usedsize = totlesize = 0;
	cscore = mystrcat(cscore, &usedsize, &totlesize, "排名：");
	cscore = myintcat(cscore, &usedsize, &totlesize, count + 1);
	str[2] = inittext(cscore, 0, -30);
	settextstyle(60, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	outtextxy(winw / 2 - 100, str[2].y, str[2].tstr);
	//时间
	usedsize = totlesize = 0;
	cscore = mystrcat(cscore, &usedsize, &totlesize, "用时：");
	time /= 1000;
	timeh = time / 60 / 60;
	timem = time / 60 - timeh;
	times = time % 60 - timem;
	if (islogin)
	{//更新当前时间
		usr.timeh += timeh;
		usr.timem += timem;
		usr.times += times;

		if (usr.times / 60 > 0)
		{
			usr.timem += usr.times / 60;
			usr.times -= usr.times;
		}
		if (usr.timem / 60 > 0)
		{
			usr.timeh += usr.timem / 60;
			usr.timem -= usr.timem;
		}
	}

	if (timeh > 0)
	{
		cscore = myintcat(cscore, &usedsize, &totlesize, timeh);
		cscore = mystrcat(cscore, &usedsize, &totlesize, "时");
		cscore = myintcat(cscore, &usedsize, &totlesize, timem);
		cscore = mystrcat(cscore, &usedsize, &totlesize, "分");
	}
	else
	{
		if (timem > 0)
		{
			cscore = myintcat(cscore, &usedsize, &totlesize, timem);
			cscore = mystrcat(cscore, &usedsize, &totlesize, "分");
		}
	}
	cscore = myintcat(cscore, &usedsize, &totlesize, times);
	cscore = mystrcat(cscore, &usedsize, &totlesize, "秒");
	str[3] = inittext(cscore, 0, 30);
	settextstyle(60, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	outtextxy(winw / 2 - 100, str[3].y, str[3].tstr);

	if (islogin)
	{
		usr.playcount++;
		writeinfo(head);
	}
	settextstyle(50, 0, "微软雅黑");
	setbkcolor(background);
	settextcolor(noticecolor);
	schoise[0] = inittext("返回主菜单", 100, 130);
	schoise[1] = inittext("重新开始", -100, 130);
	outtextxy(schoise[0].x, schoise[0].y, schoise[0].tstr);
	outtextxy(schoise[1].x, schoise[1].y, schoise[1].tstr);
	if (mousehandle(schoise, 2) == 0)
	{
		return false;
	}
	return true;
}

/*
函数功能：向文件写入用户信息
函数参数：无
函数返回值：无
*/
void writeinfo(user *head)
{
	FILE *fp;
	user *pp = NULL, *pn = NULL;

	fopen_s(&fp, finfo, "w");

if (fp == NULL)
	{
		MessageBox(GetHWnd(), "未能写入用户信息文件", "错误", MB_ICONERROR);
		exit(-1);
	}
	pp = head;
	while (pp != NULL)
	{
		if (strcmp(pp->username, usr.username) == 0)
		{
			fprintf_s(fp, "%s %s %u %u %u %u %u %u\n", usr.username, usr.password, usr.playcount, usr.timeh, usr.timem, usr.times, usr.maxscore, usr.order);
		}
		else
		{
			fprintf_s(fp, "%s %s %u %u %u %u %u %u\n", pp->username, pp->password, pp->playcount, pp->timeh, pp->timem, pp->times, pp->maxscore, pp->order);
		}
		pp = pp->next;
	}
	fclose(fp);
	pp = head;
	while (pp != NULL)
	{
		pn = pp->next;
		free(pp);
		pp = pn;
	}
}