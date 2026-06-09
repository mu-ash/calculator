#include <stdio.h>
#include <easyx.h>
#include <string.h>
#include <math.h>

 ExMessage msg =  {0};
 bool mouse_down = false;



 typedef enum
 {
	 state_input_num1,
	 state_input_op,
	 state_input_num2,
	 state_calc_result
 }calcstate;
 char num1_str[30] = "";
 char num2_str[30] = "";
 double num1 = 0, num2 = 0, result = 0;
 char op = 0;
 calcstate calc_state = state_input_num1;


void centertext()
{
	int  w = 240, h = 400;
	settextcolor(BLACK);
	char str[] = "Calculator";

	int hspace = (w - textwidth(str))/2;
	int vspace = (h - textheight(str))/2;

	settextstyle(25, 0, _T("微软雅黑"));
	outtextxy(hspace,0,str);
}

bool inarea(int x, int y, int w, int h, int mx, int my)
{
	if (mx > x && mx<x + w && my>y && my < y + h)
	{
		return true;
	}
	return false;
}

bool button(int x, int y, int w, int h, const char* text)
{
	if (inarea(x, y, w, h, msg.x, msg.y))
	{
		setfillcolor(WHITE);
	}
	else
	{
		setfillcolor(BLACK);
	}
	fillrectangle(x,y,x+w,y+h);

	settextcolor(WHITE);
	int hspace = (w - (textwidth(text))) / 2;
	int vspace = (h - (textheight(text))) / 2;

	outtextxy(x+hspace,y+vspace,text);

	bool click = false;
	if (msg.message == WM_LBUTTONDOWN && inarea(x, y, w, h, msg.x, msg.y) && !mouse_down)
	{
		click = true;
		mouse_down = true;
	}
	else if (msg.message == WM_LBUTTONUP)
	{
		mouse_down = false;
	}

	return click;

}

void drawtext(char test)
{

	settextstyle(38, 0, _T("微软雅黑"));
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);//设置背景模式
	outtextxy(10, 10, test);
}



void showScreen(const char* s)
{
	setfillcolor(WHITE);
	fillrectangle(0, 0, 240, 90);   // 清空显示区域
	settextcolor(BLACK);
	settextstyle(32, 0, _T("微软雅黑"));
	setbkmode(TRANSPARENT);
	outtextxy(10, 20, s);
}
// 拼接字符到数字字符串
void appendChar(char* dest, char ch)
{
	int len = strlen(dest);
	if (len < 28)
	{
		dest[len] = ch;
		dest[len + 1] = '\0';
	}
}

// 清空计算器所有数据
void clearAll()
{
	num1_str[0] = '\0';
	num2_str[0] = '\0';
	num1 = num2 = result = 0;
	op = 0;
	calc_state = state_input_num1;
	showScreen("0");
}

// 数字按键处理
void dealNumber(char ch)
{
	if (calc_state == state_input_num1)
	{
		appendChar(num1_str, ch);
		showScreen(num1_str);
	}
	else if (calc_state == state_input_num2)
	{
		appendChar(num2_str, ch);
		showScreen(num2_str);
	}
}

// 小数点处理（禁止多个小数点）
void dealDot()
{
	if (calc_state == state_input_num1)
	{
		if (strchr(num1_str, '.') == NULL)
		{
			appendChar(num1_str, '.');
			showScreen(num1_str);
		}
	}
	else if (calc_state == state_input_num2)
	{
		if (strchr(num2_str, '.') == NULL)
		{
			appendChar(num2_str, '.');
			showScreen(num2_str);
		}
	}
}

//取反
void dealNegate()
{
	char* p = NULL;
	if (calc_state == state_input_num1)
	{
		p = num1_str;
	}
	else if (calc_state == state_input_num2)
	{
		p = num2_str;
	}

	if (p == NULL) return;

	if (strlen(p) == 0)
	{
		appendChar(p, '-');
	}
	else if (p[0] == '-')
	{
		int len = strlen(p);
		for (int i = 0; i < len; i++)
		{
			p[i] = p[i + 1];
		}
	}
	else
	{
		if (strlen(p) < 28)
		{
			int len = strlen(p);
			for (int i = len; i >= 0; i--)
			{
				p[i + 1] = p[i];
			}
			p[0] = '-';
		}
	}

	if (calc_state == state_input_num1)
		showScreen(num1_str);
	else
		showScreen(num2_str);
}

