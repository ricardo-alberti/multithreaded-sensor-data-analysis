#define TIME_BUF 32
#define FILES_NUM 2

#define LOG_PATH "logs/analysis.log"
#define MESSAGE_LEN 64
#define MAX_LOG_MESSAGES 32

typedef struct {
    char messages[MAX_LOG_MESSAGES][MESSAGE_LEN];   // buffer circular
    int head;                                       // onde produtor escreve
    int tail;                                       // onde logger lê                                     
    int count;                                      // menssagens para ler
    pthread_mutex_t lock;
} Log_Buffer;

// TODO: verificar como calcular spreading factor
typedef struct {
    int higherTemp;
    int lowerTemp;
    char higherTempTime[TIME_BUF];
    char lowerTempTime[TIME_BUF];

    int higherUmidity;
    int lowerUmidity;
    char higherUmidityTime[TIME_BUF];
    char lowerUmidityTime[TIME_BUF];

    int higherAirPressure;
    int lowerAirPressure;
    char higherAirPressureTime[TIME_BUF];
    char lowerAirPressureTime[TIME_BUF];

    int sumTemp;
    int sumUmidity;
    int sumAirPressure;
    int recordsCount;

    int batteryInitialUsage;
    int batteryFinalUsage;
} City;

extern City bento;
extern City caxias;
extern Log_Buffer log_buffer;
