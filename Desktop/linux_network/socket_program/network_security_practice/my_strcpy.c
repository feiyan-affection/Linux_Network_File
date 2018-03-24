#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *my_strcpy(char *str_dset, const char *str_src)
{
	char *p_return = str_dset;

	//check out the pointer whether is NULL
	if(str_dset == NULL || str_src == NULL)
	{
		fprintf(stderr, "NULL pointer\n");
		return NULL;
	} 

	while((*str_dset++ = *str_src++) != '\0')
		;

	return p_return;
}

int main(int argc, char const *argv[])
{
	char string1[32];
	char string2[32];
	int c;
	int i = 0;


	printf("please input your string:");
	//check out the length of string
	while(((c = getchar()) != '\n') && (c != EOF) && (i < 31))
	{
		string2[i] = c;
		i++;
	}
	string2[i] = '\0';

	//check out the return value
	if(my_strcpy(string1, string2) == NULL)
	{
		fprintf(stderr, "return from my_strcpy\n");
		exit(1);
	}

	printf("string1:%s\n", string1);
	printf("string2:%s\n", string2);

	return 0;
}