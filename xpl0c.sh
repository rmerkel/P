#!/bin/bash
for i in $( ls *.p ); do
	./pl0c $i &> $i.lst
	cmp $i.lst test/$i.lst
	if [ "$?" != "0" ]; then
		diff $i.lst test/$i.lst
		exit
	fi
done

