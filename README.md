# README IOOPM Inlupp 2 {#mainpage}


## Origin of the code
- **Hash table and hash table iterator:** Lucas Assignment 1
- **Linked list and list iterator:** Henrik's Assignment 1
- **Utils:** Lucas C bootstrap labs

## What the program does
The program implements a warehouse management system where users can manage merchandise, storage locations, stock levels, and shopping carts. Users can add, remove, and edit merchandise, replenish stock at different storage locations, and view available inventory. The program also supports creating and managing shopping carts, adding or removing items, calculating the total cost, and completing purchases through checkout, which updates the warehouse inventory accordingly.

This is all done through the users input where they can choose different options from a menu. Every action that removes or alters data will ask for confirmation first.

## Running the program
To compile the program you need to run:

```bash
make main
``` 

`make main` will compile the whole program using the linked_list library and the hash_table library. 

To then run the whole program you need to enter:
```bash
./main
```

This command runs the whole program through an event loop, giving you a list of options of what you can do. You need to have a utils, ui and the business files present, considering this is where the frontend and backend is located.

The program can also read input from a file instead of the keyboard, this is done by typing
```bash
./main < textfile.txt
```

## Compiling and running the tests
To compile and run the tests enter:

```bash
make tests
```
To run with valgrind, enter:

```bash
make memtest
```

If you have compiled the program with make and you want to run it again, you can enter:
```bash
./tests
```

## Test Coverage
Coverage was measured with gcov

Run the coverage reports with:
```bash
make coverage
```
You will get the result : 
 
 ```
File 'business.c'
Lines executed:81.23% of 357
Branches executed:76.27% of 118
```

## Program structure and modularization
The program is divided into four modules.
```
main.c              creates the warehouse, runs the event loop and destroys it
ui.c / .h           the menu and all user interaction
business.c / .h     warehouse logic (no user-interaction)
utils.c / .h        generic input helpers
generic_data/       generic hash table, linked list and iterators

```

The dependencies point downwards, UI uses the business layer, the business layer uses the generic data structures and they know nothing about the warehouse.



## Documentation
We have documented the header files using the Doxygen syntax, describing the purpose of each function along with it's parameters and what it returns.

