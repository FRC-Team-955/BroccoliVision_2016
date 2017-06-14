#!/bin/bash
cd ./build
#cmake -DCMAKE_C_COMPILER=/bin/clang -DCMAKE_CXX_COMPILER=/bin/clang++ -DCMAKE_BUILD_TYPE=Debug .. 
cmake -DCMAKE_BUILD_TYPE=Debug .. 
