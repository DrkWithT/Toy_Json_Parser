# Toy JSON Parser
## By: Derek Tan

### Summary:
This repository contains code for a homemade recursive JSON parser in C. Because this is a _toy_ parser, not all JSON standard features are supported. However, this is also a work in progress, so the notes below may not be 100% true.

### Usage:
 - Build: `make all`
 - Run: `./myjson <json file name>`
 - Clean: `make clean`

### Caveats:
 1. No backslash escaped characters.
 2. No Unicode support.
 3. No booleans yet.
 4. The JSON source is copied into a memory buffer which is inefficient use of memory for larger files.
 5. The parser code has some ugly spaghetti in the parse object function.

### To Do:
 1. Implement JSON data structures! (DONE)
 2. Implement Lexer. (DONE)
 3. Implement recursive token parser. (DONE FOR NOW)
 4. Test run parse result and do structure function tests. (DONE FOR NOW)
