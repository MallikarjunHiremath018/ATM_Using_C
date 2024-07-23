#include<stdio.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
	time_t curtime;
	time(&curtime);
	printf("current time: %s",ctime(&curtime));
	return(0);
}
