#include <stdio.h>
#include "temps.h"
#include <iostream>
using namespace std;

void fill(int amount, const char withWhat)
{
	while (amount > 0) {
		cout << withWhat;
		--amount;
	}
}

void drawValue(int minimum, int valueToDraw) 
{
	int negativeDraw = 0;
	if (valueToDraw < 0) {
		negativeDraw = -valueToDraw;
	}
	int padding = -minimum - negativeDraw;

	fill(padding, ' ');
	fill(negativeDraw, '*');
	fill(1, '|');
	fill(valueToDraw, '*');
	printf("\n");	
}



int findMinimum(const int tmp[], int size)
{
	int minimum = 0;
	
	for (int i = 0; i < size; ++i)
	{
		if (tmp[i] < minimum && tmp[i] != no_value)
		{
			minimum = tmp[i];
		}
	}
	return minimum;
}

void drawThermometer(const int tmp[], int size, int minimum)
{
	int lastCorrectValue = 0;
	for (int i = 0; i < size; ++i)
	{
		if (tmp[i] != no_value)
		{
			lastCorrectValue = tmp[i];
		}
		drawValue(minimum, lastCorrectValue);
	}
}

int main() 
{
	int length = sizeof(temperatures) / sizeof(temperatures[0]);
	int minimum = findMinimum(temperatures, length);
	drawThermometer(temperatures, length, minimum);

}