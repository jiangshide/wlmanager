#!/bin/bash

echo "create ubuntu 20.04 os"
docker pull ubuntu:20.04

echo "load new name with input,eg:jsd"
sudo docker run -it --name=$1 ubuntu:20.04 bash
