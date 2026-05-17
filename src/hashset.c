void
hashset_init(HashSet* set, unsigned long initial_size)
{
    set->size = initial_size;
    set->count = 0;
    set->table = calloc(set->size, sizeof(HashEntry));
}

unsigned long
hash_function(const char* str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

bool
hashset_add(HashSet* set, const char* key)
{
    if (set->count >= set->size)
    {
        log_push("Tamanho do HashSet excedido");
        exit(1);
    }

    unsigned long index = hash_function(key) % set->size;

    while (set->table[index].key != NULL)
    {
        if (strcmp(set->table[index].key, key) == 0)
        {
            return false;
        }

        index = (index + 1) % set->size;
    }

    set->table[index].key = strdup(key);
    set->count++;
    return true;
}

bool
hashset_contains(HashSet* set, const char* key)
{
    unsigned long index = hash_function(key) % set->size;

    while (set->table[index].key != NULL)
    {
        if (strcmp(set->table[index].key, key) == 0)
        {
            return true;
        }

        index = (index + 1) % set->size;
    }

    return false;
}
