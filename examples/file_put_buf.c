#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>


const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_buf";
const char* buffer = "contents...";
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

    // 调用 ufile_put_buf 上传缓存
    int buf_len = strlen(buffer);
    error = ufile_put_buf(bucket_name, key, mime_type, buffer, buf_len);
    if UFILE_HAS_ERROR(error.code) {
        printf("调用 ufile_put_buf 失败，错误信息为：%s\n", error.message);
    }else{
        printf("调用 ufile_put_buf 成功\n");
    }

    ufile_sdk_cleanup();
    return 0;
}