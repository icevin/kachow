#! /bin/bash
./build/bin/server tests/request_handlers_config &
SERVER_PID=$!
sleep 1
curl -k https://127.0.0.1/echo -o ./returned_response -s
diff ./returned_response tests/expected_response
EQUIVALENT=$?
rm ./returned_response
kill $SERVER_PID
exit $EQUIVALENT