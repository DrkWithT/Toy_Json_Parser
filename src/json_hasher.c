/**
 * @file json_hasher.c
 * @author Derek Tan (you@domain.com)
 * @brief Implements the JSON object key hashing function.
 * @date 2023-03-25
 */

#include "json_hasher.h"

size_t hash_object_key(const char *key_str)
{
    size_t len = strnlen(key_str, MAX_JSONKEY_LEN);
    size_t place_val = 1;
    size_t hash_num = 0;
    size_t char_val = 0;

    for (size_t str_i = 0; str_i < len; str_i++)
    {
        char_val = (size_t)key_str[str_i];

        hash_num += place_val * char_val;

        place_val *= BASE_PRIME;
    }

    return hash_num;
}
