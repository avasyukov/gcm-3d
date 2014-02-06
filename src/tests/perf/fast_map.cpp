#include <sys/time.h>
#include <cmath>
#include <ctime>
#include <stdio.h>
#include <unordered_map>
#include <assert.h>

#include <util/FastMap.h>

using namespace std;

#define LEN 10000000
#define N 10

float diff_ns(struct timespec start, struct timespec stop) {
    return (stop.tv_sec-start.tv_sec)*1000000000L+(stop.tv_nsec-start.tv_nsec);
}

void get_stats(float *data, int len, float &mean, float &disp) {
    mean = disp = 0.0;

    for (int i = 0; i < len; i++)
        mean += data[i];

    mean /= len;

    for (int i = 0; i < len; i++)
        disp += (data[i] - mean)*(data[i] - mean);

    disp = sqrt(disp/len);

}

int main() {
    unordered_map<int, int> um;

    srand(time(NULL));

    int *tmp = new int[4*LEN];

    float um_fill_time, um_fill_disp;
    float *um_fill_times = new float[N];

    float um_get_time, um_get_disp;
    float *um_get_times = new float[N];

    float fm_fill_time, fm_fill_disp;
    float *fm_fill_times = new float[N];

    float fm_get_time, fm_get_disp;
    float *fm_get_times = new float[N];

    printf("======= TESTS INFO =======\n");
    printf("data size: %d\n", LEN);
    printf("number of runs: %d\n", N);
    for (int k = 0; k < 2; k++) {
        printf("\n\n\n");
        if (k == 1) {
            printf("======= BAD DATA =========\n");
        } else {
            printf("======= GOOD DATA ========\n");
        }
        printf("\n");
        for (int i = 0; i < N; i++) {
            struct timespec start, stop;

            int m = 1+3*k;
            for (int j = 0; j < m*LEN; j++)
                tmp[j] = j;

            for (int j = m*LEN-1; j > 0; j--) {
                int idx = 1.0*(j-1)*rand()/RAND_MAX;
                int t = tmp[idx];
                tmp[idx] = tmp[j];
                tmp[j] = t;
            }

            um.clear();

            // unordered map fill
            clock_gettime(CLOCK_REALTIME, &start);
            for (int j = 0; j < LEN; j++)
                um[tmp[j]] = j;
            clock_gettime(CLOCK_REALTIME, &stop);

            um_fill_times[i] = diff_ns(start, stop);

            // unordered map get
            clock_gettime(CLOCK_REALTIME, &start);
            for (int j = 0; j < LEN; j++)
                um.at(tmp[j]);
            clock_gettime(CLOCK_REALTIME, &stop);

            um_get_times[i] = diff_ns(start, stop);

            FastMap<int, int> fm(LEN);

            // fast map fill
            clock_gettime(CLOCK_REALTIME, &start);
            for (int j = 0; j < LEN; j++)
                fm.put(tmp[j], j);
            fm.sort();
            clock_gettime(CLOCK_REALTIME, &stop);

            fm_fill_times[i] = diff_ns(start, stop);

            // fast map get
            clock_gettime(CLOCK_REALTIME, &start);
            for (int j = 0; j < LEN; j++)
                fm.get(tmp[j]);
            clock_gettime(CLOCK_REALTIME, &stop);

            fm_get_times[i] = diff_ns(start, stop);

            // check if two maps are equal
            for (int j = 0; j < LEN; j++)
                assert(fm.get(tmp[j]) == j);
        }

        get_stats(um_fill_times, N, um_fill_time, um_fill_disp);
        get_stats(fm_fill_times, N, fm_fill_time, fm_fill_disp);

        get_stats(um_get_times, N, um_get_time, um_get_disp);
        get_stats(fm_get_times, N, fm_get_time, fm_get_disp);



        printf("---------- FILL ----------\n");
        printf("unordered map : %0.2fns (%0.2f%%)\n", um_fill_time, um_fill_disp/um_fill_time);
        printf("fast map: %0.2fns (%0.2f%%)\n", fm_fill_time, fm_fill_disp/um_fill_time);
        printf("ratio   : %0.2f\n", um_fill_time/fm_fill_time);

        printf("----------- GET ----------\n");
        printf("unordered map : %0.2fns (%0.2f%%)\n", um_get_time, um_get_disp/um_get_time);
        printf("fast map: %0.2fns (%0.2f%%)\n", fm_get_time, fm_get_disp/um_get_time);
        printf("ratio   : %0.2f\n", um_get_time/fm_get_time);
    }

    return 0;
}
