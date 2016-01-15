
#include <stdio.h>
int add(int var1,int var2){return(var1 + var2);}
int sub(int var1,int var2){return(var1 - var2);}
int mul(int var1,int var2){return(var1 * var2);}
int div(int var1,int var2){return(var1 / var2);}


int main()
{
       int a = add(3,5);
       int b = sub(3,5);
       int c = mul(3,5);
       int d = div(3,5);
	printf("Results <- %i - %i - %i - %i ->",a,b,c,d);
	return 0;
}
