#define kB(value) (value * 1024)
#define MB(value) (kB(value) * 1024)

// File Paths
#define FILE_INFOS \
    { "data/mqtt_senzemo_cx_bg.json", 0, {0}, {0}, true,  "created_at"   }, \
    { "data/senzemo_cx_bg.json",      0, {0}, {0}, false, "payload_date" }

/*
#define FILE_INFOS \
    { "data/test_mqtt_senzemo_cx_bg.json", 0, {0}, {0}, true,  "created_at"   }, \
    { "data/test_senzemo_cx_bg.json",      0, {0}, {0}, false, "payload_date" }
*/

#define LOG_PATH "logs/analysis.log"

// Log_Buffer
#define MESSAGE_LEN 64
#define MAX_LOG_MESSAGES 32

// Record_Buffer
#define MAX_RECORDS 32

// HashSet
#define TABLE_SIZE kB(16)

// City
#define SPREADING_FACTOR_COUNT 6

typedef struct {
    char* key;
} HashEntry;

typedef struct {
    HashEntry* table;
    unsigned long size;
    unsigned long count;
} HashSet;

typedef struct {
    char messages[MAX_LOG_MESSAGES][MESSAGE_LEN];   // buffer circular
    int head;                                       // produtor qualquer escreve
    int tail;                                       // logger lê
    int count;                                      // número de menssagens para ler
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Log_Buffer;

typedef struct {
    yyjson_doc* docs[MAX_RECORDS];
    int head;                                       // onde json_reader escreve
    int tail;                                       // onde record_resolver lê
    int count;                                      // menssagens para ler
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Record_Buffer;

typedef struct {
    const char* path;
    int records_count;

    char period_start[11];
    char period_end[11];

    bool ascending_order;
    const char* date_field;
} File_Info;

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

    int spreading_factors[SPREADING_FACTOR_COUNT];
} City;

extern City bento;
extern City caxias;
extern Log_Buffer log_buffer;
extern Record_Buffer rec_buffer;
extern HashSet visited;

extern File_Info files[];
extern const int num_files;
