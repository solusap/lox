#!/bin/bash

origin_file="/home/shanruibin/lox/cxxLox/test/test_unwind.txt" 
new_file="/home/shanruibin/lox/cxxLox/test/test_unwind_new.txt" 

while read line
do
    left_p=$(echo $line | awk -F'(' '{print $1}')
    right_p=$(echo $line | awk -F'+' '{print $2}')
    function_name_d=$(echo $line | awk -F'(' '{print $2}' | awk -F'+' '{print $1}')
    echo "start to demangle $function_name_d"
    funtion_name_t=$(c++filt $function_name_d)

    echo "$leftp$funtion_name_t$right_p" >> "$new_file"
done < "$origin_file"