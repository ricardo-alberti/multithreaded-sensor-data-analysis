#define _XOPEN_SOURCE 700    // strptime e outras funções de tempo

#include <stdio.h>
#include <pthread.h>
#include <time.h>            // medir o tempo de execução
#include <limits.h>          // INT_MAX e INT_MIN 

#include "yyjson.h"          // biblioteca para leitura de json
#include "analyser.h"        // defines e structs do programa

#include "logger.c"          // gravação de logs
#include "record_resolver.c" // cálculo das estatísticas
#include "json_reader.c"     // desserialização dos arquivos
#include "analyser.c"        // ponto de entrada para análise dos arquivos
#include "display.c"         // exibir resultados

City bento = { 0 };
City caxias = { 0 };
Log_Buffer log_buffer = { 0 };
Record_Buffer rec_buffer = { 0 };

static void
init_city(City *city)
{
    city->lower_temp = INT_MAX;
    city->higher_temp = INT_MIN;
    city->lower_airpressure = INT_MAX;
    city->higher_airpressure = INT_MIN;
    city->lower_humidity = INT_MAX;
    city->higher_humidity = INT_MIN;
    city->period_start = INT64_MAX;
    city->period_end = 0;
}

int
main()
{
    clock_t start = clock();

    const char* paths[] = { JSON_PATHS };
    const int num_files = sizeof(paths) / sizeof(paths[0]);

    init_city(&bento);
    init_city(&caxias);

    analyse(paths, num_files);
    print_results(start, paths, num_files);

    return 0;
}
