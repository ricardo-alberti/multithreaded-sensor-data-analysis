#define kB(value) (value * 1024)
#define MB(value) (kB(value) * 1024)

/*
#define JSON_PATHS \
    "data/mqtt_senzemo_cx_bg.json", \
    "data/senzemo_cx_bg.json"
*/

#define JSON_PATHS \
    "data/test_mqtt_senzemo_cx_bg.json", \
    "data/test_senzemo_cx_bg.json"

#define LOG_PATH "logs/analysis.log"

#define MESSAGE_LEN 64
#define MAX_LOG_MESSAGES MB(4)
#define MAX_RECORDS MB(16)

typedef struct {
    char messages[MAX_LOG_MESSAGES][MESSAGE_LEN];   // buffer circular
    int head;                                       // produtor qualquer escreve
    int tail;                                       // logger lê                                     
    int count;                                      // número de menssagens para ler
    pthread_mutex_t lock;
} Log_Buffer;

typedef struct {
    yyjson_doc* docs[MAX_RECORDS]; 
    int head;                                       // onde json_reader escreve
    int tail;                                       // onde record_resolver lê
    int count;                                      // menssagens para ler
    pthread_mutex_t lock;
} Record_Buffer;

// TODO: verificar como calcular spreading factor
typedef struct {
    double higher_temp;
    double lower_temp;
    time_t higher_temp_time;
    time_t lower_temp_time;

    double higher_humidity;
    double lower_humidity;
    time_t higher_humidity_time;
    time_t lower_humidity_time;

    double higher_airpressure;
    double lower_airpressure;
    time_t higher_airpressure_time;
    time_t lower_airpressure_time;

    double sum_temp;
    double sum_humidity;
    double sum_airpressure;
    double records_count;

    double battery_initial;
    time_t period_start;

    double battery_final;
    time_t period_end;
} City;

extern City bento;
extern City caxias;
extern Log_Buffer log_buffer;
extern Record_Buffer rec_buffer;
