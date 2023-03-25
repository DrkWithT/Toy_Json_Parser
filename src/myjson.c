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
    // Test pure JSON Array:    
    JsonThing *array_thing = JsonThing_Create("nums", ARR, 4); // The last arg is extra for the array case: I pass it anyways!
    Array *array_ref = (Array*)array_thing->root->data.chunk;

    Array_Push(array_ref, ArrayItem_Int(1));
    Array_Push(array_ref, ArrayItem_Int(2));
    Array_Push(array_ref, ArrayItem_Int(4));
    Array_Push(array_ref, ArrayItem_Int(8));

    size_t item_count = Array_Length(array_ref);

    // Test 1: length should be 4!
    printf("nums.length = %zu\n", item_count);

    // Test 2: output should be "1 2 4 8".
    printf("nums: ");

    for (size_t i = 0; i < item_count; i++)
    {
        printf("%zu ", Array_Get(array_ref, i)->data.i);
    }

    printf("\n");
    
    // Test 3: Destroy simple Array:
    JsonThing_Destroy(array_thing);
    free(array_thing);
    array_thing = NULL;

    // Test pure JSON Object:
    // todo

    return 0;
}