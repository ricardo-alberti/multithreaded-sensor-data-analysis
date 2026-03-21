#include <stdio.h>
#include "yyjson.h"

int
main()
{
    yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_file("data/test.json", flg, NULL, &err);

    if (!doc)
    {
        printf("read error (%u): %s at position: %ld\n", err.code, err.msg, err.pos);
        yyjson_doc_free(doc);
        return 1;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);

    // checar se root é um array
    if (yyjson_is_arr(root))
    {
        yyjson_val *item;
        yyjson_arr_iter arr_iter;
        yyjson_arr_iter_init(root, &arr_iter);

        // iterar pelos objetos do array
        while ((item = yyjson_arr_iter_next(&arr_iter)))
        {
            printf("--- New Log Entry ---\n");

            // iterar pelas chaves dos objetos
            yyjson_val *key, *val;
            yyjson_obj_iter obj_iter;
            yyjson_obj_iter_init(item, &obj_iter);

            while ((key = yyjson_obj_iter_next(&obj_iter)))
            {
                val = yyjson_obj_iter_get_val(key);
                const char *key_name = yyjson_get_str(key);
                
                // printar nome da chave e tipo de valor que possui
                printf("%s: %s\n", key_name, yyjson_get_type_desc(val));
            }
        }
    }

    yyjson_doc_free(doc);
    return 0;
}
