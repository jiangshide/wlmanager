#!/bin/bash


name="android10";

if [ -n "$1"];then
    $name=$1;
fi

sudo docker container start $name
sudo docker exec -it $name bash
