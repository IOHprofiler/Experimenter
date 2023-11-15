#!/usr/bin/env bash

#PBS -q beta
#PBS -l select=1:ncpus=24
#PBS -l walltime=00:01:00
#PBS -N francois_constraints
#PBS -j oe

module add cmake/3.22
module add conda3-2023.02
module add gcc/11.2
module add git/2.42.0
module add LLVM/clang-llvm-10.0

cd /scratchbeta/rusind/
conda activate ./IOHexperimenter/.conda_environment
ipython3 ./IOHexperimenter/tests/python/never.py
