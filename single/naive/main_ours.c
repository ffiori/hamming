/*
 * Test harness for the k-mismatches algorithms.
 */

#ifdef __linux__
#define _GNU_SOURCE
#include <sched.h>
#endif

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

/* Implemented by the test algorithms */
extern void prep(unsigned char *P, size_t m);
extern size_t exec(unsigned char *T, size_t n);

/*
 * Benchmarking functions.
 * Peltola, H. Towards faster string matching. PhD thesis, 2014.
 */
static struct tms start;
static void startclock()
{
//~ #ifdef __linux__
#if 0 //not working in server
    int onln;  /* number of processors online */
    if ((onln = sysconf(_SC_NPROCESSORS_ONLN)) > 1) {
        int i;
        pid_t pid = getpid();
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(onln-1, &cpu_set);  /* CPU numbering starts from 0 */
        if (sched_setaffinity(pid, sizeof(cpu_set), &cpu_set) != 0) {
            printf("Setting CPU affinity failed.\n");
            abort();
        }

        if (sched_getaffinity(pid, sizeof(cpu_set), &cpu_set) != 0) {
            printf("Getting CPU affinity failed.\n");
            abort();
        }
        for (i = onln-2; i >= 0; i--) {
            if (CPU_ISSET(i, &cpu_set)) {
                printf("Failed to unset cpu affinity for cpu %d\n", i);
            }
        }
        if (!CPU_ISSET(onln-1, &cpu_set)) {
            printf("Failed to set cpu affinity for cpu %d\n", onln-1);
        }
    }
#endif
    times(&start);
}

static clock_t stopclock()
{
    struct tms t;
    times(&t);
    return t.tms_utime - start.tms_utime;
    /* we should also keep watch on tms_stime */
}

/*
 * Read the contents of the stream `in' and store the content length to *size.
 * The function supports non-seekable streams such as stdin.
 *
 * The returned pointer should be freed with free().
 */
static void *read_stream(FILE *in, size_t *size)
{
    unsigned char *buf, *new;
    size_t allocated, length;

    /* Read data into a dynamic buffer (to support non-seekable stream) */
    allocated = 256;
    if (!(buf = malloc(allocated)))
        return NULL;
    length = 0;
    while (!feof(in)) {
        length += fread(&buf[length], sizeof(char), allocated-length, in);
        if (ferror(in))
            goto fail;

        while (length >= allocated) {
            /* Double the buffer size */
            allocated *= 2;
            if (!(new = realloc(buf, allocated)))
                goto fail;
            buf = new;
        }
    }

    /* Truncate unused bytes at the end of the buffer  */
    if (0 < length && length < allocated) {
        if (!(new = realloc(buf, length)))
            goto fail;
        buf = new;
    }

    if (size) *size = length;
    return buf;

fail:
    free(buf);
    return NULL;
}

/*
 * Command-line interface.
 */
int main(int argc, char *argv[])
{
    FILE *in;
    int c, k, p, e, hz;
    size_t m, n, mmin, mmax, np, i, occ;
    unsigned char *P, *T;
    clock_t tprep, texec;

    /* Parse arguments */
    k = 0;
    p = e = 1;
    while ((c = getopt(argc, argv, "k:p:e:")) != -1) {
        switch (c) {
        case 'k': sscanf(optarg, "%d", &k); break;
        case 'p': sscanf(optarg, "%d", &p); break;
        case 'e': sscanf(optarg, "%d", &e); break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default: abort();
        }
    }
    if (optind != argc-1 || e < 1 || p < 1 || k < 0) {
        fprintf(stderr, "Usage: %s [-k err] [-p preps] [-e execs] "
                        "textfile <patterns\n", argv[0]);
        return 1;
    }

    /* Read input text and patterns */
    in = fopen(argv[argc-1], "rb");
    if (!in) {
        fprintf(stderr, "Cannot open `%s' for reading.\n", argv[argc-1]);
        return 2;
    }
    T = read_stream(in, &n);

    fclose(in);
    unsigned char *new;
    //Add 32 bytes of padding at the end of the text; this is required by certain algs (e.g. ans)
    if (!(new = realloc(T, n + 32))) {
      free(T);
      fprintf(stderr, "Error reading and/or padding past the end`%s'.\n", argv[argc-1]);
      return 3;
    }
    T = new;

    if ((P = read_stream(stdin, &m)) == NULL) {
        fprintf(stderr, "Error reading patterns from stdin.\n");
        free(T);
        return 4;
    }

    /* Run the algorithm for each pattern */
    tprep = texec = 0;
    i = mmin = mmax = np = occ = 0;
    while (i < m) {
        int r;
        size_t j, len;
        for (j = i; P[j] != '\n' && j < m; j++);
        if (i == j) {
            fprintf(stderr, "Warning: empty pattern\n");
            goto next;
        }
        if (P[j] != '\n') {
            fprintf(stderr, "Warning: EOL missing\n");
        }
        P[j] = '\0';

        mmin = (!mmin || j-i < mmin) ? j-i : mmin;
        mmax = (!mmax || j-i > mmax) ? j-i : mmax;
        if (n < j-i) {
            fprintf(stderr, "Warning: pattern longer than the text\n");
            goto next;
        }

        /* Preprocessing */
        len = j-i;
        startclock();
        for (r = 0; r < p; r++) {
            prep(&P[i], len);
        }
        tprep += stopclock();

        /* Searching */
        startclock();
        occ += exec(T, n);
        for (r = 1; r < e; r++) {
            (void)exec(T, n);
        }
        texec += stopclock();

next:   i = j+1;
        np++;
    }

    /* Report results */
    hz = sysconf(_SC_CLK_TCK);
    fprintf(stdout, "alg=%s T=%s(%lu) np=%lu", argv[0], argv[argc-1],
            (unsigned long)n, (unsigned long)np);
    fprintf(stdout, " m=%lu", (unsigned long)mmin);
    if (mmin != mmax) fprintf(stdout, "..%lu", (unsigned long)mmax);
    fprintf(stdout, " k=%d occ=%lu", k, (unsigned long)occ);
    fprintf(stdout, " p=%.4fs(%lu/%d) e=%.4fs(%lu/%d)\n",
            (double)tprep / (hz*p), (unsigned long)tprep, p,
            (double)texec / (hz*e), (unsigned long)texec, e);

    /* Cleanup */
    free(P);
    free(T);
    return 0;
}
