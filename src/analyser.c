void analyse(const char** paths, int num_files) {
    pthread_t reader_threads[num_files];
    pthread_t logger_thread;

    // iniciar thread logger
    pthread_create(&logger_thread, NULL, logger, (void*)LOG_PATH);

    // iniciar threads leitura de jsons
    for (int i = 0; i < num_files; ++i)
    {
        pthread_create(&reader_threads[i], NULL, read_json, (void*)paths[i]);
    }

    // sincronizar threads leitura de jsons
    for (int i = 0; i < num_files; ++i)
    {
        pthread_join(reader_threads[i], NULL);
    }

    log_push("[INFO] Threads de leitura sincronizadas com join.");

    // sincronizar thread logger
    log_push("SHUTDOWN");                         // encerrar logger
    pthread_join(logger_thread, NULL);

    return;
}
