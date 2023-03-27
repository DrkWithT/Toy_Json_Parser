/**
 * @file json_data.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Implements basic data structures for JSON data representation.
 * @date 2023-03-24
 */

#include "json_data.h"

/// ArrayItem:

ArrayItem *ArrayItem_Int(int value)
{
    ArrayItem *result = malloc(sizeof(ArrayItem));

    if (!result)
        return result;

    result->type = INT;
    result->data.i = value;
    result->next = NULL;

    return result;
}

ArrayItem *ArrayItem_Float(float value)
{
    ArrayItem *result = malloc(sizeof(ArrayItem));

    if (!result)
        return result;
    
    result->type = FLO;
    result->data.f = value;
    result->next = NULL;

    return result;
}

ArrayItem *ArrayItem_String(char *str)
{
    ArrayItem *result = malloc(sizeof(ArrayItem));

    if (!result)
        return result;
    
    result->type = STR;
    result->data.str = str;
    result->next = NULL;

    return result;
}

ArrayItem *ArrayItem_Chunk(void *chunk, DataType type)
{
    ArrayItem *result = malloc(sizeof(ArrayItem));

    if (!result)
        return result;

    if (type == ARR || type == OBJ)
    {
        result->data.chunk = chunk;
        result->type = type;
        result->next = NULL;
    }
    else
    {
        result->data.chunk = NULL;
        result->type = NUL;
        result->next = NULL;
    }

    return result;
}

void ArrayItem_Destroy(ArrayItem *self)
{
    if (self->type == STR && self->data.str != NULL)
    {
        free(self->data.str);
        self->data.str = NULL;
    }
    else if (self->type == ARR && self->data.chunk != NULL)
    {
        Array_Destroy((Array*)self->data.chunk);
        self->data.chunk = NULL;
    }
    else if (self->type == OBJ && self->data.chunk != NULL)
    {
        Object_Destroy((Object*)self->data.chunk);
        self->data.chunk = NULL;
    }
}

/// Array:

Array *Array_Create()
{
    Array *result = malloc(sizeof(Array));

    if (!result)
        return result;

    result->head = NULL;
    result->length = 0;
    
    return result;
}

void Array_Destroy(Array *self)
{
    size_t arr_length = self->length;
    ArrayItem *targetPtr = self->head;

    if (arr_length < 1)
        return;

    while (self->head != NULL)
    {
        targetPtr = targetPtr->next;

        ArrayItem_Destroy(self->head);
        free(self->head);
        
        self->head = targetPtr;
    }
    
    self->head = NULL;
    self->length = 0;
}

size_t Array_Length(const Array *self) { return self->length; }

const ArrayItem *Array_Get(const Array *self, size_t pos)
{
    size_t countdown = pos;

    if (countdown < 0 || countdown >= self->length)
        return NULL;
    
    ArrayItem *trackPtr = self->head;

    while (countdown > 0)
    {
        trackPtr = trackPtr->next;
        countdown--;

        if (!trackPtr)
            break;
    }

    return trackPtr;
}

void Array_Push(Array *self, ArrayItem *item)
{
    if (self->length < 1)
    {
        // move assign by pointer to avoid extra copies... the item must be allocated prior!
        self->head = item;
        self->length = 1;
        return;
    }

    ArrayItem *trackPtr = self->head;

    while (trackPtr->next != NULL)
    {
        trackPtr = trackPtr->next;
    }

    trackPtr->next = item;
    self->length++;
}

/// Object:

Object *Object_Create(size_t slots)
{
    Object *result = malloc(sizeof(Object));

    if (!result)
        return result;

    // allocate bucket array with max load 0.5 (no collisions I guess... YOLO!)
    size_t bucket_slots = slots << 1;
    result->buckets = malloc(sizeof(Property*) * bucket_slots);
    
    if (result->buckets != NULL)
    {
        for (size_t i = 0; i < bucket_slots; i++)
            result->buckets[i] = NULL;
        
        result->bucket_count = bucket_slots;
    }
    else
        result->bucket_count = 0; // invalidate hash table on failed allocation

    return result;
}

void Object_Destroy(Object *self)
{
    if (!self->buckets)
        return;
    
    size_t bucket_count = self->bucket_count;

    for (size_t curr = 0; curr < bucket_count; curr++)
    {
        if (!self->buckets[curr])
            continue;
        
        Property_Destroy((Property*)self->buckets[curr]);
        free(self->buckets[curr]);
        self->buckets[curr] = NULL;
    }
    
    free(self->buckets);
    self->buckets = NULL;
}

void Object_SetItem(Object *self, const char *key, Property *prop_val)
{
    size_t bucket = hash_object_key(key) % self->bucket_count;

    if (!self->buckets[bucket])
    {
        self->buckets[bucket] = prop_val;
    }
}

const Property *Object_GetItem(Object *self, const char *key)
{
    size_t bucket = hash_object_key(key) % self->bucket_count;

    return (Property*)self->buckets[bucket];
}

/// Property:
Property *Property_Int(char *name, int value)
{
    Property *result = malloc(sizeof(Property));

    if (!result)
        return result;
    
    result->name = name;
    result->type = INT;
    result->data.i = value;

    return result;
}

Property *Property_Float(char *name, float value)
{
    Property *result = malloc(sizeof(Property));

    if (!result)
        return result;
    
    result->name = name;
    result->data.f = value;
    result->type = FLO;

    return result;
}

Property *Property_String(char *name, char *value)
{
    Property *result = malloc(sizeof(Property));

    if (!result)
        return result;
    
    result->name = name;
    result->data.str = value;
    result->type = STR;

    return result;
}

Property *Property_Chunk(char *name, void *value, DataType type)
{
    Property *result = malloc(sizeof(Property));

    if (!result)
        return result;
    
    result->name = name;
    result->data.chunk = value;
    result->type = type;

    return result;
}

void Property_Destroy(Property *self)
{
    // check type for whether there is dynamic memory to free
    switch (self->type)
    {
    case INT:
    case FLO:
        break;
    case STR:
        if (self->data.str != NULL)
        {
            free(self->data.str);
            self->data.str = NULL;
        }
        break;
    case ARR:
        if (self->data.chunk != NULL)
        {
            Array_Destroy((Array*)self->data.chunk);
            free(self->data.chunk);
            self->data.chunk = NULL;
        }
        break;
    case OBJ:
        if (self->data.chunk != NULL)
        {
            Object_Destroy((Object*)self->data.chunk);
            free(self->data.chunk);
            self->data.chunk = NULL;
        }
        break;
    default:
        break;
    }

    // free property name's dynamic C-String
    if (self->name != NULL)
    {
        free(self->name);
        self->name = NULL;
    }
}

int Property_AsInt(const Property *self) { return self->data.i; }

float Property_AsFloat(const Property *self) { return self->data.f; }

const char *Property_AsStr(const Property *self) { return self->data.str; }

const void *Property_AsChunk(const Property *self, DataType *type_flag)
{
    *type_flag = self->type;

    return self->data.chunk; // non-primitive typed data!
}
