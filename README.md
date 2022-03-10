# A simple Network File System

A simple Network File System

Author: Minh-Hieu Nguyen [mnguyen19@seattleu.edu](mnguyen19@seattleu.edu)

## My own rating

Rating: A, B, C, D, or F?

**A**

Explanation:

The Network File System is operating correctly with all the provided functionalities. All the test cases, provided in the instructions and customized below in this project, have been passed without error. The point of testing is to cover all the possible cases, hence all the error codes have been introduced in the test cases.

## Test cases

All the test cases provided in the instruction have been tested and passed without error. Below is the list of customized test cases:

```text
Test case #1: mkdir test
Result: success

Test case #2: create testfile1
Result: success

Test case #3: create testfile11
Result: 504 File name is too long

Test case #4: ls
Result: test/ testfile1

Test case #5: append testfile1 a-very-simple-network-filesystem
Result: success

Test case #6: cat testfile1
Result: a-very-simple-network-filesystem

Test case #7: append test invalid
Result: 501 File is a directory

Test case #8: cd testfile1
Result: 500 File is not a directory

Test case #9: cat test
Result: 501 File is a directory

Test case #10: stat test
Result:
Directory name: test
Directory block: 2

Test case #11: stat testfile1
Result:
Inode block: 3
Bytes in file:32
Number of blocks: 2
First block: 4

Test case #12: mkdir test1
Result: success

Test case #13: ls
Result: test/ testfile1 test1/

Test case #14: cd test
Result: success

Test case #15: ls
Result: empty folder

Test case #16: create testfile
Result: success

Test case #17: append testfile anothertestfile
Result: success

Test case #18: cat testfile
Result: anothertestfile

Test case #19: head testfile 7
Result: another

Test case #20: head testfile 20
Result: anothertestfile

Test case #21: mkdir dir1
Result: success

Test case #22: mkdir dir2
Result: success

Test case #23: mkdir dir3
Result: success

Test case #24: mkdir dir4
Result: success

Test case #25: mkdir dir5
Result: success

Test case #26: mkdir dir6
Result: success

Test case #27: mkdir dir7
Result: success

Test case #28: mkdir dir8
Result: success

Test case #29: mkdir dir9
Result: success

Test case #30: mkdir dir10
Result: 506 Directory is full

Test case #31: home
Result: success

Test case #32: ls
Result: test/ testfile1 test1/

Test case #33: rmdir test
Result: 507 Directory is not empty

Test case #34: mkdir rmdir
Result: success

Test case #35: rmdir rmdir
Result: success

Test case #36: ls
Result: test/ testfile1 test1/

Test case #37: quit
Result: (Connection terminated at both client and server)
```
