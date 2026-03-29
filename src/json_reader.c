void*
read_json(void* path)
{
    yyjson_read_flag flg = 0;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_file((const char*)path, flg, NULL, &err);

    if (!doc)
    {
        printf("read error (%u): %s at position: %ld\n", err.code, err.msg, err.pos);
        yyjson_doc_free(doc);
    }

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

        printf("---- new record ----\n");
        while ((key = yyjson_obj_iter_next(&obj_iter)))
        {
            val = yyjson_obj_iter_get_val(key);
            const char *key_str = yyjson_get_str(key);

            printf("%s: %s\n", key_str, yyjson_get_type_desc(val));
        }
    }

    yyjson_doc_free(doc);
    pthread_exit(NULL);
}
