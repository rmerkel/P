#!/bin/bash
for i in $( ls test2/*.p ); do
	s=$(basename $i)
	./p -l $i < $i.in &> objs/$s.lst
	cmp objs/$s.lst $i.lst
	if [ "$?" != "0" ]; then
		diff objs/$s.lst $i.lst
		exit
	fi
done

