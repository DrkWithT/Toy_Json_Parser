/**
 * @file myjson.c
 * @author Derek Tan (DrkWithT at GitHub)
 * @brief Test driver code for my JSON parser.
 * @date 2023-03-25
 */

#include "json_thing.h"
#include <stdio.h>

int main(void)
{
    // Test numeric JSON Array:
    JsonThing *array1 = JsonThing_Create(ARR, 4); // The last arg is extra for the array case: I pass it anyways!
    Array *array_ref = (Array*)array1->root->data.chunk;

    // Test Array append function:
    Array_Push(array_ref, ArrayItem_Int(1));
    Array_Push(array_ref, ArrayItem_Int(2));
    Array_Push(array_ref, ArrayItem_Int(4));
    Array_Push(array_ref, ArrayItem_Int(8));

    // Test Array length function:
    size_t item_count = Array_Length(array_ref);

    // Test length (4)!
    printf("nums.length = %zu\n", item_count);

    // Test output should be "1 2 4 8".
    printf("nums: ");

    for (size_t i = 0; i < item_count; i++)
    {
        printf("%i ", Array_Get(array_ref, i)->data.i);
    }

    printf("\n");
    
    // Test destroy simple Array
    JsonThing_Destroy(array1);
    free(array1);
    array1 = NULL;

    // Test native JSON Object

    char *propname_1 = malloc(sizeof(char) * 6); // first name
    strcpy(propname_1, "first");

    char *first_name = malloc(sizeof(char) * 4);
    strcpy(first_name, "Bob");

    char *propname_2 = malloc(sizeof(char) * 5); // last name
    strcpy(propname_2, "last");

    char *last_name = malloc(sizeof(char) * 4);
    strcpy(last_name, "Cox");

    char *propname_3 = malloc(sizeof(char) * 3); // xy coords
    strcpy(propname_3, "xy");

    Array *coords = Array_Create();
    Array_Push(coords, ArrayItem_Int(69));
    Array_Push(coords, ArrayItem_Int(69));

    JsonThing *object1 = JsonThing_Create(OBJ, 3); // person & coord data 

    Object *object_ref = (Object*)object1->root->data.chunk;

    Object_SetItem(object_ref, propname_1, Property_String(propname_1, first_name));
    Object_SetItem(object_ref, propname_2, Property_String(propname_2, last_name));
    Object_SetItem(object_ref, propname_3, Property_Chunk(propname_3, coords, ARR));

    // Test Object_GetItem:
    const Property *xy_coords = Object_GetItem(object_ref, propname_3);

    const Array *xyarr_ref = (Array*)xy_coords->data.chunk;

    printf("person.coords = [%i, %i]\n",
        Array_Get(xyarr_ref, 0)->data.i,
        Array_Get(xyarr_ref, 1)->data.i
    );

    JsonThing_Destroy(object1);
    free(object1);
    object1 = NULL;

    return 0;
}