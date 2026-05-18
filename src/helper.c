time_t
parse_iso8601(const char *s)
{
    int year  = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
    int month = (s[5] - '0') * 10 + (s[6] - '0');
    int day   = (s[8] - '0') * 10 + (s[9] - '0');
    int hour  = (s[11] - '0') * 10 + (s[12] - '0');
    int min   = (s[14] - '0') * 10 + (s[15] - '0');
    int sec   = (s[17] - '0') * 10 + (s[18] - '0');

    if (month < 3)
    {
        month += 12;
        year -= 1;
    }

    long long days = (365LL * year) + (year / 4) - (year / 100) + (year / 400) +
                     (306 * (month + 1) / 10) + day - 719561;

    // retornar UTC timestamp
    return (time_t)(days * 86400LL + hour * 3600 + min * 60 + sec);
}

static void
format_iso8601_date_br(const char *iso, char out[11])
{
    out[0] = iso[8];
    out[1] = iso[9];
    out[2] = '/';

    out[3] = iso[5];
    out[4] = iso[6];
    out[5] = '/';

    out[6] = iso[0];
    out[7] = iso[1];
    out[8] = iso[2];
    out[9] = iso[3];

    out[10] = '\0';
}

int
utf8_field_width(const char *str, int field_width)
{
    int extra = 0;

    for (const char *p = str; *p; p++)
    {
        if ((*p & 0xC0) == 0x80)
        {
            extra++;
        }
    }

    return field_width + extra;
}

void
format_time(time_t t, char *out_buf)
{
    if (t == 0)
    {
        strcpy(out_buf, "N/A");
        return;
    }

    struct tm *tm_info = localtime(&t);
    strftime(out_buf, 20, "%d/%m/%Y %H:%M:%S", tm_info);
}

double
calculate_average(double sum, double count)
{
    return (count > 0) ? sum / count : 0.0;
}

double
calculate_execution_time(struct timespec start)
{
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}
