#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";

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

    // 删除文件
    printf("正在删除文件 %s\n", key);
    error = ufile_delete(bucket_name, key);
    if(UFILE_HAS_ERROR(error.code)){
        printf("删除文件失败，错误信息为：%s\n", error.message);
    }else{
        printf("删除文件成功\n");
    }
    // 释放初始化SDK时分配的内存
    ufile_sdk_cleanup();
    return 0;
}