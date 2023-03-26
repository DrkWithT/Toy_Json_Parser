# Toy JSON Parser
## By: Derek Tan

### Summary:
This repository contains code for a homemade recursive JSON parser in C11. Because this is a _toy_ parser, not all JSON standard features are supported. However, this is also a work in progress, so the notes below may not be 100% true.

### Usage:
 - Build: `make clean && make all`
 - Run: `./myjson <json file name>`

### Caveats:
 1. No backslash escaped characters.
 2. No Unicode support.
 3. No booleans yet.
 4. The JSON source is copied into a memory buffer, which may be inefficient use of memory.

### To Do:
 1. Implement JSON data structures! (DONE, needs TESTING)
 2. Implement Lexer.
 3. Implement iterative token parser. (Maybe use a stack for tracking nested levels and names.)
 4. Test run parse result with property tests.
