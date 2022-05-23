#! /bin/bash
./build/bin/server tests/multithread_config &
SERVER_PID=$!

# server startup delay
sleep 1

# call delay handler
curl -s http://127.0.0.1/sleep >> ./multi.out &
DELAY_PID=$!

# call echo handler from separate process before delayed handler finishes blocking
sleep 0.1
curl -s http://127.0.0.1/echo >> ./multi.out


sleep 2

# /echo response should be present before /sleep response
diff ./multi.out tests/multithread_expected
EQUIVALENT=$?

# cleanup
rm ./multi.out
kill $SERVER_PID
kill $DELAY_PID
exit $EQUIVALENT