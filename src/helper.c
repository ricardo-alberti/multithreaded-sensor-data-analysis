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
