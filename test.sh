#!/bin/sh

cd Debug

for f in ../Tests/?
do
    filename="${f##*/}"
    ./AnimalGame.exe < $f > $filename.test
    diff $f.out $filename.test
done
