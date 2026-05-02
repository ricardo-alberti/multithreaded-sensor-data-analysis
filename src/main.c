#include <stdio.h>
#include <pthread.h>

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

File_Info files[] = { FILE_INFOS };
const int num_files = sizeof(files) / sizeof(files[0]);

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
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    init_city(&bento);
    init_city(&caxias);

    analyse(files, num_files);

    print_results(start, files, num_files);

    return 0;
}
