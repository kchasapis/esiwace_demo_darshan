#!/usr/bin/env bash

RANK=0
CLIENT=0

      while getopts "hc:r:" option
        do
            case $option in
                h)
                    echo "$0 -c <number_of_client_nodes> -r <number_of_mpi_rank_per_client> "
                    exit 0
                    ;;
                r)
                    RANK=$OPTARG;
                    ;;
                c)
                    CLIENT=$OPTARG;
                    ;;
                \?)
                    echo "$0 -c <number_of_client_nodes> -r <number_of_mpi_rank_per_client> "
                    exit 1
                    ;;
            esac
        done

if [[ $RANK -eq 0 ]]  || [[ $CLIENT -eq 0 ]]
  then
  echo "$0 -c <number_of_client_nodes> -r <number_of_mpi_rank_per_client> "
  exit 1
fi

TOTAL_RANK=`expr $RANK \* $CLIENT`

SRC=ssf.c
BIN=`pwd`/ssf_exe

mpicc $FLAGS $SRC -o $BIN
OUTPUT="/tmp/local_output"

LD_PRELOAD=$DARSHAN_LIB mpirun --oversubscribed -np $TOTAL_RANK $BIN -k 65536 -v -p $OUTPUT
echo removing output file
rm -f ${OUTPUT}/local_output
