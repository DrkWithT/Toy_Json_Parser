#ifndef JSON_TYPES_H
#define JSON_TYPES_H

/// Base C Libraries:

#include <stdlib.h>

/// Enums:

typedef enum json_type {
    INT,
    FLT,
    STR,
    ARR,
    OBJ,
    NUL
} DataType;

/// Specifies relation of a primitive value to something: may be to none, to property, to array.
typedef enum json_prox {
    TO_NONE,  // primitive value is sole JSON data
    TO_PROP,  // primitive value is object property value
    TO_ARR    // primitive value is array item value 
} JsonProx;

#endif