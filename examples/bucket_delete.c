#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>

const char* bucket_name = "new-bucket-csdk";

int main(int argc, char *argv[]){
    struct ufile_config cfg;
    cfg.public_key = getenv("UFILE_PUBLIC_KEY");
    cfg.private_key = getenv("UFILE_PRIVATE_KEY");
    cfg.bucket_host = "api.ucloud.cn";
    cfg.file_host = "cn-bj.ufileos.com";

    printf("正在初始化 SDK ......\n");
    struct ufile_error error;
    error = ufile_sdk_initialize(cfg, 1);
    if(UFILE_HAS_ERROR(error.code)){
        ufile_sdk_cleanup();
        printf("初始化 sdk 失败，错误信息为：%s\n", error.message);
        return 1;
    }

    error = ufile_bucket_delete(bucket_name);
    if (UFILE_HAS_ERROR(error.code)) {
        printf("删除 bucket 失败，错误信息为：%s\n", error.message);
    }else{
        printf("删除 bucket 成功\n");
    }

    ufile_sdk_cleanup();
    return 0;
}