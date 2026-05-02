void*
read_json(void *arg)
{
    File_Info *file = (File_Info*)arg;

    const char *path_str = file->path;
    char log_msg[MESSAGE_LEN];

    yyjson_read_flag flg = YYJSON_READ_INSITU;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_file(path_str, flg, NULL, &err);

    if (!doc)
    {
        fprintf(stderr, "Read error (%u): %s at pos: %ld\n", err.code, err.msg, err.pos);
        return NULL;
    }

    snprintf(log_msg, sizeof(log_msg), "Processando: %s", path_str);
    log_push(log_msg);    

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *item;
    yyjson_arr_iter arr_iter;
    yyjson_arr_iter_init(root, &arr_iter);

    while ((item = yyjson_arr_iter_next(&arr_iter)))
    {
        yyjson_val *val = yyjson_obj_get(item, "payload");
        if (!val) val = yyjson_obj_get(item, "brute_data");

        if (val)
        {
            const char *val_str = yyjson_get_str(val);
            if (val_str)
            {
                yyjson_doc *payload_doc = yyjson_read(val_str, strlen(val_str), 0);
                if (payload_doc)
                {
                    file->records_count++;
                    record_push(payload_doc);
                }
            }
        }
    }

    // criar log de arquivo lido
    snprintf(log_msg, sizeof(log_msg), "Concluído: %s", path_str);
    log_push(log_msg);    

    yyjson_doc_free(doc);
    return NULL; 
}
