#include <memory>
#include <cstring>
#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>
using namespace std;

int main()
{
	// ex1 malloc 100 int
	//int *int_arr = (int*) malloc( sizeof(int)*100 );
	// ex2 malloc 1000 double
	// double *double_arr = (double*) malloc( sizeof(double)*1000 );
	// ex3 assign array out of its size
	int *int_arr = (int*) malloc( sizeof(int)*10 );
	
	for(int i=0; i< 100000; i++)
	{
		int_arr[i] = i;
	//	cout<<int_arr[i]<<" ";
	}
	free(int_arr);
	return 0;
} 
