#!/bin/sh

cd Debug

for f in ../Tests/? ../Tests/??
do
    filename="${f##*/}"
    ./AnimalGame.exe < $f > $filename.test
    echo Test $filename
    diff $f.out $filename.test
done

