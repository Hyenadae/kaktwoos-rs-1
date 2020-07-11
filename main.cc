#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstring>
#include <inttypes.h>
#include "k2.h"

double time()
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now().time_since_epoch();
    return (double) duration_cast<milliseconds>(now).count() / 1000;
}

int main(int argc, char **argv)
{
    int threads = 0;
    unsigned long long start = 0;
    unsigned long long end = 0;
    unsigned long long chunkSeed = 0;
    int neighbor1 = 0;
    int neighbor2 = 0;
    int neighbor3 = 0;
    int diagonalIndex = 0;
    int height = 0;

    char *strend;

    struct checkpoint_vars {
    unsigned long long offset;
    unsigned long long start;
    int block;
    double elapsed_chkpoint;
    int total_seed_count;
    };

    if (argc % 2 != 1) {
        printf("Failed to parse arguments\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i += 2) {
        const char *param = argv[i];
        if (strcmp(param, "-t") == 0 || strcmp(param, "--threads") == 0) {
            threads = atoi(argv[i + 1]);
        } else if (strcmp(param, "-s") == 0 || strcmp(param, "--start") == 0) {
            sscanf(argv[i + 1], "%" SCNd64, &start);
        } else if (strcmp(param, "-e") == 0 || strcmp(param, "--end") == 0) {
            sscanf(argv[i + 1], "%" SCNd64, &end);
        } else if (strcmp(param, "-cs") == 0 || strcmp(param, "--chunkseed") == 0) {
            sscanf(argv[i + 1], "%" SCNd64, &chunkSeed);
        } else if (strcmp(param, "-n1") == 0 || strcmp(param, "--neighbor1") == 0) {
            neighbor1 = atoi(argv[i + 1]);
        } else if (strcmp(param, "-n2") == 0 || strcmp(param, "--neighbor2") == 0) {
            neighbor2 = atoi(argv[i + 1]);
        } else if (strcmp(param, "-n3") == 0 || strcmp(param, "--neighbor3") == 0) {
            neighbor3 = atoi(argv[i + 1]);
        } else if (strcmp(param, "-di") == 0 || strcmp(param, "--diagonalindex") == 0) {
            diagonalIndex = atoi(argv[i + 1]);
        } else if (strcmp(param, "-ch") == 0 || strcmp(param, "--cactusheight") == 0) {
            height = atoi(argv[i + 1]);
        } else {
            printf("Unknown parameter: %s\n", param);
        }
    }

    fprintf(stderr,"Received work unit: %" SCNd64 "\n", chunkSeed);
    fprintf(stderr,"Data: n1: %d, n2: %d, n3: %d, di: %d, ch: %d\n",
            neighbor1,
            neighbor2,
            neighbor3,
            diagonalIndex,
            height);

    static unsigned long long out[1024 * 1024];
    static int neighbors[] = { neighbor1, neighbor2, neighbor3 };

	k2_params(chunkSeed, neighbors, diagonalIndex, height);
    if (argc < 2) {
        printf("usage: %s [threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    unsigned long long total = end - start;
    unsigned long long count = 0;


    double start_time = time();
    for (unsigned long long i = start; i < end; ) {

        count += k2_start_block(threads, &i, end, out + count);

    }

    double end_time = time();
    double elapsed = end_time - start_time;
    double per_sec = total / elapsed;
    printf("%.2lfs %.2lfm/s\n", elapsed, per_sec / 1000000.0);
    return EXIT_SUCCESS;

}
