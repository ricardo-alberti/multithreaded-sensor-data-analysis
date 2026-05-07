// cores
#define BOLD_WHITE  "\033[1;37m"
#define YELLOW      "\033[33m"
#define RESET       "\033[0m"

// separadores
#define SEPARATOR_LONG   "============================================================"
#define SEPARATOR_SHORT  "------------------------------------------------------------"
#define SEPARATOR_TABLE  "-----------------------------------------------------------------------------------------------"

// cabeçalhos de seção
#define SECTION_TEMPERATURE  "TEMPERATURA (°C)"
#define SECTION_HUMIDITY     "UMIDADE (%)"
#define SECTION_AIRPRESSURE  "PRESSÃO ATMOSFÉRICA (hPa)"
#define SECTION_BATTERY      "BATERIA"
#define SECTION_SF           "SPREADING FACTORS UTILIZADOS"
#define SECTION_PERFORMANCE  "DESEMPENHO"

// cabeçalhos de coluna
#define COL_HEADER_METRIC   "Cidade            | Mínima | Data/Hora             | Máxima | Data/Hora             | Média"
#define COL_HEADER_BATTERY  "Cidade            | Inicial (V) | Final (V) | Consumo (V)"
#define COL_HEADER_SF       "Cidade            | SF utilizados"

// ajusta a largura de impressão de strings UTF-8 no terminal
int
utf8_field_width(const char *str, int field_width)
{
    int extra = 0;

    for (const char *p = str; *p; p++)
    {
        if ((*p & 0xC0) == 0x80)
        {
            extra++;
        }
    }

    return field_width + extra;
}

// formata um time_t em uma string legível
void
format_time(time_t t, char *out_buf)
{
    if (t == 0)
    {
        strcpy(out_buf, "N/A");
        return;
    }

    struct tm *tm_info = localtime(&t);
    strftime(out_buf, 20, "%d/%m/%Y %H:%M:%S", tm_info);
}

// calcula a média
double
calculate_average(double sum, double count)
{
    return (count > 0) ? sum / count : 0.0;
}

// calcula o tempo de execução do programa
double
calculate_execution_time(struct timespec start)
{
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}

// imprime separador
void
print_separator(const char *color, const char *sep)
{
    printf("%s%s%s\n", color, sep, RESET);
}

// imprime cabeçalho
void
print_header()
{
    print_separator(YELLOW, SEPARATOR_LONG);
    printf(BOLD_WHITE "ANÁLISE DE DADOS DOS SENSORES - CityLivingLab\n" RESET);
    printf(BOLD_WHITE "Processamento utilizando pthreads\n" RESET);
    print_separator(YELLOW, SEPARATOR_LONG);
    printf("\n");
}

// imprime informações de arquivo
void
print_file_info(const File_Info *file)
{
    char period_start_str[26];
    char period_end_str[26];

    format_time(file->period_start, period_start_str);
    format_time(file->period_end, period_end_str);

    printf("Arquivo analisado: %s\n", file->path);
    printf("Total de registros processados: %d\n", file->records_count);
    printf(
        "Período analisado: %s a %s\n\n",
        period_start_str,
        period_end_str
    );
}

// imprime cabeçalho de seção
void
print_section_header(const char *title, const char *col_header, int is_wide_table)
{
    // título
    print_separator(YELLOW, SEPARATOR_SHORT);
    printf(BOLD_WHITE "%s\n" RESET, title);
    print_separator(YELLOW, SEPARATOR_SHORT);

    // cabeçalhos de coluna
    printf("%s\n", col_header);
    print_separator(YELLOW, is_wide_table ? SEPARATOR_TABLE : SEPARATOR_SHORT);
}

// imprimem linhas de dados
void
print_metric_row(const char *name, double min_val, time_t min_time, double max_val, time_t max_time, double avg)
{
    char min_t[26], max_t[26];

    format_time(min_time, min_t);
    format_time(max_time, max_t);

    printf(
        "%-*s | %-6.2f | %-21s | %-6.2f | %-21s | %-6.2f\n",
        utf8_field_width(name, 17),
        name,
        min_val,
        min_t,
        max_val,
        max_t,
        avg
    );
}

void
print_battery_row(const char *name, const City *city)
{
    double battery_consumption = city->battery_initial - city->battery_final;

    printf(
        "%-*s | %-11.2f | %-9.2f | %-10.2f\n",
        utf8_field_width(name, 17),
        name,
        city->battery_initial,
        city->battery_final,
        battery_consumption
    );
}

