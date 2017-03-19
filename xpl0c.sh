#!/bin/bash
for i in $( ls *.p ); do
	./pl0c $i &> $i.lst
	cmp $i.lst $i.expected
	if [ "$?" != "0" ]; then
		diff $i.lst $i.expected
		exit
	fi
done

