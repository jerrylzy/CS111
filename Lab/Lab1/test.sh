#!/bin/bash

chmod 755 simpsh

TEMP_DIR="lab1_test"
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

printf "\n*** Test 5 ***\n"
printf "5\n4\n3\n1\n2\n" > sort_input.txt
./simpsh --rdonly sort_input.txt --creat --wronly test_output.txt \
--wronly err.txt --command 0 1 2 sort --wait > wait_output.txt 2> /dev/null ;
sleep 2
sort < sort_input.txt > sort_output.txt
diff -q test_output.txt sort_output.txt
if [ $? -eq 0 ]
then
    if grep -q "sort" wait_output.txt
    then
        echo "!!! Test 5 passed. --creat, --wait work. !!!"
    else
        echo "!!! Test 5 failed !!!"
    fi
else
    echo "!!! Test 5 failed !!!"
fi

printf "\n*** Test 6 ***\n"
echo "Eggert" > neoutput.txt
sleep 1
./simpsh --rdonly einput.txt --wronly err.txt --pipe --trunc --wronly neoutput.txt \
--command 0 3 1 cat --command 2 4 1 cat > /dev/null 2>&1 ;
sleep 2
diff -q empty.txt neoutput.txt
if [ $? -eq 0 ]
then
    echo "!!! Test 6 passed. --trunc, --pipe work. !!!"
else
    echo "!!! Test 6 failed !!!"
fi

printf "\n*** Test 7 ***\n"
./simpsh --rdonly neinput.txt --trunc --wronly eoutput.txt --wronly err.txt \
--abort --command 0 1 2 cat > /dev/null 2>&1 ;
sleep 1
if [ -s "eoutput.txt" ]
then
    echo "!!! Test 7 failed !!!"
else
    echo "!!! Test 7 passed. --abort work. !!!"
fi

sleep 1
echo "Clean up ..."
cd ..
rm -rf $TEMP_DIR
