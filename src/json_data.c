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

void ArrayItem_Destroy(ArrayItem *self)
{
    if (self->type == STR && self->data.str != NULL)
    {
        free(self->data.str);
        self->data.str = NULL;
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

    do {
        trackPtr = trackPtr->next;
        countdown--;
    } while (trackPtr->next != NULL && countdown != 0);

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

Property *Property_Array(char *name, Array *value)
{
    Property *result = malloc(sizeof(Property));

    if (!result)
        return result;
    
    result->name = name;
    result->data.arr = value;
    result->type = ARR;

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
        Array_Destroy(self->data.arr);
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

const Array *Property_AsArr(const Property *self) { return self->data.arr; }