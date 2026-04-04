void*
read_json(void* path)
{
    const char* path_str = (const char*)path;
    char log_msg[MESSAGE_LEN];

    yyjson_read_flag flg = 0;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_file(path_str, flg, NULL, &err);

    if (!doc)
    {
        printf("read error (%u): %s at position: %ld\n", err.code, err.msg, err.pos);
        yyjson_doc_free(doc);
    }

    snprintf(log_msg, sizeof(log_msg), "Processando arquivo %s", path_str);
    log_push(log_msg);    

    yyjson_val *root = yyjson_doc_get_root(doc);

    yyjson_val *item;
    yyjson_arr_iter arr_iter;
    yyjson_arr_iter_init(root, &arr_iter);

    // iterar pelos objetos do array
    while ((item = yyjson_arr_iter_next(&arr_iter)))
    {
        // iterar pelas chaves dos objetos
        yyjson_val *key, *val;
        yyjson_obj_iter obj_iter;
        yyjson_obj_iter_init(item, &obj_iter);

        while ((key = yyjson_obj_iter_next(&obj_iter)))
        {
            val = yyjson_obj_iter_get_val(key);

            const char *key_str = yyjson_get_str(key);
            const char *val_str = yyjson_get_str(val);

            if (strcmp(key_str, "brute_data") == 0 || strcmp(key_str, "payload") == 0)
            {
                // desserializar payload/brute_data
                yyjson_doc *payload = yyjson_read(val_str, strlen(val_str), 0);
                if (payload)
                {
                    // enviar payload/brute_data para a fila de cálculo de estatísticas
                    record_push(payload);
                }
            }
        }
    }

    snprintf(log_msg, sizeof(log_msg), "Arquivo %s Lido", path_str);
    log_push(log_msg);    

    yyjson_doc_free(doc);
    pthread_exit(NULL);
}