void
print_sf_row(const char *name, const City *city)
{
    char buffer[64] = "";

    for (int i = 0; i < 6; i++)
    {
        if (city->spreading_factors[i])
        {
            char sf[8];

            sprintf(
                sf, "%sSF%d",
                buffer[0] ? ", " : "",
                i + 7
            );

            strcat(buffer, sf);
        }
    }

    printf(
        "%-*s | %s\n",
        utf8_field_width(name, 17),
        name,
        buffer[0] ? buffer : "Nenhum"
    );
}

// imprimem seções
void
print_temperature_section(const char *name, const City *city)
{
    double avg_temp = calculate_average(city->sum_temp, city->records_count);

    print_metric_row(
        name,
        city->lower_temp,
        city->lower_temp_time,
        city->higher_temp,
        city->higher_temp_time,
        avg_temp
    );
}

void
print_humidity_section(const char *name, const City *city)
{
    double avg_humidity = calculate_average(city->sum_humidity, city->records_count);

    print_metric_row(
        name,
        city->lower_humidity,
        city->lower_humidity_time,
        city->higher_humidity,
        city->higher_humidity_time,
        avg_humidity
    );
}

void
print_airpressure_section(const char *name, const City *city)
{
    double avg_airpressure = calculate_average(city->sum_airpressure, city->records_count);

    print_metric_row(
        name,
        city->lower_airpressure,
        city->lower_airpressure_time,
        city->higher_airpressure,
        city->higher_airpressure_time,
        avg_airpressure
    );
}

void
print_performance_section(double exec_time, File_Info *files, int num_files)
{
    print_separator(YELLOW, SEPARATOR_SHORT);
    printf(BOLD_WHITE "%s\n" RESET, SECTION_PERFORMANCE);
    print_separator(YELLOW, SEPARATOR_SHORT);

    int num_threads = num_files + 2;

    printf("Tempo total de execução: %.2f segundos\n", exec_time);
    printf("Threads utilizadas: %d\n", num_threads);

    int thread_id = 1;

    // threads de leitura
    for (int i = 0; i < num_files; i++, thread_id++)
    {
        printf(" - Thread %d: leitura de %s\n", thread_id, files[i].path);
    }

    // threads fixas
    printf(" - Thread %d: registro de logs\n", thread_id++);
    printf(" - Thread %d: processamento de registros\n", thread_id++);

    printf("\nArquivo de log gerado: %s\n\n", LOG_PATH);

    print_separator(YELLOW, SEPARATOR_LONG);
    printf("Processamento finalizado com sucesso.\n");
    print_separator(YELLOW, SEPARATOR_LONG);
}

// impressão principal
void
print_results(struct timespec start_time, File_Info *files, int num_files)
{
    // informações dos arquivos
    print_header();
    for (int i = 0; i < num_files; i++)
    {
        print_file_info(&files[i]);
    }

    // temperatura
    print_section_header(SECTION_TEMPERATURE, COL_HEADER_METRIC, 1);
    print_temperature_section("Caxias do Sul", &caxias);
    print_temperature_section("Bento Gonçalves", &bento);

    // umidade
    printf("\n\n");
    print_section_header(SECTION_HUMIDITY, COL_HEADER_METRIC, 1);
    print_humidity_section("Caxias do Sul", &caxias);
    print_humidity_section("Bento Gonçalves", &bento);

    // pressão atmosférica
    printf("\n\n");
    print_section_header(SECTION_AIRPRESSURE, COL_HEADER_METRIC, 1);
    print_airpressure_section("Caxias do Sul", &caxias);
    print_airpressure_section("Bento Gonçalves", &bento);

    // bateria
    printf("\n\n");
    print_section_header(SECTION_BATTERY, COL_HEADER_BATTERY, 0);
    print_battery_row("Caxias do Sul", &caxias);
    print_battery_row("Bento Gonçalves", &bento);

    // spreading factors
    printf("\n\n");
    print_section_header(SECTION_SF, COL_HEADER_SF, 0);
    print_sf_row("Caxias do Sul", &caxias);
    print_sf_row("Bento Gonçalves", &bento);

    // desempenho
    printf("\n\n");
    print_performance_section(
        calculate_execution_time(start_time),
        files,
        num_files
    );
}
