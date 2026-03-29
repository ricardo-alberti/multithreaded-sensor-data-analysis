#include <stdio.h>
#include <pthread.h>
#include "yyjson.h"       // biblioteca para leitura de json
#include "analyser.h"     // defines e structs do programa

#include "logger.c"       // gravação de logs
#include "json_reader.c"  // desserialização dos arquivos
#include "analyser.c"     // ponto de entrada para análise dos arquivos
#include "display.c"      // exibir resultados

City bento = { 0 };
City caxias = { 0 };

Log_Buffer log_buffer = { 0 };

int
main()
{
    const char* paths[] = { 
        "data/test_mqtt_senzemo_cx_bg.json",
        "data/test_senzemo_cx_bg.json"
    };

    analyse(paths, FILES_NUM);
    print_results();

    return 0;
}
