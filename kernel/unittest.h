
#pragma once

#define INTERRUPT_PROBLEM 0x1
#define PRINT_PROBLEM     0x2
#define KMALLOC_PROBLEM   0x4
#define KFREE_PROBLEM     0x8


//Various tests designed to catch
//any regressions in the kernel.
//This one runs all of the tests
int runAllTests();

