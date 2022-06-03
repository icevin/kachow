#!/bin/bash
wget https://www.googleapis.com/oauth2/v1/certs -x -O ./keys/certs.json &
./update.sh &
./server $1