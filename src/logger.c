void
log_push(const char *fmt, ...)
{
    pthread_mutex_lock(&log_buffer.lock);

    while (log_buffer.count == MAX_LOG_MESSAGES)
    {
        pthread_cond_wait(&log_buffer.not_full, &log_buffer.lock);
    }

    // formatar mensagem usando va_list
    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buffer.messages[log_buffer.head], MESSAGE_LEN, fmt, args);
    va_end(args);

    log_buffer.head = (log_buffer.head + 1) % MAX_LOG_MESSAGES;
    log_buffer.count++;

    pthread_cond_signal(&log_buffer.not_empty);
    pthread_mutex_unlock(&log_buffer.lock);
}

void*
logger(void* path)
{
    const char* file_path = (const char*)path;
    FILE* log = fopen(file_path, "a");

    if (!log)
    {
        perror("Erro ao abrir log");
        return NULL;
    }

    pthread_mutex_init(&log_buffer.lock, NULL);
    pthread_cond_init(&log_buffer.not_empty, NULL);
    pthread_cond_init(&log_buffer.not_full, NULL);

    log_push("Logger thread iniciada, log em: %s", file_path);

    for (;;)
    {
        pthread_mutex_lock(&log_buffer.lock);

        // sleep se não houver mensagens, acorda quando produtor sinalizar
        while (log_buffer.count == 0)
        {
            pthread_cond_wait(&log_buffer.not_empty, &log_buffer.lock);
        }

        char msg[MESSAGE_LEN];
        strncpy(msg, log_buffer.messages[log_buffer.tail], MESSAGE_LEN - 1);

        log_buffer.tail = (log_buffer.tail + 1) % MAX_LOG_MESSAGES;
        log_buffer.count--;

        // acordar produtor se estava esperando por espaço
        pthread_cond_signal(&log_buffer.not_full);
        pthread_mutex_unlock(&log_buffer.lock);

        if (strcmp(msg, "SHUTDOWN") == 0) break; 

        // formatar para adicionar timestamp
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        struct tm *tm_info = localtime(&ts.tv_sec);
        char f_buf[32], t_buf[64];
        strftime(f_buf, sizeof(f_buf), "%H:%M:%S", tm_info);
        snprintf(t_buf, sizeof(t_buf), "%s.%03ld", f_buf, (long)(ts.tv_nsec / 1000000));

        fprintf(log, "[%s] %s\n", t_buf, msg);
    }

    pthread_mutex_destroy(&log_buffer.lock);
    pthread_cond_destroy(&log_buffer.not_empty);
    pthread_cond_destroy(&log_buffer.not_full);
    
    fclose(log);
    return NULL;
}
