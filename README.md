# Toy JSON Parser
## By: Derek Tan

### Summary:
This repository contains code for a homemade recursive JSON parser in C. Because this is a toy parser, not all standard JSON features are meant to be supported. I do not intend this to be a production-ready, blazingly fast parser, but I made this for fun. Feel free to fork my work in progress project.

### Usage:
 - Build: `make all`
 - Run: `./myjson <test number>`
    - Test 1: Access Array in an Object.
    - Test 2: Access the first item in a plain Array.
    - Test 3: Access a property of the second Object in a list of Objects.
 - Clean: `make clean`

### Caveats:
 1. No backslash escaped characters.
 2. No Unicode support.
 3. No booleans yet. (I should add this!)
 4. The JSON source is copied into a memory buffer which is inefficient use of memory for larger files _if_ I intend to later support those file sizes.
 5. The parser code has some ugly spaghetti in the parse object function.

### To Do:
 1. ~~Implement JSON data structures!~~
 2. ~~Implement Lexer.~~
 3. ~~Implement recursive token parser.~~
 4. ~~Test run parse result and do structure function tests.~~
 5. ~~Refactor helper function for reading JSON file.~~
 6. Refactor `Parser_Parse_Obj(...)`, specifically the "expect" flags.
 7. Add another test for `null` property values. 
