# Test Rationale

## Constructors
For this part of the test I just want to make sure that there is any errors at compile time, making sure that I am doing all the memory management stuff correctly. This test is also check if the constructors work properly in regards to whether all the data is being transferred. The test doesn't compile when I try to use the rvalue to test move, just a note I have checked whether an rvalue is created.

## Modifiers
In modifiers test I just added very basic tests to check whether it does its job properly. I also used some of the examples from the spec, to test whether the merge_replace works properly.
I also check whether the modifiers return the correct boolean while the function has been completed.
In all of the modifiers that throw, I intentionally involved nodes that arent suppose to be their as arguments for the function which should trigger a throw exception.

## Iterators
For iterators it was tricky to test the explicit iterator constructor because it could not be used outside of the implementation of graph. So graph.begin and end will be used to test for both explicit iterator and begin/end as they are the functions that uses it. The ++ and -- operators are tested in conjunction with * to check if the values are correct. The range iterator erase function also checks whether the operators are working properly.

## Extractor
I just used the example in the spec, as it will give the exact spacing that is expected.