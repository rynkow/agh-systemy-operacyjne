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

int get_pixel_val(int row, int col){
    return image[col+row*w];
}

void set_pixel_val(int row, int col, int val){
    image[col+row*w] = val;
}

int int_ceil(int numerator, int denominator){
    if (numerator%denominator == 0)
        return numerator/denominator;
    return numerator/denominator+1;
}

void *thread_function(void *ptr){
    int thread_no = *((int*)ptr);

    struct timeval start, stop;
    double secs = 0;

    gettimeofday(&start, NULL);

    int from = (thread_no-1)*int_ceil(w,threads);
    int to = thread_no*int_ceil(w,threads)-1;
    //printf("%d: %d %d\n", thread_no, from, to);

    for (int i = 0; i < h; ++i) {
        for (int j = from; j <= to && j < w; ++j) {
            int pix_val = get_pixel_val(i,j);
            set_pixel_val(i,j, 255 - pix_val);
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
    //printf("w = %d\n", w);

    pthread_t *t;
    t = malloc(threads*sizeof(pthread_t));
    for (int i = 0; i < threads; ++i) {
        int *ptr = malloc(sizeof(int));
        *ptr = i+1;
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