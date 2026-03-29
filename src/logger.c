void
log_push(char* message)
{
    // busy wait
    while (log_buffer.count == MAX_LOG_MESSAGES);

    pthread_mutex_lock(&log_buffer.lock);

    strncpy(log_buffer.messages[log_buffer.head], message, MESSAGE_LEN - 1);
    log_buffer.messages[log_buffer.head][MESSAGE_LEN - 1] = '\0';

    log_buffer.head = (log_buffer.head + 1) % MAX_LOG_MESSAGES;
    log_buffer.count++;

    pthread_mutex_unlock(&log_buffer.lock);
}

char*
log_pop()
{
    // busy wait
    while (log_buffer.count == 0);

    pthread_mutex_lock(&log_buffer.lock);

    char* msg = log_buffer.messages[log_buffer.tail];

    log_buffer.tail = (log_buffer.tail + 1) % MAX_LOG_MESSAGES;
    log_buffer.count--;

    pthread_mutex_unlock(&log_buffer.lock);

    return msg;
}

void*
logger(void* path) 
{
    const char* file_path = (const char*)path;
    
    FILE* log = fopen(file_path, "a");

    if (log == NULL)
    {
        perror("Erro ao abrir log");
        pthread_exit(NULL);
    }

    fprintf(log, "[INFO] Thread de logging iniciada com sucesso.\n");

    // busy wait
    for (;;)
    {
        const char* message = log_pop();

        if (strcmp(message, "SHUTDOWN") == 0)
        {
            break; 
        }

        fprintf(log, "%s\n", message);
    }
    
    fprintf(log, "[INFO] Thread de logging encerrada.\n");
    fclose(log); 
    pthread_exit(NULL);
}
