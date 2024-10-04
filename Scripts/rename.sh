#!/usr/bin/env bash

for d in $(find . -type d -not -name "\.*" -maxdepth 1)
do

# Move files
#    DIR=$(basename $d)
#    echo $DIR
#    mkdir $DIR/profiles
#    for p in $(find $DIR -not -name "$DIR.csv")
#    do
#        echo "mv $p $DIR/profiles"
#        mv "$p" "$DIR/profiles"
#
#    done

# Replace .00 in files
    cd $d
    FILE="$(basename $d).csv"
    NEW_FILE="fix_$FILE"
    echo $FILE
    echo "id;length;freeflow_speed" > $NEW_FILE

    sed -e 's#\.00##g' $FILE > $NEW_FILE

# Rename path files
#cd $d/profiles
#for p in $(find . -type f -name "*.csv")
#do
#    PID=$(basename -s .csv $p)
#    NEWPID="${PID}_profile.csv"
##    echo $PID.csv $NEWPID
#    mv "$PID.csv" "$NEWPID"
#done

#DAY_FILE="/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Data/SpeedProfiles/probability_uk/54/profiles/0_Probability4_profile.csv"
## Append day string to the profiles
#cd $d/profiles
#
#for p in $(find . -type f -name "*.csv")
#do
#    p=$(basename $p)
#    echo $p
#    mv "$p" "old_$p"
#    while read days <&3 && read profile <&4
#    do
#        IFS='|' read -r -a array <<< "$days"
#        echo "${array[0]}|${array[1]}|${array[2]}|$profile" >> $p
#
#    done 3<$DAY_FILE 4<old_$p
#    rm "old_$p"
#done

cd -
done