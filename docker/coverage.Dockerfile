### Build/test container ###
# Define builder stage
FROM kachow:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make
RUN ctest --output-on_failure

RUN gcovr -r ../src/ . ../tests/ --html