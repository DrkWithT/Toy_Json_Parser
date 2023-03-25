/**
 * @file json_thing.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Implements the main JSON data structure, which represents content.
 * @date 2023-03-25
 */

#include "json_thing.h"

JsonThing *JsonThing_Create(const char* file_name, DataType root_type, size_t opt_prop_count)
{
    JsonThing *result = malloc(sizeof(JsonThing));
    
    if (!result)
        return result;
    
    result->file_name = file_name;
    
    Property *new_root = NULL; 

    switch (root_type)
    {
    case ARR:
        Array *temp_arr = Array_Create();

        if (temp_arr != NULL)
            new_root = Property_Array(NULL, temp_arr);
        break;
    case OBJ:
        Object *temp_obj = Object_Create(opt_prop_count);

        if (temp_obj != NULL)
            new_root = Property_Object(NULL, temp_obj);
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
        Array_Destroy(self->root);
        break;
    case OBJ:
        Object_Destroy(self->root);
        break;
    default:
        // NOTE: primitives cannot be the root!
        break;
    }
}
