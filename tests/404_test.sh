#! /bin/bash
./build/bin/server tests/request_handlers_config &
SERVER_PID=$!
sleep 1
curl -k https://127.0.0.1/unknown -o ./returned_404 -s
diff ./returned_404 tests/expected_404
EQUIVALENT=$?
rm ./returned_404
kill $SERVER_PID
exit $EQUIVALENT