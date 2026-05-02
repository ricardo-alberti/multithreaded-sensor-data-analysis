void
record_push(yyjson_doc* doc)
{
    pthread_mutex_lock(&rec_buffer.lock);

    while (rec_buffer.count == MAX_RECORDS)
    {
        pthread_cond_wait(&rec_buffer.not_full, &rec_buffer.lock);
    }

    rec_buffer.docs[rec_buffer.head] = doc;
    rec_buffer.head = (rec_buffer.head + 1) % MAX_RECORDS;
    rec_buffer.count++;

    pthread_cond_signal(&rec_buffer.not_empty);
    pthread_mutex_unlock(&rec_buffer.lock);
}

yyjson_doc*
record_pop()
{
    pthread_mutex_lock(&rec_buffer.lock);

    while (rec_buffer.count == 0)
    {
        pthread_cond_wait(&rec_buffer.not_empty, &rec_buffer.lock);
    }

    yyjson_doc* doc = rec_buffer.docs[rec_buffer.tail];
    rec_buffer.tail = (rec_buffer.tail + 1) % MAX_RECORDS;
    rec_buffer.count--;

    pthread_cond_signal(&rec_buffer.not_full);
    pthread_mutex_unlock(&rec_buffer.lock);
    return doc;
}

void update_city_stats(City *city, const char *variable, double value, const char *time)
{
    time_t parsed_time = (time[17]-'0')*10 + (time[18]-'0') +
                         (time[14]-'0')*600 + (time[15]-'0')*60; 
    
    if (parsed_time > city->period_end)   city->period_end = parsed_time;
    if (parsed_time < city->period_start) city->period_start = parsed_time;

    switch (variable[0]) 
    {
        case 't': // temperature
            city->sum_temp += value;
            if (value > city->higher_temp) {
                city->higher_temp = value;
                city->higher_temp_time = parsed_time;
            }
            if (value < city->lower_temp) {
                city->lower_temp = value;
                city->lower_temp_time = parsed_time;
            }
            break;
        case 'h': // humidity
            city->sum_humidity += value;
            if (value > city->higher_humidity) {
                city->higher_humidity = value;
                city->higher_humidity_time = parsed_time;
            }
            if (value < city->lower_humidity) {
                city->lower_humidity = value;
                city->lower_humidity_time = parsed_time;
            }
            break;
        case 'a': // airpressure
            city->sum_airpressure += value;
            if (value > city->higher_airpressure) {
                city->higher_airpressure = value;
                city->higher_airpressure_time = parsed_time;
            }
            if (value < city->lower_airpressure) {
                city->lower_airpressure = value;
                city->lower_airpressure_time = parsed_time;
            }
            break;

        case 'b': // batterylevel
            if (parsed_time <= city->period_start) city->battery_initial = value;
            if (parsed_time >= city->period_end)   city->battery_final = value;
            break;

        case 'l': // lora_spreading_factor OU lora_bandwidth
            // checar quinto caracter 's' = lora_spreading
            if (variable[5] == 's') 
            {
                int sf = (int)value;
                if (sf >= 7 && sf <= 12)
                {
                    city->spreading_factors[sf - 7] = 1;
                }
            }
            break;
    }
}

void
process_payload(yyjson_doc *payload_doc, City *city)
{
    city->records_count++;

    yyjson_val *root = yyjson_doc_get_root(payload_doc);
    yyjson_val *data_arr = yyjson_obj_get(root, "data");

    yyjson_val *val;
    yyjson_arr_iter iter;
    yyjson_arr_iter_init(data_arr, &iter);

    while ((val = yyjson_arr_iter_next(&iter)))
    {
        const char *variable = yyjson_get_str(yyjson_obj_get(val, "variable"));
        double value = yyjson_get_num(yyjson_obj_get(val, "value"));
        const char *time = yyjson_get_str(yyjson_obj_get(val, "time"));

        if (variable && time)
        {
            update_city_stats(city, variable, value, time);
        }
    }
}

void*
record_resolver(void* args)
{
    pthread_mutex_init(&rec_buffer.lock, NULL);
    pthread_cond_init(&rec_buffer.not_empty, NULL);
    pthread_cond_init(&rec_buffer.not_full, NULL);

    (void)args;

    for (;;)
    {
        yyjson_doc* doc = record_pop();

        if (doc == NULL)
        {
            break;
        }

        yyjson_val* root = yyjson_doc_get_root(doc);
        const char *dev_id_str = yyjson_get_str(yyjson_obj_get(root, "device_name"));

        City* city = (dev_id_str && dev_id_str[0] == 'C') ? &caxias : &bento;

        process_payload(doc, city);
        yyjson_doc_free(doc);
    }

    pthread_mutex_destroy(&rec_buffer.lock);
    pthread_cond_destroy(&rec_buffer.not_empty);
    pthread_cond_destroy(&rec_buffer.not_full);

    return NULL;
}
