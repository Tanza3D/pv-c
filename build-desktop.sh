#!/bin/bash
 
rm bin/pv-c

cmake -DDESKTOP=1 .
make
./bin/pv-c