#ifndef JSON_TYPES_H
#define JSON_TYPES_H

/// Base C Libraries:

#include <stdlib.h>

/// Enums:

typedef enum json_type {
    INT,
    FLO,
    STR,
    ARR,
    OBJ,
    NUL
} DataType;

#endif