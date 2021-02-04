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
    printf("初始化 sdk 成功\n");

    // 获取文件基本信息
    printf("调用 ufile_head 获取文件基本信息\n");
    struct ufile_file_info file_info;
    error = ufile_head(bucket_name, key, &file_info);
    if UFILE_HAS_ERROR(error.code) {
        printf("调用 head 失败，错误信息为：%s\n", error.message);
    }else{
        printf("文件基本信息为: size=%lld,etag=%s,mime-type=%s\n", file_info.bytes_len, file_info.etag, file_info.mime_type);
    }
    ufile_free_file_info(file_info);
    // 释放初始化SDK时分配的内存
    ufile_sdk_cleanup();
    return 0;
}