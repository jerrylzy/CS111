# plot average wait time for mutex in list
./lab2_list --threads=1  --iterations=1000 --sync=m > lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m >> lab_2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m >> lab_2b_list.csv

# Confirm the correctness of new implementation
# how many iterations it takes to reliably fail
# --yield=id, 4 lists, 1,4,8,12,16 threads,
# and 1, 2, 4, 8, 16 iterations (and no synchronization)
./lab2_list --threads=1  --iterations=1   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=2   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=4   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=8   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=16  --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=2   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=4   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=8   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=16  --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=2   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=4   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=8   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=16  --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=2   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=4   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=8   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=16  --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=1   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=2   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=4   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=8   --yield=id --lists=4 >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=16  --yield=id --lists=4 >> lab_2b_list.csv

# updates are now properly protected
# --yield=id, 4 lists, 1,4,8,12,16 threads,
# and 10, 20, 40, 80 iterations, --sync=s and --sync=m
# m
./lab2_list --threads=1  --iterations=10  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=20  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=40  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=80  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=10  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=20  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=40  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=80  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=10  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=20  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=40  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=80  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=10  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=20  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=40  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=80  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=10  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=20  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=40  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=80  --yield=id --lists=4 --sync=m >> lab_2b_list.csv
# s
./lab2_list --threads=1  --iterations=10  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=20  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=40  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=80  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=10  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=20  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=40  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=80  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=10  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=20  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=40  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=80  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=10  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=20  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=40  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=80  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=10  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=20  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=40  --yield=id --lists=4 --sync=s >> lab_2b_list.csv
./lab2_list --threads=16 --iterations=80  --yield=id --lists=4 --sync=s >> lab_2b_list.csv

# measure the performance
# without yields for 1000 iterations, 1,2,4,8,12 threads, and 1,4,8,16 lists
# 1 list
#./lab2_list --threads=1  --iterations=1000 --sync=m >> lab_2b_list.csv
#./lab2_list --threads=2  --iterations=1000 --sync=m >> lab_2b_list.csv
#./lab2_list --threads=4  --iterations=1000 --sync=m >> lab_2b_list.csv
#./lab2_list --threads=8  --iterations=1000 --sync=m >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m >> lab_2b_list.csv
# I'm wondering if Dr. Reiher ever realized this test is done before. This lab is a JOKE.
./lab2_list --threads=1  --iterations=1000 --sync=s >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s >> lab_2b_list.csv
# 4 lists
./lab2_list --threads=1  --iterations=1000 --sync=m --lists=4 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=1000 --sync=s --lists=4 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s --lists=4 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s --lists=4 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s --lists=4 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab_2b_list.csv
# 8 lists
./lab2_list --threads=1  --iterations=1000 --sync=m --lists=8 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m --lists=8 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m --lists=8 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m --lists=8 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=1000 --sync=s --lists=8 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s --lists=8 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s --lists=8 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s --lists=8 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab_2b_list.csv
# 16 lists
./lab2_list --threads=1  --iterations=1000 --sync=m --lists=16 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m --lists=16 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m --lists=16 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m --lists=16 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab_2b_list.csv
./lab2_list --threads=1  --iterations=1000 --sync=s --lists=16 >> lab_2b_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s --lists=16 >> lab_2b_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s --lists=16 >> lab_2b_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s --lists=16 >> lab_2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab_2b_list.csv
