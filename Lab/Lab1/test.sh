#!/bin/bash

chmod 755 simpsh

TEMP_DIR="lab1a_test"
rm -rf $TEMP_DIR
mkdir $TEMP_DIR
cp simpsh ./$TEMP_DIR/
cd $TEMP_DIR

echo "Can't believe I finished on time." > input.txt
cp input.txt neinput.txt
echo "--command 0 1 2 cat" > verbose.txt
touch empty.txt
touch einput.txt
touch neoutput.txt
touch eoutput.txt
touch err.txt

echo "Wait for files to be ready"
sleep 1

printf "\n*** Test 1 ***\n"
./simpsh --rdonly neinput.txt --wronly neoutput.txt \
--wronly err.txt --command 0 1 2 cat > /dev/null 2>&1 ;
sleep 1
diff -q input.txt neoutput.txt ;
if [ $? -eq 0 ]
then
    echo "!!! Test 1 passed. --command works. !!!"
else
    echo "!!! Test 1 failed !!!"
fi

printf "\n*** Test 2 ***\n"
./simpsh --rdonly einput.txt --wronly eoutput.txt ;
sleep 1
diff -q empty.txt einput.txt ;
if [ $? -eq 0 ]
then
    diff -q empty.txt eoutput.txt ;
    if [ $? -eq 0 ]
    then
        echo "!!! Test 2 passed. Parent process doesn't affect files. !!!"
    else
        echo "!!! Test 2 failed !!!"
    fi
else
    echo "!!! Test 2 failed !!!"
fi

printf "\n*** Test 3 ***\n"
./simpsh --rdonly einput.txt --wronly eoutput.txt --wronly err.txt \
--command 0 1 2 sleep 0.1 --verbose --command 0 1 2 cat > vo.txt 2> /dev/null ;
sleep 1
diff -q vo.txt verbose.txt ;
if [ $? -eq 0 ]
then
echo "!!! Test 3 passed. --verbose works before commands. !!!"
else
echo "!!! Test 3 failed !!!"
fi

rm vo.txt ;
touch vo.txt
printf "\n*** Test 4 ***\n"
./simpsh --rdonly einput.txt --wronly eoutput.txt \
--wronly err.txt --command 0 1 2 cat --verbose > vo.txt ;
sleep 1
diff -q vo.txt empty.txt ;
if [ $? -eq 0 ]
then
    echo "!!! Test 4 passed. --verbose works after commands. !!!"
else
    echo "!!! Test 4 failed !!!"
fi

sleep 1
echo "Clean up ..."
cd ..
rm -rf $TEMP_DIR
