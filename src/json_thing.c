/**
 * @file json_thing.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Implements the main JSON data structure, which represents content.
 * @date 2023-03-25
 */

#include "json_thing.h"

JsonThing *JsonThing_Create(DataType root_type, size_t opt_prop_count)
{
    JsonThing *result = malloc(sizeof(JsonThing));
    
    if (!result)
        return result;
    
    Property *new_root = NULL; 
    Array *temp_arr = NULL;
    Object *temp_obj = NULL;

    switch (root_type)
    {
    case ARR:
        temp_arr = Array_Create();

        if (temp_arr != NULL)
            new_root = Property_Chunk(NULL, temp_arr, ARR);
        break;
    case OBJ:
        temp_obj = Object_Create(opt_prop_count);

        if (temp_obj != NULL)
            new_root = Property_Chunk(NULL, temp_obj, OBJ);
        break;
    default:
        break;
    }

    result->root = new_root;

    return result;
}

void JsonThing_Destroy(JsonThing *self)
{
    if (!self->root)
        return;
    
    DataType root_type = self->root->type;

    switch (root_type)
    {
    case ARR:
        Array_Destroy((Array*)self->root->data.chunk);
        break;
    case OBJ:
        Object_Destroy((Object*)self->root->data.chunk);
        break;
    default:
        // NOTE: primitives cannot be the root!
        break;
    }
}
