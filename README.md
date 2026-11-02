# README IOOPM Inlupp 2

Hash table and hash table iterator comes from Lucas Assignment 1
Linked list and list iterator comes from Henrik's Assignment 1
Utils comes from Lucas C bootstrap labs

## What the program does
The program implements a warehouse management system where users can manage merchandise, storage locations, stock levels, and shopping carts. Users can add, remove, and edit merchandise, replenish stock at different storage locations, and view available inventory. The program also supports creating and managing shopping carts, adding or removing items, calculating the total cost, and completing purchases through checkout, which updates the warehouse inventory accordingly.

## Running the program
To run the program you need to 

* Run: 
make main 

Which will compile the whole program using the linked_list library and the hash_table library. 

* Then run :
./main

This command runs the whole program through an event loop, giving you a list of options of what you can do. You need to have a utils, ui and the business files present, considering this is where the frontend and backend is located.

## Compiling and running the tests
To compile the tests, you need to run compile and run through the Makefile.

* Run:
make tests

* To run with valgrind:
make memtest

## Test Coverage
Coverage was measured with gcov

Run the coverage reports with:

make coverage

* You will get the result : 
 
File 'business.c'

Lines executed:81.23% of 357

Branches executed:76.27% of 118

## Documentation
We have documented the header files using the Doxygen syntax, describing the purpose of each function along with it's parameters and what it returns.

