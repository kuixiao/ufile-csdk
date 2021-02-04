#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
// #include <errno.h>
// #include <string.h>
// #include "helper.h"

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";
const char* ul_file_path = "ul-test.txt";
const char* mime_type = "plain/text";

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

    // 调用 ufile_put_file 上传文件
    printf("调用 ufile_put_file 上传文件 %s\n", ul_file_path);
    FILE *fp = fopen(ul_file_path, "rb");
    if (fp == NULL){
        fprintf(stderr, "打开文件失败, 错误信息为: %s\n", strerror(errno));
        return 1;
    }
    error = ufile_put_file(bucket_name, key, mime_type, fp);
    if UFILE_HAS_ERROR(error.code) {
        printf("调用 ufile_put_file 失败，错误信息为：%s\n", error.message);
    }else{
        printf("调用 ufile_put_file 成功\n");
    }
    fclose(fp);
    // 释放初始化SDK时分配的内存
    ufile_sdk_cleanup();
    return 0;
}