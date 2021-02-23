#include "../lib/api.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "thpool.h" //线程池模块，仅做为测试用，不包含在sdk模块中。

char buckets[10][50] = {
    "csdk-for-dahua",
    "csdk-for-dahua1",
    "csdk-for-dahua2",
    "csdk-for-dahua3",
    "csdk-for-dahua4",
    "csdk-for-dahua5",
    "csdk-for-dahua6",
    "csdk-for-dahua7",
    "csdk-for-dahua8",
    "csdk-for-dahua9"
};

void head_file(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    int *index = (int*)param;
    int i = (*index)%10;
    char keyname[10];
    sprintf(keyname,"%d",*index);
    struct ufile_error error;
    struct ufile_file_info file_info;
    error = ufile_head(buckets[i], keyname, &file_info);
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    printf("%s/%s", buckets[i], keyname);
    printf(" timeuse=%d ms \n", timeuse/1000);
    if (timeuse/1000 > 1000) {
        printf("timeout!!! url:%s.%s, index=%d", buckets[i], keyname, i);
        printf("timeuse=%d ms ", timeuse/1000);
        printf("start=%d ", start);
        printf("end=%d ", end);
        printf("index=%d \n", *index);
    }
    return;
}

int main(int argc, char *argv[]){
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
    int index[1000000];
    threadpool thpool = thpool_init(30);
    int i;
    for (i=0; i < 1000000; i++) {
        index[i]=i;
        thpool_add_work(thpool, &head_file, &index[i]);
        usleep(1000*(i%100));
    }
    printf("finish thpool_add_work!\n");
    thpool_wait(thpool);
    thpool_destroy(thpool);

    ufile_sdk_cleanup();
    return 0;
}