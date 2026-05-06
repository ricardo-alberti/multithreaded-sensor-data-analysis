unsigned long
hash_function(const char* str)
{
    unsigned long hash = 5381;
    int c;

    str = (char *)str;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % TABLE_SIZE;
}

bool
hashset_add(HashSet* set, const char* key)
{
    unsigned long index = hash_function(key);
    Node *current = set->buckets[index];

    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return false; 
        }

        current = current->next;
    }

    Node *new_node = malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->next = set->buckets[index];
    set->buckets[index] = new_node;
    return true;
}

bool
hashset_contains(HashSet* set, const char* key)
{
    unsigned long index = hash_function(key);
    Node *current = set->buckets[index];

    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return true;
        }

        current = current->next;
    }
    return false;
}
