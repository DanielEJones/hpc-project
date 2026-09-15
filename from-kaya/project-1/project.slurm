#!/bin/bash

#SBATCH --cpus-per-task=96
#SBATCH --mem=6G
#SBATCH --partition=cits3402
#SBATCH --time=00:20:00

#SBATCH --job-name=mega_96
#SBATCH --output=job_%j.out
#SBATCH --error=job_%j.err

make parallel
./main /group/cits/cits053/project-1/2_mega_a.pdf /group/cits/cits053/project-1/2_mega_b.pdf
