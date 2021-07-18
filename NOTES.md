# Notes

## Dealing with various types of options

Create a few arrays of options, one for operations, one for operation modifiers, one for global operations, and any other operations that aren't booleans will be manually implemented.

## General flow

The main function will call another function to validate the operation arguments (only one specified) and then set `config.op`. Then, construct an operation class (sync, remove, query, etc.) which will inherit a operation super/abstract class and set the constructed class to a variable with the type as the super/abstract class (not sure if this is possible but try it anyways). Then, inside of each operation class declare a run/execute method which will take the already parsed arguments from the constructor and execute them accordingly.