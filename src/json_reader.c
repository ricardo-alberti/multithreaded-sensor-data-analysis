static bool
generate_unique_key(yyjson_val *payload_root, char *out_key, size_t max_len)
{
    if (!payload_root) return false;

    bool ret = false;
    const char *device_id = yyjson_get_str(yyjson_obj_get(payload_root, "device_id"));
    
    snprintf(out_key, max_len, "%s", device_id ? device_id : "");

    yyjson_val *data_arr = yyjson_obj_get(payload_root, "data");
    if (yyjson_is_arr(data_arr)) 
    {
        yyjson_val *data_item;
        yyjson_arr_iter data_iter;
        yyjson_arr_iter_init(data_arr, &data_iter);

        while ((data_item = yyjson_arr_iter_next(&data_iter))) 
        {
            const char *variable = yyjson_get_str(yyjson_obj_get(data_item, "variable"));
            
            if (!variable || 
                (strcmp(variable, "temperature") != 0 && 
                 strcmp(variable, "humidity") != 0 &&
                 strcmp(variable, "lora_spreading_factor") != 0))
            {
                continue; 
            }

            ret = true;
            double value = yyjson_get_num(yyjson_obj_get(data_item, "value"));
            const char *time_str = yyjson_get_str(yyjson_obj_get(data_item, "time"));

            char part[256];
            snprintf(part, sizeof(part), "|%s:%.2f:%s", variable, value, time_str);
            
            size_t current_len = strlen(out_key);
            if (current_len + strlen(part) < max_len) {
                strcat(out_key, part);
            } else {
                break;
            }
        }
    }

    return ret;
}

static void
set_file_period(File_Info *file, yyjson_val *root)
{
    const yyjson_val *first = yyjson_arr_get_first(root);
    const yyjson_val *last = yyjson_arr_get_last(root);

    if (first && last)
    {
        const char *first_date = yyjson_get_str(
            yyjson_obj_get(first, file->date_field)
        );

        const char *last_date = yyjson_get_str(
            yyjson_obj_get(last, file->date_field)
        );

        if (file->ascending_order)
        {
            format_iso8601_date_br(first_date, file->period_start);
            format_iso8601_date_br(last_date, file->period_end);
        }
        else
        {
            format_iso8601_date_br(last_date, file->period_start);
            format_iso8601_date_br(first_date, file->period_end);
        }
    }
}

void*
read_json(void *arg)
{
    File_Info *file = (File_Info*)arg;
    const char *path_str = file->path;

    yyjson_read_flag flg = YYJSON_READ_INSITU;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_file(path_str, flg, NULL, &err);

    if (!doc)
    {
        fprintf(stderr, "Read error (%u): %s at pos: %ld\n", err.code, err.msg, err.pos);
        return NULL;
    }

    log_push("Processando: %s", path_str);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *item;
    yyjson_arr_iter arr_iter;
    yyjson_arr_iter_init(root, &arr_iter);

    set_file_period(file, root);

    while ((item = yyjson_arr_iter_next(&arr_iter)))
    {
        const yyjson_val *val = 
            yyjson_obj_get(item, "payload") ?
            yyjson_obj_get(item, "payload") :
            yyjson_obj_get(item, "brute_data");

        const char *val_str = yyjson_get_str(val);
        yyjson_doc *payload_doc = yyjson_read(val_str, strlen(val_str), 0);

        char unique_key[128] = { 0 };
        if (!generate_unique_key(yyjson_doc_get_root(payload_doc), unique_key, sizeof(unique_key)))
        {
            log_push(
                "SKIP: %s [ID %d] Sem dados válidos",
                path_str,
                yyjson_get_int(yyjson_obj_get(item, "id"))
            );

            yyjson_doc_free(payload_doc);
            continue;
        }

        if (hashset_contains(&visited, val_str))
        {
            const yyjson_val *id_val =
                yyjson_obj_get(item, "payload_id") ?
                yyjson_obj_get(item, "payload_id") :
                yyjson_obj_get(item, "id");

            log_push(
                "DUP: %s [ID %d] %s",
                path_str,
                yyjson_get_int(id_val),
                unique_key
            );

            yyjson_doc_free(payload_doc);
            continue;
        }

        hashset_add(&visited, val_str);                                       

        record_push(payload_doc);
        file->records_count++;
    }

    log_push("Concluído: %s", path_str);

    yyjson_doc_free(doc);
    return NULL; 
}
