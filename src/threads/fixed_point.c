#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/fixed_point.h"
#define SHIFT_LEFT 14

/* convert int to fixed point*/
int int_to_fixed(int a)
{
	return (int)(a << SHIFT_LEFT);
}

/* add 2 fixed point numbers */
int add_fixed (int a, int b)
{
	return a+b;
}

/*Add a fixed point and integer */
int add_int_fixed (int a, int b)
{
	return (a+(b << SHIFT_LEFT));
}

/* Subtract 2 fixed point numbers */
int sub_fixed (int a, int b)
{
	return a-b;
}

/* Subtract int and fixed point numbers */
int sub_int_fixed (int a, int b)
{
	return (a-(b << SHIFT_LEFT));
}

/* Multiply fixed point and int numbers */
int mul_int_fixed (int a, int b)
{
	return a*b;
}

/* Multiply 2 fixed point numbers */
int mul_fixed (int a, int b)
{
	return ((int)((((int64_t) a ) * b >> SHIFT_LEFT)));
}

/* Divide int and fixed point numbers */
int div_int_fixed(int a, int b)
{
	return (a/b);
}

/* Divide fixed point numbers */
int div_fixed (int a, int b)
{
	return ((int)((((int64_t)a) << SHIFT_LEFT) / b));
}

/* Convert fixed point to int */
int fixed_to_int (int a)
{
	return (a >> SHIFT_LEFT);
}

/* round off to nearest int */
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








