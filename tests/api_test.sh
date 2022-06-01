#! /bin/bash
./build/bin/server tests/api_config &
SERVER_PID=$!
FILE=./api/shoes/1
sleep 1

curl -k -v --silent -X POST -H "application/json" -d "{'a': 1}" https://localhost/api/Shoes 2>&1 | grep "< " | sed 's/< //g' > ./postresponse
if ! cmp -s ./postresponse tests/expected_api_201; then
    rm ./postresponse
    rm -rf ./api
    echo -e "API test 1 (POST) fail - create unsuccessful"
    kill $SERVER_PID
    exit 1
fi
rm ./postresponse

curl -k -v --silent -X GET -H "application/json"  https://localhost/api/Shoes/1 2>&1 | grep "< " | sed 's/< //g' > getresponse
if ! cmp -s ./getresponse tests/expected_api_200; then
    rm ./getresponse
    rm -rf ./api
    echo -e "API test 2 (GET) fail - reterive unsuccessful"
    kill $SERVER_PID
    exit 1
fi
rm ./getresponse

curl -k -v --silent -X GET -H "application/json"  https://localhost/api/Shoes/1 > getresponse
if ! cmp -s ./getresponse tests/expected_get; then
    rm ./getresponse
    rm -rf ./api
    echo -e "API test 2 (GET) fail - content not same"
    kill $SERVER_PID
    exit 1
fi
rm ./getresponse

curl -k -v --silent -X PUT -H "application/json" -d "{'a': 2}" https://localhost/api/Shoes/1 2>&1 | grep "< " | sed 's/< //g' > ./putresponse
if ! cmp -s ./putresponse tests/expected_api_200_2; then
    rm ./putresponse
    rm -rf ./api
    echo -e "API test 3 (PUT) fail - update unsuccessful"
    kill $SERVER_PID
    exit 1
fi
rm ./putresponse

curl -k -v --silent -X GET -H "application/json"  https://localhost/api/Shoes/1 2>&1 | grep "< " | sed 's/< //g' > getresponse
if ! cmp -s ./getresponse tests/expected_api_200; then
    rm ./getresponse
    rm -rf ./api
    echo -e "API test 4 (GET) fail - reterive unsuccessful"
    kill $SERVER_PID
    exit 1
fi
rm ./getresponse

curl -k -v --silent -X GET -H "application/json"  https://localhost/api/Shoes/1 > getresponse
if ! cmp -s ./getresponse tests/expected_put; then
    rm ./getresponse
    rm -rf ./api
    echo -e "API test 4 (GET) fail - content not same"
    kill $SERVER_PID
    exit 1
fi
rm ./getresponse

curl -k -v --silent -X DELETE -H "application/json"  https://localhost/api/Shoes/1 2>&1 | grep "< " | sed 's/< //g' > deleteresponse
if ! cmp -s ./deleteresponse tests/expected_api_200_2; then
    rm ./deleteresponse
    rm -rf ./api
    echo -e "API test 5 (DELETE) fail - did not return 200 response"
    kill $SERVER_PID
    exit 1
fi
rm ./deleteresponse

if [ -f $FILE ]; then
    rm -rf ./api
    echo -e "API test 5 (DELETE) fail - file did not get deleted"
    kill $SERVER_PID
    exit 1
fi

rm -rf ./api
kill $SERVER_PID
exit 0