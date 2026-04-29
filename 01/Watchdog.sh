#!/bin/bash

TIMEOUT=$1

shift

$@ &
PID=$!

for (( i=0; i<$TIMEOUT; i++ )); do
    sleep 1
    if kill -0 $PID 2>/dev/null; then
        echo "Still running..."
    else
        break 
    fi
done

if kill -0 $PID 2>/dev/null; then
    kill -TERM $PID
    sleep 1
    if kill -0 $PID 2>/dev/null; then
        kill -KILL $PID
    fi
fi