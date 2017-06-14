#!/bin/bash
rm broccolilog.txt
../build/Revision1 | tee broccolilog.txt| nc -lp 5805
