/**
 * @file json_thing.c
 * @author (DrkWithT at GitHub)
 * @brief Implements the main JSON data structure, which represents content.
 * @date 2023-03-25
 */

#include "json_thing.h"

JsonThing *JsonThing_Create(DataType root_type, Property *new_root)
{
    JsonThing *result = malloc(sizeof(JsonThing));
    
    if (!result)
        return result;
    
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
    case STR:
        Property_Destroy(self->root);
        break;
    default:
        // NOTE: numeric primitives are statically allocated, so no freeing here.
        break;
    }

    if (self->root != NULL)
    {
        free(self->root);
        self->root = NULL;
    }

    puts("JsonThing_Destroy: Freed root."); // DEBUG
}
