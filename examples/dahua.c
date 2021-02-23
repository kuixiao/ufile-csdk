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

char ul_filepaths[10][50] = {
    "/data/pics/1.jpg",
    "/data/pics/2.jpg",
    "/data/pics/3.jpg",
    "/data/pics/4.jpg",
    "/data/pics/5.jpg",
    "/data/pics/6.jpg",
    "/data/pics/7.jpg",
    "/data/pics/8.jpg",
    "/data/pics/9.jpg",
    "/data/pics/0.jpg"
};

char keynames[10][50] = {
    "1.jpg",
    "2.jpg",
    "3.jpg",
    "4.jpg",
    "5.jpg",
    "6.jpg",
    "7.jpg",
    "8.jpg",
    "9.jpg",
    "0.jpg"
};

char dl_filepaths[10][50] = {
    "/data/pics/download/1.jpg",
    "/data/pics/download/2.jpg",
    "/data/pics/download/3.jpg",
    "/data/pics/download/4.jpg",
    "/data/pics/download/5.jpg",
    "/data/pics/download/6.jpg",
    "/data/pics/download/7.jpg",
    "/data/pics/download/8.jpg",
    "/data/pics/download/9.jpg",
    "/data/pics/download/0.jpg"
};


void upload_file1(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    int key_index = rand() % 10;
    char keyname[10];
    sprintf(keyname,"%d",*tmp);
    FILE *fp = fopen(ul_filepaths[index], "rb");
    if (fp == NULL){
        printf("upload_file:打开文件失败");
        return;
    }
    struct ufile_error error;
    error = ufile_put_file(buckets[0], keyname, "jpg", fp);
    fclose(fp);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("upload_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}

void download_file1(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    char keyname[10];
    sprintf(keyname,"%d",*tmp);
    FILE *fp = fopen(ul_filepaths[index], "rb");
    if (fp == NULL){
        printf("upload_file:打开文件失败");
        return;
    }
    struct ufile_error error;
    error = ufile_put_file(buckets[0], keyname, "jpg", fp);
    fclose(fp);

    FILE *dl_fp = fopen(dl_filepaths[index], "wb");
    if (dl_fp == NULL){
        printf("download_file:打开文件失败");
        return;
    }
    error = ufile_download(buckets[0], keyname, dl_fp, NULL);
    fclose(dl_fp);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("download_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}

void delete_file1(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    char keyname[10];
    sprintf(keyname,"%d",*tmp);
    struct ufile_error error;
    FILE *fp = fopen(ul_filepaths[index], "rb");
    if (fp == NULL){
        printf("delete_file:打开文件失败");
        return;
    }
    error = ufile_put_file(buckets[0], keyname, "jpg", fp);
    fclose(fp);
    error = ufile_delete(buckets[0], keyname);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("delete_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
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
    while (1) {
        struct timeval start1, end1;
        gettimeofday( &start1, NULL );
        threadpool thpool = thpool_init(30);
        int i = 0;
        int index[10000];
        for (; i < 10000; i++) {
            index[i]=i;
            thpool_add_work(thpool, &upload_file1, &index[i]);
            thpool_add_work(thpool, &upload_file1, &index[i]);
            thpool_add_work(thpool, &download_file1, &index[i]);
            
            // thpool_add_work(thpool, &download_file, &index[i]);
            //thpool_add_work(thpool, &head_file, &index[i]);
            //thpool_add_work(thpool, &upload_file, &index[i]);
            //thpool_add_work(thpool, &delete_file, &index[i]);
            // sleep(10);
        }
        thpool_wait(thpool);
        thpool_destroy(thpool);
        gettimeofday( &end1, NULL );
        int timeuse1 = 1000000 * ( end1.tv_sec - start1.tv_sec ) + end1.tv_usec - start1.tv_usec; 
        printf("finish 10000 request!!! time use: %d ms\n", timeuse1/1000);
    }
    
    ufile_sdk_cleanup();
    return 0;
}



void upload_file(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    int key_index = rand() % 10;
    FILE *fp = fopen(ul_filepaths[index], "rb");
    if (fp == NULL){
        printf("upload_file:打开文件失败");
        return;
    }
    struct ufile_error error;
    error = ufile_put_file(buckets[index], keynames[key_index], "jpg", fp);
    fclose(fp);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("upload_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}

void head_file(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );

    int *tmp = (int*)param;
    int index = (*tmp)%10;
    int key_index = rand() % 10;
    struct ufile_error error;
    FILE *ul_fp = fopen(ul_filepaths[key_index], "rb");
    if (ul_fp == NULL){
        printf("head_file:打开文件失败");
        return;
    }
    error = ufile_put_file(buckets[index], keynames[key_index], "jpg", ul_fp);
    fclose(ul_fp);

    struct ufile_file_info file_info;
    error = ufile_head(buckets[index], keynames[key_index], &file_info);
    
    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("head_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}

void download_file(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    int key_index = rand() % 10;
    struct ufile_error error;
    FILE *ul_fp = fopen(ul_filepaths[key_index], "rb");
    if (ul_fp == NULL){
        printf("download_file:打开文件失败");
        return;
    }
    error = ufile_put_file(buckets[index], keynames[key_index], "jpg", ul_fp);
    fclose(ul_fp);

    FILE *dl_fp = fopen(dl_filepaths[key_index], "wb");
    if (dl_fp == NULL){
        printf("download_file:打开文件失败");
        return;
    }
    error = ufile_download(buckets[index], keynames[key_index], dl_fp, NULL);
    fclose(dl_fp);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("download_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}

void delete_file(void *param){
    struct timeval start, end;
    gettimeofday( &start, NULL );
    
    int *tmp = (int*)param;
    int index = (*tmp)%10;
    int key_index = rand() % 10;
    struct ufile_error error;
    FILE *fp = fopen(ul_filepaths[key_index], "rb");
    if (fp == NULL){
        printf("delete_file:打开文件失败");
        return;
    }
    error = ufile_put_file(buckets[index], keynames[key_index], "jpg", fp);
    fclose(fp);
    error = ufile_delete(buckets[index], keynames[key_index]);

    gettimeofday( &end, NULL );
    int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
    if (timeuse/1000 > 500) {
        printf("delete_file -- time: %d, timeuse gt 500ms: %d ms, ", end.tv_sec, timeuse/1000);
        printf("bucket/name: %s/%s \n", buckets[index], keynames[index]);
    }
    return;
}
