#ifndef JSON_HASHER_H
#define JSON_HASHER_H

#include <string.h>

#define MAX_JSONKEY_LEN 12
#define BASE_PRIME 5

size_t hash_object_key(const char *key_str);

#endif