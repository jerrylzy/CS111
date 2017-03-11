#!/bin/bash

sort < lab3b_check.txt > check.txt
diff -q check.txt correct.txt
if [ $? -eq 0 ]
then
    echo "!!! Test passed !!!"
else
    echo "!!! Test failed !!!"
fi
rm -f check.txt
