# What is it?

This is a small header-only library which creates an event driven array type.
Instead of accessing per element, one accesses an element with messages or events.
With this approach it is easy to use this type in a multithreaded context.

## Build instructions

This is a CMake library with no external dependencies, except the Google Test Suite for the tests which can be disabled.
It uses features such as concepts, which is why it requires the c++23 standard.

## Examples

For examples check the unit-tests which should cover all possibilities of using this library.