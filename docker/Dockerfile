### Build/test container ###
# Define builder stage
FROM kachow:base as builder
#FROM base_image as builder

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

# Copy static directory
COPY static/* static/

# Copy login directory
COPY login/* login/

# Copy ssl directory
COPY ssl/* ssl/

# Copy keys directory
COPY keys/* keys/

# Expose some port(s)
EXPOSE 80

RUN mkdir keys > /dev/null 2>&1 || true

# Update certificates
RUN wget https://www.googleapis.com/oauth2/v1/certs -x -O ./keys/certs.json

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./run.sh"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["conf/deploy.conf"]
