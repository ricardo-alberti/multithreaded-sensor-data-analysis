#include <pthread.h>
#include <stdarg.h>          // para funções variádicas (log_push)

#include "yyjson.h"          // biblioteca para leitura de json
#include "analyser.h"        // defines e structs do programa

#include "helper.c"          // funções utilitárias 
#include "logger.c"          // gravação de logs
#include "hashset.c"         // implementação de hash set para evitar processar arquivos repetidos
#include "record_resolver.c" // cálculo das estatísticas
#include "json_reader.c"     // desserialização dos arquivos
#include "analyser.c"        // ponto de entrada para análise dos arquivos
#include "display.c"         // exibir resultados

City bento = { 0 };
City caxias = { 0 };
Log_Buffer log_buffer = { 0 };
Record_Buffer rec_buffer = { 0 };
HashSet visited = { 0 };

File_Info files[] = { FILE_INFOS };
const int num_files = sizeof(files) / sizeof(files[0]);

static void
city_init(City *city)
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

    hashset_init(&visited, kB(TABLE_SIZE));
    city_init(&bento);
    city_init(&caxias);

    analyse(files, num_files);

    print_results(start, files, num_files);

    return 0;
}
