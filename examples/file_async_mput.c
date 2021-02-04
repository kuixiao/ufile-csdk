#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "thpool.h" //线程池模块，仅做为测试用，不包含在sdk模块中。

const char* bucket_name = "csdk-create-bucket";
const char* key = "ufile_put_file";
const char* file_path = "ul-test.txt";
const char* mime_type = "plain/text";
const int  jobs = 5;

struct part_data{
    struct ufile_mutipart_state *state;
    char *buf;
    size_t buf_len;
    int pos;
};

// 线程的分片上传任务，param为分片数据
void upload_task(void *param){
    struct part_data *part = (struct part_data*)param;
    struct ufile_error error;
    printf("第 %d 个分片上传中\n", part->pos);
    error = ufile_multiple_upload_part(part->state, part->buf, part->buf_len, part->pos);
    if(UFILE_HAS_ERROR(error.code)){
        printf("调用 upload 失败，错误信息为：%s\n", error.message);
        ufile_multiple_upload_abort(part->state); // 某一分片上传失败，则放弃分片上传
        exit(1); //某一分片上传失败，放弃分片上传后直接退出程序。
    }
    free(part->buf);
    free(part);
}


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
    printf("调用ufile_multiple_upload_init初始化分片\n");
    struct ufile_mutipart_state state;
    error = ufile_multiple_upload_init(&state, bucket_name, key, file_path);
    if(UFILE_HAS_ERROR(error.code)){
        ufile_sdk_cleanup();
        printf("调用 ufile_multiple_upload_init 失败，错误信息为：%d, %s\n", error.code, error.message);
        return 1;
    }
    // 初始化线程池，线程数量为 jobs
    threadpool thpool = thpool_init(jobs); 

    FILE *fp = fopen(file_path, "rb");
    int i;
    for(i=0; ; i++){
        struct part_data *part = malloc(sizeof(struct part_data));
        part->state = &state;
        part->buf = malloc(state.part_size);
        part->buf_len = fread(part->buf, 1, state.part_size, fp);
        part->pos = i;
        if(part->buf_len == 0){
            free(part->buf);
            free(part);
            break;
        }
        // 为每个分片添加一个分片任务到线程池的工作队列中
        thpool_add_work(thpool, &upload_task, part);
    }
    // 等待线程池中的所有分片上传任务完成
    thpool_wait(thpool);
    // 分片上传任务完成，销毁线程池
    thpool_destroy(thpool);
    // 完成分片上传
    error = ufile_multiple_upload_finish(&state);
    if(UFILE_HAS_ERROR(error.code)){
        printf("调用 ufile_multiple_upload_part 失败，错误信息为：%s\n", error.message);
    }else{
        printf("调用 (mput) 分片上传成功\n");
    }
    ufile_sdk_cleanup();
    return 0;
}