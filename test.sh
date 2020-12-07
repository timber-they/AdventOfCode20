#!/bin/bash

for i in ./*.c
do
	gcc "$i";
	[$? -eq 0] || exit 1;
done

exit 0
