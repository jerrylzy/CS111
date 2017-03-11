#!/bin/bash

./lab3a disk-image
sort < bitmap.csv > bitmap
diff -q bitmap sample/bitmap.csv
if [ $? -eq 0 ]
then
    echo "!!! Test bitmap passed !!!"
else
    echo "!!! Test bitmap failed !!!"
fi
rm -f bitmap
sort < group.csv > group
diff -q group sample/group.csv
if [ $? -eq 0 ]
then
    echo "!!! Test group passed !!!"
else
    echo "!!! Test group failed !!!"
fi
rm -f group
diff -q super.csv sample/super.csv
if [ $? -eq 0 ]
then
    echo "!!! Test super passed !!!"
else
    echo "!!! Test super failed !!!"
fi
sort < inode.csv > inode
diff -q inode sample/inode.csv
if [ $? -eq 0 ]
then
    echo "!!! Test inode passed !!!"
else
    echo "!!! Test inode failed !!!"
fi
rm -f inode
sort < directory.csv > directory
diff -q directory sample/directory.csv
if [ $? -eq 0 ]
then
    echo "!!! Test directory passed !!!"
else
    echo "!!! Test directory failed !!!"
fi
rm -f directory
sort < indirect.csv > indirect
diff -q indirect sample/indirect.csv
if [ $? -eq 0 ]
then
    echo "!!! Test indirect passed !!!"
else
    echo "!!! Test indirect failed !!!"
fi
rm -f indirect
