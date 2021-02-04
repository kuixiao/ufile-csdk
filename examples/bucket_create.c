#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>

const char* bucket_name = "new-bucket-csdk";
const char* region = "tw-tp";
const char* bucket_type = "public";

int main(int argc, char *argv[]){
    struct ufile_config cfg;
    cfg.public_key = getenv("UFILE_PUBLIC_KEY");
    cfg.private_key = getenv("UFILE_PRIVATE_KEY");
    cfg.bucket_host = getenv("UFILE_BUCKET_HOST");
    cfg.file_host = getenv("UFILE_FILE_HOST");

    printf("正在初始化 SDK ......\n");
    struct ufile_error error;
    error = ufile_sdk_initialize(cfg, 1);
    if(UFILE_HAS_ERROR(error.code)){
        ufile_sdk_cleanup();
        printf("初始化 sdk 失败，错误信息为：%s\n", error.message);
        return 1;
    }
    printf("创建 Bucket: bucket_name=%s region=%s bucket_type=%s\n", bucket_name, region, bucket_type);
    error = ufile_bucket_create(bucket_name, region, bucket_type);
    if(UFILE_HAS_ERROR(error.code)){
        printf("创建 bucket 失败，错误信息为：%s\n", error.message);
    }else{
        printf("创建 bucket 成功\n");
    }

    ufile_sdk_cleanup();
    return 0;
}