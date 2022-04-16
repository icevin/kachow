#! /bin/bash
./build/bin/server tests/example_config &
SERVER_PID=$!
sleep 1
curl http://127.0.0.1 -o ./returned_response -s
diff ./returned_response tests/expected_response
EQUIVALENT=$?
rm ./returned_response
kill $SERVER_PID
exit $EQUIVALENT