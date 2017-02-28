# generate time per operation numbers
./lab2_add --threads=1  --iterations=10000 --sync=m > lab2_add.csv
./lab2_add --threads=2  --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=4  --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=8  --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=1  --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=2  --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=4  --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=8  --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv

# measure the performance
./lab2_list --threads=1  --iterations=1000 --sync=m > lab2_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv
./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv
