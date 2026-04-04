time_t
parse_iso8601(const char *timestamp)
{
    struct tm tm = {0};
    
    if (strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm) == NULL)
    {
        return 0;
    }

    return mktime(&tm);
}

void
record_push(yyjson_doc* doc)
{
    for (;;) 
    {
        pthread_mutex_lock(&rec_buffer.lock);

        if (rec_buffer.count < MAX_RECORDS) 
        {
            rec_buffer.docs[rec_buffer.head] = doc;
            rec_buffer.head = (rec_buffer.head + 1) % MAX_RECORDS;
            rec_buffer.count++;

            pthread_mutex_unlock(&rec_buffer.lock);
            return;
        }

        pthread_mutex_unlock(&rec_buffer.lock);
        sched_yield(); 
    }
}

yyjson_doc*
record_pop()
{
    for (;;)
    {
        pthread_mutex_lock(&rec_buffer.lock);
        
        if (rec_buffer.count > 0)
        {
            yyjson_doc* doc = rec_buffer.docs[rec_buffer.tail];
            rec_buffer.tail = (rec_buffer.tail + 1) % MAX_RECORDS;
            rec_buffer.count--;
            
            pthread_mutex_unlock(&rec_buffer.lock);
            return doc;
        }
        
        pthread_mutex_unlock(&rec_buffer.lock);
        sched_yield(); 
    }
}

void
update_city_stats(City *city, const char *variable, double value, const char *time)
{
    time_t parsed_time = parse_iso8601(time);

    if (parsed_time > city->period_end)
    {
        city->period_end = parsed_time;
    }
    if (parsed_time < city->period_start)
    {
        city->period_start = parsed_time;
    }

    if (strcmp(variable, "temperature") == 0)
    {
        city->sum_temp += value;
        if (value > city->higher_temp)
        {
            city->higher_temp = value;
            city->higher_temp_time = parsed_time;
        }
        if (value < city->lower_temp)
        {
            city->lower_temp = value;
            city->lower_temp_time = parsed_time;
        }
    } 
    else if (strcmp(variable, "humidity") == 0)
    {
        city->sum_humidity += value;
        if (value > city->higher_humidity)
        {
            city->higher_humidity = value;
            city->higher_humidity_time = parsed_time;
        }
        if (value < city->lower_humidity)
        {
            city->lower_humidity = value;
            city->lower_humidity_time = parsed_time;
        }
    }
    else if (strcmp(variable, "airpressure") == 0)
    {
        city->sum_airpressure += value;
        if (value > city->higher_airpressure)
        {
            city->higher_airpressure = value;
            city->higher_airpressure_time = parsed_time;
        }
        if (value < city->lower_airpressure)
        {
            city->lower_airpressure = value;
            city->lower_airpressure_time = parsed_time;
        }
    }
    else if (strcmp(variable, "batterylevel") == 0)
    {
        if (city->period_start == parsed_time)
        {
            city->battery_initial = value;
        }
        if (city->period_end == parsed_time)
        {
            city->battery_final = value;
        }
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
    (void)args;

    // busy wait
    for (;;)
    {
        yyjson_doc* doc = record_pop();

        if (doc == NULL)
        {
            break;
        }

        yyjson_val* root = yyjson_doc_get_root(doc);

        yyjson_val *dev_id_val = yyjson_obj_get(root, "device_name");
        const char *dev_id_str = yyjson_get_str(dev_id_val);
        
        City* city = &bento;
        if (strcmp(dev_id_str, "Caxias - Praça (S2)") == 0)
        {
            city = &caxias;
        }

        process_payload(doc, city);

        yyjson_doc_free(doc); 
    }

    return NULL;
}