// 运算符处理
void dealOp(char ch)
{
	if (calc_state == state_input_num1)
	{
		num1 = atof(num1_str);
		op = ch;
		calc_state = state_input_num2;
	}
}

// 计算结果（switch 实现四则运算）
void calcEqual()
{
	if (calc_state != state_input_num2 || op == 0)
		return;

	num2 = atof(num2_str);
	result = 0;

	switch (op)
	{
	case '+':
		result = num1 + num2;
		break;
	case '-':
		result = num1 - num2;
		break;
	case '*':
		result = num1 * num2;
		break;
	case '/':
		// 逻辑非：判断除数不为0
		if (!(num2 == 0))
			result = num1 / num2;
		else
		{
			showScreen("除数不能为0");
			Sleep(800);
			clearAll();
			return;
		}
		break;
	case '%':
		if (!(num2 == 0))
			result = fmod(num1, num2);
		else
		{
			showScreen("除数不能为0");
			Sleep(800);
			clearAll();
			return;
		}
		break;
	}


	// 结果转为字符串显示
	char res_buf[30];
	sprintf(res_buf, "%.6g", result);
	showScreen(res_buf);

	// 结果作为下一次运算第一个数
	strcpy(num1_str, res_buf);
	num1 = result;
	num2_str[0] = '\0';
	num2 = 0;
	op = 0;
	calc_state = state_input_num1;
}

int main()
{
	
	initgraph(240,400,EX_SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	setbkmode(TRANSPARENT);
	
	clearAll();
	centertext();
	while (true)
	{
	
		
		peekmessage(&msg, EX_MOUSE);
		Sleep(20);
		BeginBatchDraw();
		//cleardevice();
		
		if (button(0, 100, 60, 60, "C"))
		{
			
			printf("清空\n");
			clearAll();

		}
		
		if (button(60, 100, 60, 60, "+/-"))
		{
			printf("取反\n");
			dealNegate();
			
		}
		if (button(120, 100, 60, 60, "%"))
		{
			printf("取余\n");
			dealOp('%');
			
		}
		if (button(180, 100, 60, 60, "/"))
		{
			printf("/\n");
			dealOp('/');
		}

		if (button(0, 160, 60, 60, "7"))
		{
			dealNumber('7');
		}
		if (button(60, 160, 60, 60, "8"))
		{
			dealNumber('8');
		}
		if (button(120, 160, 60, 60, "9"))
		{
			dealNumber('9');
		}
		if (button(180, 160, 60, 60, "*"))
		{
			printf("*\n");
			dealOp('*');
		}

		if (button(0, 220, 60, 60, "4"))
		{
			dealNumber('4');
		}
		if (button(60, 220, 60, 60, "5"))
		{
			dealNumber('5');
		}
		if (button(120, 220, 60, 60, "6"))
		{
			dealNumber('6');
		}
		if (button(180, 220, 60, 60, "-"))
		{
			printf("-\n");
			dealOp('-');
		}

		if (button(0, 280, 60, 60, "1"))
		{
			dealNumber('1');
			
		}
		if (button(60, 280, 60, 60, "2"))
		{
			dealNumber('2');
		
		}
		if (button(120, 280, 60, 60, "3"))
		{
			dealNumber('3');
		
		}
		if (button(180, 280, 60, 60, "+"))
		{
			printf("+\n");
			dealOp('+');
		}

		if (button(0, 340, 120, 60, "0"))
		{
			dealNumber('0');
		
		}
		if (button(120, 340, 60, 60, "."))
		{
			printf(".\n");
			dealDot();
		}
		if (button(180, 340, 60, 60, "="))
		{
			printf("=\n");
			calcEqual();
		}
		
		//Sleep(20);
	

		
		EndBatchDraw();
	}


	getchar();
	closegraph();
	return 0;
}
