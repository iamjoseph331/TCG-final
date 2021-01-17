#include <iostream>
using namespace std;

void printCapture()
{
	cout << "{";
	for(int i = 0; i < 14; ++i)
	{
		cout << "{";
		for(int j = 0; j < 14; ++j)
		{
			if(j == 0)
			{
				if(i >= 7 && i < 13)
				{
					cout << "1";
				}
				else
				{
					cout << "0";
				}
				continue;
			}
			if(i / 7 == j / 7)
			{
				cout << ",0";
				continue;
			}
			if(i % 7 == 1)
			{
				cout << ",1";
				continue;
			}
			if(i % 7 == 0 && j % 7 == 6)
			{
				cout << ",1";
				continue;
			}
			if(i % 7 == 6 && j % 7 == 0)
			{
				cout << ",0";
				continue;
			}
			if(i % 7 >= j % 7)
			{
				cout << ",1";
				continue;
			}
			else
			{
				cout << ",0";
				continue;
			}
		}
		cout << "}," << endl;
	}
	cout << "};";
}

void printMovable()
{
	cout << "{2,";
	for(int i = 1; i < 31; ++i)
	{
		if(i % 4 == 0)cout << endl;
		int base = 4;
		if(i % 4 == 0 || i % 4 == 3)
			base -= 1;
		if(i < 4 || i >= 28)
			base -= 1;
		cout << " " << base << ",";
	}
	cout << " 2};";
}

void printMoves()
{
	int dx[4] = {-4,1,4,-1};
	cout << "{";
	for(int i = 0; i < 32; ++i)
	{
		cout << "{";
		bool flag = false;
		int cnt = 0;
		for(int j = 0; j < 4; ++j)
		{
			int x = i + dx[j];
			if(x < 0 || x > 31 || ((j == 1 || j == 3) && x / 4 != i / 4))
			{
				cnt += 1;
				continue;
			}
			if(!flag)flag = true;
			else cout << ", ";
			cout << x;
		}
		for(int j = 0; j < cnt; ++j)
		{
			cout << ", -1";
		}
		cout << "},\n";
	}
	cout << "};";
}

void convert()
{
	static const char skind[]={'-','X','P','C','N','R','M','G','K','p','c','n','r','m','g','k'};
	for(int i = 0; i < 32; ++i)
	{
		char c;
		cin >> c;
		for(int f = 0; f < 16; ++f)
		{
			if(skind[f] == c)
			{
				cout << f - 2 << endl;
				break;
			}
			if(f == 15)i -= 1;
		}
	}
}

void secondHand()
{
	cout << "{";
	for(int i = 0; i < 16; ++i)
	{
		if(i % 4 == 0)cout << endl;
		cout << i + 8 << ",";
	}
	for(int i = 16; i < 32; ++i)
	{
		if(i % 4 == 0)cout << endl;
		cout << i - 8 << ",";
	}
	cout << "};";
}

int main()
{
	//printCapture();
	secondHand();
	return 0;
}
