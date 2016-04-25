#!/bin/bash

for((i=1;i<=10;i++))
do
    echo process_num=$i;
    for((j=0; j < $i; j++))
    do
	./a.out&
    done
    wait
done


