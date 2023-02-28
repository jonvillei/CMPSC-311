#include "student.h"

int largest(int array[], int length) {
     int max = 0;
     for (int x = 0; x < length; x++)
     {
	  if (x == 0)
	  {
	       max = array[x];
	  }
	  if (array[x] >= max)
	  {
	       max = array[x];
	  }
	  
     }
     return max;
}

int sum(int array[], int length) {
     int sum = 0;
     for (int x = 0; x < length; x++)
     {
	  sum += array[x];
     }
     return sum;
}

void swap(int *a, int *b) {
     int tmp = *a;
     *a = *b;
     *b = tmp;
}

void rotate(int *a, int *b, int *c) {
     int tmp = *a;
     int tmp2 = *b;
     *a = *c;
     *b = tmp;
     *c = tmp2;
}

void sort(int array[], int length) {
     int index = 0;
     for (int x = 0; x < length; x++)
     {
	  index = x;
	  for (int y = x + 1; y < length; y++)
	  {
	       if (array[y] < array[index])
	       {
		    index = y;
	       }
	  }
	  int tmp = array[index];
	  array[index] = array[x];
	  array[x] = tmp;
     }
     
}

void double_primes(int array[], int length) {
     int primeFlag = 0;
     for (int x = 0; x < length; x++)
     {
	  if (array[x] == 2)
	  {
	       array[x] = array[x] * 2;
	  }
	  else if (array[x] > 1)
	  {
	       {
		    for (int y = 2; y <= array[x] / 2; y++)
		    {
			 if (array[x] % y == 0)
			 {
			      primeFlag = 1;
			      break;
			 }
			 primeFlag = 0;
			 
		    }
		    if (primeFlag == 0)
		    {
			 array[x] = array[x] * 2;
		    }
	       }
	  }
     }
}

void negate_armstrongs(int array[], int length) {
     for (int x = 0; x < length; x++)
     {
	  if (array[x] > 0)
	  {
	       int original = array[x];
	       int sum = 0;
	       int digits = 0;
	       while (original != 0)
	       {
		    original /= 10;
		    digits += 1;
	       }
	       original = array[x];
	       while (original != 0)
	       {
		    int remainder = original % 10;
		    int digitSum = 1;
		    for (int i = 1; i <= digits; i++)
		    {
			 digitSum *= remainder;
		    }
		    sum += digitSum;
		    original /= 10;
	       }
	       if (sum == array[x])
	       {
		    array[x] *= -1;
	       }
	  }
     }
}
