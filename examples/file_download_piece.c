#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
#define fourMegabyte 1 << 22 //4M

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";
const char* file_path = "ufile_download_piece.txt";

int main(int argc, char *argv[]){
    // 读取配置初始化SDK
    struct ufile_config cfg;
    cfg.public_key = getenv("UFILE_PUBLIC_KEY");
    cfg.private_key = getenv("UFILE_PRIVATE_KEY");
    cfg.bucket_host = getenv("UFILE_BUCKET_HOST");
    cfg.file_host = getenv("UFILE_FILE_HOST");
    struct ufile_error error;
    error = ufile_sdk_initialize(cfg, 0);
    if(UFILE_HAS_ERROR(error.code)){
        printf("初始化 sdk 失败，错误信息为：%s\n", error.message);
        return 1;
    }
    printf("初始化 sdk 成功\n");

    printf("创建本地文件 %s\n", file_path);
    FILE *fp = fopen(file_path, "a+");
    if (fp == NULL){
        fprintf(stderr, "打开文件失败, 错误信息为: %s\n", strerror(errno));
        return 1;
    }

    // 分片下载
    printf("调用 ufile_download_piece 分片下载\n");
    char buf[fourMegabyte]; 
    size_t pos = 0;
    size_t return_size;
    while(!UFILE_HAS_ERROR(error.code)){
        error = ufile_download_piece(bucket_name, key, pos, buf, fourMegabyte, &return_size);
        size_t nc = fwrite(buf, 1, return_size, fp);
        if(return_size < fourMegabyte){ //若实际下载的字节数小于buf大小，则表示已经读到了文件结尾。
            printf("return_size < fourMegabyte: %d < %d \n", return_size, fourMegabyte);
            break;
        }
        pos += fourMegabyte;
        printf("pos = %d, fourMegabyte = %d. 写入 %d 字节\n", pos, fourMegabyte, nc);
        sleep(1); 
    }
    fclose(fp);
    
    if(UFILE_HAS_ERROR(error.code)){
        printf("调用 ufile_download_piece 失败，错误信息为:%s\n", error.message);
        ufile_sdk_cleanup();
        return 1;
    }
    printf("调用 ufile_download_piece 成功。\n");
    // 释放初始化SDK时分配的内存
    ufile_sdk_cleanup();
    return 0;
}