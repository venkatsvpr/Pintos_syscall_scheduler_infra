#ifndef FIXEDPOINT_H


#define FIXEDPOINT_H


#include <debug.h>
#include <list.h>
#include <stdint.h>

#define SHIFT_LEFT 14

/* convert int to fixed point */
int int_to_fixed(int a);
/* add 2 fixed point numbers */
int add_fixed (int a, int b);
/* Add a fixed point and integer */
int add_int_fixed (int a, int b);
/* Subtract 2 fixed point numbers */
int sub_fixed (int a, int b);
/*Subtract a fixed point and int */
int sub_int_fixed (int a, int b);
/* Multiply a int and fixed point*/
int mul_int_fixed (int a, int b);
/* Multiply 2 fixed point */
int mul_fixed (int a, int b);
/* Divide int and fixed point */
int div_int_fixed(int a, int b);
/* Divide 2 fixed point */
int div_fixed (int a, int b);
/* Convert fixed to int */
int fixed_to_int (int a);
/* round off to nearest int */
int roundoff(int a);
#endif  
