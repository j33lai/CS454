#!/bin/bash

make clean
make

cd ../PartialTestCode
make clean; make
cd ../PartialTestCode1
make clean; make
cd ../PartialTestCode2
make clean; make
cd ../PartialTestCode3
make clean; make
