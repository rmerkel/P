#!/bin/bash
for i in $( ls test/*.p ); do
	s=$(basename $i)
	./p -l $i &> $s.lst
	cmp $s.lst $i.lst
	if [ "$?" != "0" ]; then
		diff $s.lst $i.lst
		exit
	fi
done

