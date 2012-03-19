#!/bin/sh

cd Debug

for f in ../Tests/? ../Tests/??
do
    filename="${f##*/}"
    cp $f.mem target.mem
    touch target.mem
    ./AnimalGame.exe < $f > $filename.test
    echo Test $filename
    diff $f.out $filename.test
    echo Test $filename.mem
    mv AnimalGame.memory $filename.test.mem
    diff $filename.test.mem target.mem
done

