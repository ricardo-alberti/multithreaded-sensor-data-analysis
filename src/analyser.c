void
analyse(File_Info *files, int num_files)
{
    pthread_t reader_threads[num_files];
    pthread_t logger_thread;
    pthread_t record_resolver_thread;

    // iniciar thread logger
    pthread_create(&logger_thread, NULL, logger, (void*)LOG_PATH);

    // iniciar thread record_resolver
    pthread_create(&record_resolver_thread, NULL, record_resolver, NULL);

    // iniciar threads leitura de jsons
    for (int i = 0; i < num_files; ++i)
    {
        pthread_create(&reader_threads[i], NULL, read_json, (void*)&files[i]);
    }

    // sincronizar threads leitura de jsons
    for (int i = 0; i < num_files; ++i)
    {
        pthread_join(reader_threads[i], NULL);
    }

    // sincronizar record_resolver
    record_push(NULL); // poison pill
    pthread_join(record_resolver_thread, NULL);

    // sincronizar thread logger
    log_push("SHUTDOWN"); // poison pill
    pthread_join(logger_thread, NULL);

    return;
}
