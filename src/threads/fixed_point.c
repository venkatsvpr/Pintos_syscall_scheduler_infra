#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/fixed_point.h"
#define SHIFT_LEFT 14

int int_to_fixed(int a)
{
	return (int)(a << SHIFT_LEFT);
}


int add_fixed (int a, int b)
{
	return a+b;
}


int add_int_fixed (int a, int b)
{
	return (a+(b << SHIFT_LEFT));
}


int sub_fixed (int a, int b)
{
	return a-b;
}


int sub_int_fixed (int a, int b)
{
	return (a-(b << SHIFT_LEFT));
}


int mul_int_fixed (int a, int b)
{
	return a*b;
}


int mul_fixed (int a, int b)
{
	return ((int)((((int64_t) a ) * b >> SHIFT_LEFT)));
}


int div_int_fixed(int a, int b)
{
	return (a/b);
}


int div_fixed (int a, int b)
{
	return ((int)((((int64_t)a) << SHIFT_LEFT) / b));
}


int fixed_to_int (int a)
{
	return (a >> SHIFT_LEFT);
}


int roundoff(int a)
{
	if(a >= 0)
	{
		return ((a + (1<< (SHIFT_LEFT -1))) >> SHIFT_LEFT);
	}
	else
	{
		return ((a - (1 << ( SHIFT_LEFT -1))) >> SHIFT_LEFT);
	}
}








