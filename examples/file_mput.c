#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";
const char* file_path = "ul-test.txt";
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

    // 初始化分片
    printf("调用 ufile_multiple_upload_init 初始化分片\n");
    struct ufile_mutipart_state state;
    error = ufile_multiple_upload_init(&state, bucket_name, key, mime_type);
    if(UFILE_HAS_ERROR(error.code)){
        ufile_sdk_cleanup();
        printf("调用 ufile_multiple_upload_init 失败，错误信息为：%d, %s\n", error.code, error.message);
        return 1;
    }
    printf("调用 ufile_multiple_upload_init 初始化分片成功\n");

     
    printf("打开文件 %s\n", file_path);
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL){
        fprintf(stderr, "打开文件失败, 错误信息为: %s\n", strerror(errno));
        return 1;
    }
    // 上传分片
    printf("调用 ufile_multiple_upload_part 上传分片\n");
    char *buf = malloc(state.part_size); 
    int i;
    for(i=0; ; i++){
        size_t nc = fread(buf, 1, state.part_size, fp);
        if(nc == 0){
            break;
        }
        error = ufile_multiple_upload_part(&state, buf, nc, i);
        if(UFILE_HAS_ERROR(error.code)){
            printf("调用 ufile_multiple_upload_part 失败，错误信息为：%d, %s\n", error.code, error.message);
            // 分片上传出错，放弃分片
            ufile_multiple_upload_abort(&state);
            ufile_sdk_cleanup();
            return 1;
        }
    }
    printf("调用 ufile_multiple_upload_part 上传分片完成\n");

    // 完成分片上传
    printf("调用 ufile_multiple_upload_finish 完成分片上传\n");
    error = ufile_multiple_upload_finish(&state);
    if(UFILE_HAS_ERROR(error.code)){
        printf("调用 ufile_multiple_upload_finish 失败，错误信息为：%s\n", error.message);
    }else{
        printf("调用 ufile_multiple_upload_finish 成功\n");
    
    }
    ufile_sdk_cleanup();
    return 0;
}