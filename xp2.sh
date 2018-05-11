#!/bin/bash
for i in $( ls test2/*.p ); do
	s=$(basename $i)
	./p -l $i < $i.in &> $s.lst
	cmp $s.lst $i.lst
	if [ "$?" != "0" ]; then
		diff $s.lst $i.lst
		exit
	fi
done

