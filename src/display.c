void
format_time(time_t t, char *out_buf)
{
    if (t == 0)
    {
        strcpy(out_buf, "N/A");
        return;
    }

    struct tm *tm_info = localtime(&t);
    strftime(out_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}

void
print_city(const char* name, City city)
{
    char hTempT[26], lTempT[26], hHumiT[26], lHumiT[26], hPresT[26], lPresT[26], pStart[26], pEnd[26];
    
    format_time(city.higher_temp_time, hTempT);
    format_time(city.lower_temp_time, lTempT);
    format_time(city.higher_humidity_time, hHumiT);
    format_time(city.lower_humidity_time, lHumiT);
    format_time(city.higher_airpressure_time, hPresT);
    format_time(city.lower_airpressure_time, lPresT);
    format_time(city.period_start, pStart);
    format_time(city.period_end, pEnd);

    float avgTemp = (city.records_count > 0) ? city.sum_temp / city.records_count : 0;
    float avgHumi = (city.records_count > 0) ? city.sum_humidity / city.records_count : 0;
    float avgPres = (city.records_count > 0) ? city.sum_airpressure / city.records_count : 0;

    printf("============================================================\n");
    printf("CITY: %s\n", name);
    printf("Total Records:      %f\n", city.records_count);
    
    printf("Temperature:\n");
    printf("  Higher:           %f°C (at %s)\n", city.higher_temp, hTempT);
    printf("  Lower:            %f°C (at %s)\n", city.lower_temp, lTempT);
    printf("  Average:          %.2f°C\n", avgTemp);
    
    printf("Humidity:\n");
    printf("  Higher:           %f%% (at %s)\n", city.higher_humidity, hHumiT);
    printf("  Lower:            %f%% (at %s)\n", city.lower_humidity, lHumiT);
    printf("  Average:          %.2f%%\n", avgHumi);
    
    printf("Air Pressure:\n");
    printf("  Higher:           %f hPa (at %s)\n", city.higher_airpressure, hPresT);
    printf("  Lower:            %f hPa (at %s)\n", city.lower_airpressure, lPresT);
    printf("  Average:          %.2f hPa\n", avgPres);
    
    printf("Battery:\n");
    printf("  Start Level:      %f%%\n", city.battery_initial);
    printf("  Final Level:      %f%%\n", city.battery_final);

    printf("Period:\n");
    printf("  Period Start:      %s\n", pStart);
    printf("  Period End:        %s\n", pEnd);
}

void
print_results()
{
    print_city("Bento Gonçalves", bento);
    print_city("Caxias do Sul", caxias);
    return;
}
