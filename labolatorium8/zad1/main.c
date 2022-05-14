#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "pgma_io.h"
#include <pthread.h>
#include <sys/time.h>

int threads;
int *image;
int w, h, g;
int *processed;

int get_pixel_val(int row, int col){
    return image[col+row*w];
}

void set_pixel_val(int row, int col, int val){
    processed[col+row*w] = 1;
    image[col+row*w] = val;
}

int int_ceil(int numerator, int denominator){
    if (numerator%denominator == 0)
        return numerator/denominator;
    return numerator/denominator+1;
}

int in_thread_range(int val, int thread_no){
    if (val == 0 && thread_no == 0)
        return 1;
    int from = 255.0/threads*thread_no;
    int to = 255.0/threads*(thread_no+1);
    return val > from && val <=to;
}

void *thread_function(void *ptr){
    int thread_no = *((int*)ptr);

    struct timeval start, stop;
    double secs = 0;

    gettimeofday(&start, NULL);

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            if(processed[i*w+j])
                continue;
            int pix_val = get_pixel_val(i,j);
            if (in_thread_range(pix_val, thread_no)){
                set_pixel_val(i,j, 255 - pix_val);
                //printf("%d: %d\n", pix_val, thread_no);
            }
        }
    }

    gettimeofday(&stop, NULL);
    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("thread %d: time taken %f\n",thread_no ,secs);

    return ptr;
}


int main(int argc, char **argv){

    struct timeval start, stop;
    double secs = 0;

    gettimeofday(&start, NULL);


    char *filename = argv[1];
    threads = atoi(argv[2]);

    pgma_read(filename, &w, &h, &g, &image);
    processed = calloc(w*h,sizeof(int));

    pthread_t *t;
    t = malloc(threads*sizeof(pthread_t));
    for (int i = 0; i < threads; ++i) {
        int *ptr = malloc(sizeof(int));
        *ptr = i;
        pthread_create(t+i, NULL, &thread_function, ptr);
    }
    for (int i = 0; i < threads; ++i) {
        pthread_join(t[i], NULL);

    }


    gettimeofday(&stop, NULL);
    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    printf("time taken %f\n",secs);

    pgma_write("res.pgm", "", w, h, g, image);

    return 1;
}