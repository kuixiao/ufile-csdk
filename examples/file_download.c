#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";
const char* file_path = "ufile_download.txt";

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
    FILE *fp = fopen(file_path, "wb");
    if (fp == NULL){
        fprintf(stderr, "打开文件失败, 错误信息为: %s\n", strerror(errno));
        return 1;
    }
    // 下载文件
    printf("调用 ufile_download 下载文件\n");
    error = ufile_download(bucket_name, key, fp, NULL);
    if UFILE_HAS_ERROR(error.code){
        printf("调用 download 失败，错误信息为:%s\n", error.message);
        ufile_sdk_cleanup();
        return 1;
    }
    printf("调用 ufile_download 下载文件成功。\n");
    fclose(fp);
    
    // 释放初始化SDK时分配的内存
    ufile_sdk_cleanup();
    return 0;
}