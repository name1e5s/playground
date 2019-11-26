#!bin/bash

echo 'Average filename length is:'
find . -type d | xargs -l -I {} sh -c "find {} -type f -printf '%f\n'" | awk '{print length}' | awk '{ total+=$1; count+=1} END {print total/count}'
echo 'Average file count is:'
find . -type d | xargs -l -I {} sh -c "(ls -l {} | wc -l); (ls -l {} | grep "^d" | wc -l)" | awk '{if(count % 2 == 0){ total+=$1; count+=1} else {total-=$1; count += 1}} END {print total*2/count}'
echo 'Average directory depth is:'
find . -type d -printf '%d\n' | awk '{ total+=$1; count+=1} END {print total/count}'