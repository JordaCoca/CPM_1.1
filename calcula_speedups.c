#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

/*
 * Calcula speedups automáticamente a partir de archivos:
 *   orca_2, orca_4, orca_8, orca_16, orca_32, orca_64, orca_128
 *   teen_2, teen_4, teen_8, teen_16, teen_32
 *
 * Tiempos secuenciales dados:
 *   Orca: 17.3 s
 *   Teen: 8.8 s
 *
 * El programa busca una línea tipo:
 *   26.13user 0.00system 0:13.08elapsed 199%CPU ...
 *
 * y extrae el tiempo justo antes de la palabra "elapsed".
 */

typedef struct {
    const char *machine;
    double seq_time;
    const int *threads;
    int nthreads;
} MachineConfig;

static double parse_time_token(const char *token)
{
    /*
     * Acepta formatos habituales de /usr/bin/time:
     *   SS.ss
     *   M:SS.ss
     *   H:MM:SS.ss
     */
    int colon_count = 0;
    for (const char *p = token; *p; p++) {
        if (*p == ':') colon_count++;
    }

    if (colon_count == 0) {
        return atof(token);
    }
    else if (colon_count == 1) {
        int min = 0;
        double sec = 0.0;
        if (sscanf(token, "%d:%lf", &min, &sec) == 2) {
            return min * 60.0 + sec;
        }
    }
    else if (colon_count == 2) {
        int hour = 0, min = 0;
        double sec = 0.0;
        if (sscanf(token, "%d:%d:%lf", &hour, &min, &sec) == 3) {
            return hour * 3600.0 + min * 60.0 + sec;
        }
    }

    return -1.0;
}

static int extract_elapsed_from_line(const char *line, double *elapsed)
{
    const char *p = strstr(line, "elapsed");
    if (p == NULL) return 0;

    /* Retrocedemos hasta encontrar el inicio del token anterior a "elapsed". */
    const char *end = p;
    const char *start = end;

    while (start > line && !isspace((unsigned char)*(start - 1))) {
        start--;
    }

    char token[128];
    size_t len = (size_t)(end - start);

    if (len == 0 || len >= sizeof(token)) return 0;

    memcpy(token, start, len);
    token[len] = '\0';

    double value = parse_time_token(token);
    if (value < 0.0) return 0;

    *elapsed = value;
    return 1;
}

static int read_elapsed_time(const char *filename, double *elapsed)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0;
    }

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        double value;
        if (extract_elapsed_from_line(line, &value)) {
            *elapsed = value;
            found = 1;
            /* Si hubiese varias ejecuciones en el mismo archivo, nos quedamos con la última. */
        }
    }

    fclose(fp);
    return found;
}

static void process_machine(MachineConfig cfg, double *global_inverse_sum, int *global_count)
{
    double inverse_sum = 0.0;
    int valid_count = 0;

    printf("\n=== %s ===\n", cfg.machine);
    printf("Tiempo secuencial usado: %.3f s\n", cfg.seq_time);
    printf("%-10s %-14s %-14s %-14s\n", "Archivo", "Threads", "Tiempo(s)", "Speedup");
    printf("------------------------------------------------------\n");

    for (int i = 0; i < cfg.nthreads; i++) {
        char filename[128];
        int th = cfg.threads[i];
        double elapsed = 0.0;

        snprintf(filename, sizeof(filename), "%s_%d", cfg.machine, th);

        if (!read_elapsed_time(filename, &elapsed)) {
            printf("%-10s %-14d %-14s %-14s\n", filename, th, "NO ENCONTRADO", "-");
            continue;
        }

        double speedup = cfg.seq_time / elapsed;

        printf("%-10s %-14d %-14.4f %-14.4f\n", filename, th, elapsed, speedup);

        inverse_sum += 1.0 / speedup;
        valid_count++;

        *global_inverse_sum += 1.0 / speedup;
        (*global_count)++;
    }

    if (valid_count > 0) {
        double harmonic = valid_count / inverse_sum;
        printf("------------------------------------------------------\n");
        printf("Media harmonica speedup %s: %.4f\n", cfg.machine, harmonic);
    } else {
        printf("No se ha podido calcular la media harmonica de %s.\n", cfg.machine);
    }
}

int main(void)
{
    int orca_threads[] = {2, 4, 8, 16, 32, 64, 128};
    int teen_threads[] = {2, 4, 8, 16, 32};

    MachineConfig configs[] = {
        {"orca", 17.3, orca_threads, (int)(sizeof(orca_threads) / sizeof(orca_threads[0]))},
        {"teen", 8.8, teen_threads, (int)(sizeof(teen_threads) / sizeof(teen_threads[0]))}
    };

    double global_inverse_sum = 0.0;
    int global_count = 0;

    for (int i = 0; i < (int)(sizeof(configs) / sizeof(configs[0])); i++) {
        process_machine(configs[i], &global_inverse_sum, &global_count);
    }

    if (global_count > 0) {
        double global_harmonic = global_count / global_inverse_sum;
        printf("\n======================================================\n");
        printf("Media harmonica GLOBAL del speedup: %.4f\n", global_harmonic);
        printf("Resultados validos usados: %d\n", global_count);
        printf("======================================================\n");
    } else {
        printf("\nNo se ha encontrado ningun archivo valido.\n");
    }

    return 0;
}
