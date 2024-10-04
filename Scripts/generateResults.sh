#!/bin/bash  
# Generate results for OneToOneTests
# Need utility "rename"

sed -i '/write_results=./c\write_results=1' ../pipeline.conf
sed -i '/ptdr=./c\ptdr=0' ../pipeline.conf
sed -i '/load_profiles=./c\load_profiles=0' ../pipeline.conf
sed -i '/routing_algorithm=./c\routing_algorithm=1' ../pipeline.conf
../cmake-build-debug/routing-cli -c ../pipeline.conf -i ../Data/RoutingIndexes/cz/query_sets/05_random_cities.csv -o a
rename s/route_._// *.csv
rename s/.csv/_Dijkstra.csv/ *.csv

sed -i '/routing_algorithm=./c\routing_algorithm=2' ../pipeline.conf
../cmake-build-debug/routing-cli -c ../pipeline.conf -i ../Data/RoutingIndexes/cz/query_sets/05_random_cities.csv -o a
rename s/route_._// *.csv
rename s/.csv/_BidirectionalDijkstra.csv/ *[0-9].csv

sed -i '/routing_algorithm=./c\routing_algorithm=3' ../pipeline.conf
../cmake-build-debug/routing-cli -c ../pipeline.conf -i ../Data/RoutingIndexes/cz/query_sets/05_random_cities.csv -o a
rename s/route_._// *.csv
rename s/.csv/_Astar.csv/ *[0-9].csv

sed -i '/routing_algorithm=./c\routing_algorithm=4' ../pipeline.conf
../cmake-build-debug/routing-cli -c ../pipeline.conf -i ../Data/RoutingIndexes/cz/query_sets/05_random_cities.csv -o a
rename s/route_._// *.csv
rename s/.csv/_BidirectionalAstar.csv/ *[0-9].csv

sed -i '/routing_algorithm=./c\routing_algorithm=6' ../pipeline.conf
sed -i '/alternative_count=../c\alternative_count=2' ../pipeline.conf
../cmake-build-debug/routing-cli -c ../pipeline.conf -i ../Data/RoutingIndexes/cz/query_sets/05_random_cities.csv -o a
rename s/route_0_/00_/ *.csv
rename s/route_1_/01_/ *.csv
rename s/.csv/_Alternatives.csv/ *[0-9].csv



