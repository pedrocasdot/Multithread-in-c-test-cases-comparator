pteste <ntasks> <test_cases.txt>

The program receives the number of tasks to create and a file called test_cases.txt, 
which contains the test cases. Each line represents a test case, consisting of two numbers to be provided to the function under test, 
and a third number which is the expected result.

The program should perform the following operations in the main task:
a) Read the test_cases.txt file.

b) Start the test tasks.

c) When all tasks finish, print the number of passed and failed test cases, as well as the total number of test cases. 

For the failed test cases, also print the input data and the expected and observed values. Then terminate.

The test tasks should perform the following operations in a loop:

a) Execute the next unassigned test case (use a shared variable among the tasks for this purpose). If there are no more test cases, terminate.

b) Perform the test and record the result, whether it passed or failed, along with the result value.

c) Go to step a)

Example of execution:

$ ./pteste 15 test_cases.txt

Results:

Success: 912

Failures: 86

Total cases: 998.

Error in case 404: input (30, 15) expected/observed: 155117520 / -131213633.

Error in case 464: input (31, 13) expected/observed: 206253075 / -124129024.

Error in case 492: input (33, 14) expected/observed: 818809200 / 40051392.

Error in case 511: input (35, 18) expected/observed: 242600354 / 79433710.

Error in case 546: input (35, 15) expected/observed: -1047024136 / -28345986.

...

Note that the number of tasks may be much smaller than the number of test cases, so each task will execute the next test, and each test may take different amounts of time.
