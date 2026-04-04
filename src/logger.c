void
log_push(char* message)
{
    for (;;)
    {
        pthread_mutex_lock(&log_buffer.lock);

        if (log_buffer.count < MAX_LOG_MESSAGES)
        {
            strncpy(log_buffer.messages[log_buffer.head], message, MESSAGE_LEN - 1);
            log_buffer.messages[log_buffer.head][MESSAGE_LEN - 1] = '\0';
            
            log_buffer.head = (log_buffer.head + 1) % MAX_LOG_MESSAGES;
            log_buffer.count++;
            
            pthread_mutex_unlock(&log_buffer.lock);
            return;
        }

        pthread_mutex_unlock(&log_buffer.lock);
        sched_yield(); 
    }
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

    // busy wait
    for (;;)
    {
        pthread_mutex_lock(&log_buffer.lock);

        if (log_buffer.count == 0) 
        {
            pthread_mutex_unlock(&log_buffer.lock);
            sched_yield();
            continue;
        }

        char* msg = log_buffer.messages[log_buffer.tail];
        log_buffer.tail = (log_buffer.tail + 1) % MAX_LOG_MESSAGES;
        log_buffer.count--;
        
        pthread_mutex_unlock(&log_buffer.lock);

        if (strcmp(msg, "SHUTDOWN") == 0)
        {
            break; 
        }

        // cronometrar log
        char t_buf[64]; 
        char f_buf[32]; 

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        struct tm *tm_info = localtime(&ts.tv_sec);
        strftime(f_buf, sizeof(f_buf), "%H:%M:%S", tm_info);

        snprintf(t_buf, sizeof(t_buf), "%s.%03ld", f_buf, (long)(ts.tv_nsec / 1000000));

        fprintf(log, "[%s] %s\n", t_buf, msg);  
    }
    
    fclose(log); 
    pthread_exit(NULL);
}
