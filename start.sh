#! /bin/bash
pkill cmake_study

cd /home/cheakf/Documents/cpp_study/build/

cmake ..
cmake --build . -j8
while true
do
    ps -ef | grep cmake_study | grep -v grep
    if [ "$?" -eq 1 ]
    then
        ./cmake_study
        echo "process has been restarted!"
        ./restart_video
    else
        echo "process already started!"
    fi
    sleep 1
done
