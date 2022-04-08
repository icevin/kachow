### Build/test container ###
# Define builder stage
FROM kachow:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

# Copy server output binary to "."
# TODO(nidhipanchal): update path based on names used
COPY --from=builder /usr/src/project/build/bin/server .

# Copy config files
# TODO(nidhipanchal): check if this works or if it needs to be in the same format as prev command
COPY conf/* conf/

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(nidhipanchal): Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(nidhipanchal): Update with real conf name
CMD ["conf/deploy.conf"]
