### Build/test container ###
# Define builder stage
#FROM kachow:base as builder
FROM base_image as builder

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
COPY --from=builder /usr/src/project/build/bin/server .

# Copy config files
COPY conf/* conf/

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["conf/deploy.conf"]
