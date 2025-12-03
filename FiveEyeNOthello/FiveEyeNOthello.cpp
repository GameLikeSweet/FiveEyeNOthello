#include<iostream>
#include<iomanip>
#include<vector>
#include<Windows.h>

using namespace std;

void SetConsoleFont(int width, int height)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	cfi.nFont = 0;
	cfi.dwFontSize.X = width;    // 가로 문자 크기
	cfi.dwFontSize.Y = height;   // 세로 문자 크기
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");  // 콘솔 호환 폰트

	SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
}


struct Logic
{
	int turn = 0; int Rule = 0; int Size = 0;
	int** dat;
	void makearr();
	void delarr();
	void turnmove();
	/*
	enum
	{
		Up = 0,
		Ur,
		Right,
		Dr,
		Down,
		Dl,
		Left,
		Ul
	};*/
	void putstone(int r, int c);
	bool canStone(int r, int c);
	bool check(int Rmov, int Cmov, int r, int c, int& five);
	bool isBreak(int r, int c, bool& res);
	bool endgame = false;
	void startstone(); //오셀로용 시작 돌
	vector <pair<int, int >>mem;
};

void Logic::makearr()
{
	dat = (int**)new int* [Size];
	dat[0] = (int*) new int[Size * Size];
	for (int i = 1; i < Size; i++) dat[i] = dat[i - 1] + Size;
	for (int i = 0; i < Size * Size; i++)dat[i / Size][i % Size] = 0;
}

void Logic::delarr()
{
	delete dat[0];
	delete dat;
}

void Logic::turnmove()
{
	turn = 1 - turn;
}

void Logic::putstone(int r, int c)
{
	if (dat[r][c] != 0) return;

	if (canStone(r, c))
	{
		return;
	}

	dat[r][c] = turn + 1;

	turnmove();
}

// 오목을 위해 스켄 방향을 짝지어 놓을 필요 있음
// five 값이 4보다 크면 Rule 이 1(오목) 일때 오목 승리조건 달성 및 게임오버처리
// 오셀로 규칙 확인 후 돌을 둘 수 있으면 false 없으면 true로 반환; * 단 Rule 1(오목) 이면 무조건 false로 반환;

bool Logic::canStone(int r, int c)
{
	bool res = true;
	int five = 0;

	/* 현제 오목 룰 적용 불가
	for (int i = 0; i < 8; i++)
	{
		if (i == 4) continue;
		check(i/3-1, i%3-1, r, c ,five);
	}
	*/


	if (check(1, 0, r, c, five))		res = false;
	if (check(-1, 0, r, c, five))			res = false;
	if (five >= 4 && Rule == 1) endgame = true;
	else five = 0;


	if (check(0, 1, r, c, five))		res = false;
	if (check(0, -1, r, c, five))		res = false;
	if (five >= 4 && Rule == 1) endgame = true;
	else five = 0;

	if (check(1, 1, r, c, five))			res = false;
	if (check(-1, -1, r, c, five))			res = false;
	if (five >= 4 && Rule == 1) endgame = true;
	else five = 0;

	if (check(1, -1, r, c, five))			res = false;
	if (check(-1, 1, r, c, five))			res = false;
	if (five >= 4 && Rule == 1) endgame = true;
	else five = 0;

	if (Rule == 1) return false;
	return res;
}

// switch를 이용해 dir 방향으로 이동하면서 isBreak 함수로 mem에 데이터 추가 결정
//탐색이 끝나면 Rule에 따라 mem 데이터 확인
//mem 초기화 및 오셀로 조건에 따라 res값으로 리턴

bool Logic::check(int Rmov, int Cmov, int r, int c, int& five)
{

	bool res = false;
	int mr = r + Rmov;
	int mc = c + Cmov;

	while (0 <= mr && mr < Size && 0 <= mc && mc < Size)
	{
		if (isBreak(mr, mc, res)) break;
		mr += Rmov; mc += Cmov;
	}

	if (res && Rule == 2)
	{
		res = false;
		for (auto p : mem)
		{
			if (dat[p.first][p.second] != turn + 1) res = true;
			dat[p.first][p.second] = turn + 1;
		}


	}
	if (Rule == 1)
	{
		for (auto p : mem)
		{
			if (dat[p.first][p.second] == turn + 1) five++;
			else break;
		}
	}
	mem.clear();

	return res;
}

