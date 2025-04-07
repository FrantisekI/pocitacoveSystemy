#include <stdio.h>
#include <iostream>
using namespace std;

int avg(const int pole[], int length)
{
	int total = 0;
	for (int i = 0; i < length; ++i)
	{
		total += pole[i];
	}
	cout << total;
	return total / length;
}

/*void fill(int amount, const char withWhat)
{
	while (amount > 0) {
		cout << withWhat;
		--amount;
	}
}

void drawChartUnsigned(const int values[], int length)
{
	
	for (int i = 0; i < length; ++i)
	{
		fill(values[i], '*');
		printf("\n");
	}
}
int countMinimum(const int values[], int length)
{
	int min = 0;
	for (int i = 0; i < length; ++i)
	{
		if (values[i] < min)
		{
			min = values[i];
		}
	}
	return min;
}


void drawChartSigned(const int values[], int length)
{
	int minimum = countMinimum(values, length);

}

/*int main()
{
	int a[] = { 10, 12, 14, 16, 17, 1, 2 };
	int a_len = sizeof(a) / sizeof(a[0]);

	int b[] = { -1, 15, 7, -5, -3, 1, 0 };
	int b_len = sizeof(b) / sizeof(b[0]);



	drawChartUnsigned(a, a_len);
}*/