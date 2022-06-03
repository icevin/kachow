#!/bin/bash
apt-get update && apt-get install -y wget
url=https://www.googleapis.com/oauth2/v1/certs

mkdir keys > /dev/null 2>&1 || true

while true
do
    wget $url -q -x -O ./keys/certs.json
    INNER=0
    while [ $INNER -lt 1 ]; do
        sleep 60s
        INNER=$((INNER + 1))
    done
done