// dat 값이 0 이면 무조건 true;
// dat 값이 turn + 1 이면 무조건 벡터 추가 및 res 값 true;
// dat 값이 0 또는 turn +1 이 아니면 벡터에 추가;  *단 res 값이 true 면 벡터에 추가 전에 true로 리턴

bool Logic::isBreak(int r, int c, bool& res)
{
	if (dat[r][c] == 0) return true;
	else if (dat[r][c] == turn + 1)
	{
		res = true;
		mem.push_back(make_pair(r, c));
	}
	else
	{
		if (res) return true;
		mem.push_back(make_pair(r, c));
	}
	return false;
}

void Logic::startstone()
{
	int k = Size / 2;
	dat[k][k] = 1;
	dat[k - 1][k - 1] = 1;
	dat[k - 1][k] = 2;
	dat[k][k - 1] = 2;
}

struct Render
{
	int** dat;
	int Rule; int num; // Rule 1 == 선(오목), Rule 2 ==  칸(오셀로)
	void copydat(int** a, int& R, int& s);
	string getstring(int r, int c); // 룰에 따라 공백과 선으로 표현
	void Rendering(); // 판 전체 출력
};

void Render::copydat(int** a, int& R, int& s)
{
	dat = a;
	Rule = R;
	num = s;
}


//Rule이 2일때는 구할 수 있는 데이터가 4배로 늘어남
string Render::getstring(int r, int c)
{
	string a;
	if (r <= 0 && c <= 0) a = "┌";
	else if (r <= 0 && c == num * Rule - 1) a = "┐";
	else if (r == num * Rule - 1 && c == num * Rule - 1) a = "┘";
	else if (r == num * Rule - 1 && c <= 0) a = "└";
	else if (r <= 0) a = "┬";
	else if (c <= 0) a = "├";
	else if (r == num * Rule - 1) a = "┴";
	else if (c == num * Rule - 1) a = "┤";
	else a = "┼";

	if (r % Rule && c % Rule) return a;
	else if (r % Rule) return "──";
	else if (c % Rule) return "│";

	if (r < 0 || c < 0) return a;

	if (dat[r / Rule][c / Rule] == 0)
	{
		if (Rule == 2) a = "  ";
		return a;
	}
	else if (dat[r / Rule][c / Rule] == 1) return "○ ";
	else return "● ";
}

//Rule 이라는 숫자가 1 과 2일 필요가 없다
//Rule이 데이터라는 측면에서 가치가 있어서 1,과 2 일 필요가 있게 만들면?

void Render::Rendering()
{
	for (int i = 0; i < num; i++)
	{
		cout << hex << setw(Rule * 2) << i;
	}
	cout << "\n";


	for (int i = 1 - Rule; i < num * Rule; i++)
	{
		for (int l = 1 - Rule; l < num * Rule; l++)
		{
			cout << getstring(i, l);
		}
		if (i % Rule == 0) cout << hex << i / Rule;
		cout << " \n";
	}
}

int translate(char a)
{
	if (48 <= a && a < 58) return a - 48;
	else return a - 87;
}

int main()
{

	SetConsoleFont(10, 20);

	cout << "Choose a Game : 1) Omok ,  2) Othello\n";
	int n; cin >> n;

	Logic Lg;
	Render Rd;

	if (n == 1)
	{
		Lg.Size = 16; 	Lg.Rule = 1;
	}
	else if (n == 2)
	{
		Lg.Size = 8; 	Lg.Rule = 2;
	}
	else return 0;

	Lg.makearr();
	Rd.copydat(Lg.dat, Lg.Rule, Lg.Size);

	if (n == 2) Lg.startstone();

	char input[10];
	int r; int c;

	while (1)
	{
		system("cls");
		Rd.Rendering();

		cout << "\n";
		if (Lg.turn) cout << "●";
		else cout << "○";
		cout << "Input Position : ";
		cin >> input;
		if (strcmp(input, "gg") == 0) break;
		else if (strcmp(input, "pass") == 0)
		{
			Lg.turnmove();
			continue;
		}
		r = translate(input[0]);
		c = translate(input[1]);

		Lg.putstone(r, c);

		if (Lg.endgame) break;
	}
	system("cls");

	cout << "Game Over\n";

	Lg.delarr();
}