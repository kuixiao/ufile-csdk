#include <stdio.h>
#include <stdlib.h>
 
#define FILE_NAME "demo.txt" // 文件名称
 
int main()
{
	char* szAppendStr = "Text";
 
	// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
	printf("创建本地文件 %s\n", FILE_NAME);
    FILE *fp = fopen(FILE_NAME, "a+");
    if (fp == NULL){
        fprintf(stderr, "打开文件失败\n");
        return 1;
    }
    
    fputs(szAppendStr, fp);
 
    szAppendStr = "file";
	fputs(szAppendStr, fp);
 
	fclose(fp);
	return 0;
}