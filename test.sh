#!/bin/sh

cd Debug

for f in ../Tests/? ../Tests/??
do
    filename="${f##*/}"
    cp $f.mem target.mem
    touch target.mem
    touch AnimalGame.memory
    rm AnimalGame.memory
    ./AnimalGame.exe < $f > $filename.test
    echo Test $filename
    diff $f.out $filename.test
    echo Test $filename.mem
    mv AnimalGame.memory $filename.test.mem
    diff $filename.test.mem target.mem
done

for f in ../Tests/*.meminput
do
    filename="${f##*/}"
    cp $f AnimalGame.memory
    ./AnimalGame.exe < $f.cin > $filename.test
    echo Test $filename
    diff $f.out $filename.test
    echo Test $filename.mem
    mv AnimalGame.memory $filename.test.mem
    diff $filename.test.mem $f.memory.out
done
