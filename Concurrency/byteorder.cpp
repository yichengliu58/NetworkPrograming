#include <iostream>
using namespace std;

int main()
{
	union Test
	{
		short num;
		char part[2];
	};
	Test t;
	t.num = 0x0102;
	if(t.part[0] == 2 && t.part[1] == 1)
		cout << "little" << endl;
	else
		cout << "big" << endl;
	return 0;
}