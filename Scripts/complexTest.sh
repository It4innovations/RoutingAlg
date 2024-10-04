#!/usr/bin/env bash
SETS_PATH="/scratch/work/user/sev231/StaticRoutingData/index_16_04_19/query_sets/"
FILE_TYPE=_random_ids.csv

for BUILD_TYPE in "Debug" "Release"; do
	for COMPILER in "intel" "gcc"; do
		for PARAM in "-r" "-d" "-a 3"; do
			for IDC in 10 100 1000; do
				sh /scratch/work/user/sev231/StaticRoutingCPP/startTests.sh -b $BUILD_TYPE -c $COMPILER $PARAM -i "$SETS_PATH$IDC$FILE_TYPE"
			done
		done
	done
done
