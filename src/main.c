#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

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

int
main()
{
    const char* paths[] = { JSON_PATHS };

    bento.lower_temp = INT_MAX;
    bento.higher_temp = INT_MIN;
    bento.lower_airpressure = INT_MAX;
    bento.higher_airpressure = INT_MIN;
    bento.lower_humidity = INT_MAX;
    bento.higher_humidity = INT_MIN;
    bento.period_start = INT64_MAX;
    bento.period_end = 0;

    caxias.lower_temp = INT_MAX;
    caxias.higher_temp = INT_MIN;
    caxias.lower_airpressure = INT_MAX;
    caxias.higher_airpressure = INT_MIN;
    caxias.lower_humidity = INT_MAX;
    caxias.higher_humidity = INT_MIN;
    caxias.period_start = INT64_MAX;
    caxias.period_end = 0;

    analyse(paths, sizeof(paths) / sizeof(paths[0]));
    print_results();

    return 0;
}
