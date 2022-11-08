# Kachow
UCLA CS130 Course Project, Spring 2022
Web Server w/ HTTPS & OAuth support

## Code Layout
### cmake/
- CodeCoverageReportConfig.cmake
    - cmake file for generating code coverage report

### conf/
- deploy.conf
    - Configuration the deployed version of the server is using

### docker/
- base.Dockerfile
    - Dockerfile to build the base image
- cloudbuild.yaml
    - yaml file for specifying configuration to GCP
- coverage.Dockerfile
    - Dockerfile for creating coverage container
- Dockerfile
    - Dockerfile for creating container for server

### include/
- config_parser.hh
    - Header file for config_parser
- mime.hh
    - Header file containing `extensionToMIME` called by session
- request_handler_factory.hh
    - Header file declaring all prototypes for request handler factories
- request_handler.hh
    - Header file declaring get_response function for each request handler type
    - Constructors for all request handlers are defined in this file
- server.hh
    - Header file for server declaring all functions, including those used for
    testing
- session.hh
    - Header file for session declaring all functions, including those used for
    testing

### src/
- config_parser.cc
    - Provided config parser
- request_handler_factory.cc
    - Defines `create`  for each type of request handler factory
- request_handler.cc
    - Defines `get_response` for each type of request handler
- server_main.cc
    - Parses config file, generates routes to match for each request handler,
    starts the server
- server.cc
    - Provided server file
    - Modified to add `generate_routes` function, which creates a map of
    locations to request handler factories
- session.cc
    - Provided session file, with modifications to `handle_read` to make server
    act as desired
    - `url_prefix_matches`  to check if the URL prefix is desired
    - `match` to find longest matching location in routes
## Building
Run the following code to build locally (assuming `build/` is present
in the current directory):
```
cd build
cmake ..
make
```
## Testing
Run the following code to test locally (assuming in `build/` and
everything has been built):
```
make test
```
## Running
Run the following code to run locally (assuming in `build/` and
everything has been built):
```
./bin/server ../conf/deploy.conf
```
## Request Handlers
### Adding a New Request Handler
To add a request handler,
1. Edit `request_handler_factory.hh` to add the declarations of the
constructor and `create` for the new factory
2. Edit `request_handler.hh` to add the declarations of the constructor and
`get_response` for the new handler
3. Edit `request_handler_factory.cc` to define the constructor and `create`
for the new factory
4. Edit `request_handler.hh` to define the constructor and `get_response` for
the new handler
5. Edit `server.cc` to add the new route to the map (line 56 begins
`generate_routes`, which is where modification should take place)
6. Add the new handler in the config file

### Example Request Handler
EchoHandler:
- Declared in `request_handler.hh`
- Constructor defined in `request_handler.hh`
- Functions defined in `request_handler.cc`
- Factory declared and defined in `request_handler_factory.hh`, inheriting from
RequestHandlerFactory base class
- Factory's `create` declared in `request_handler_factory.hh`, defined in
`request_handler_factory.cc`
- When a server is created, `generate_routes` runs and maps any EchoHandler to
a new EchoHandlerFactory
- Whenever a session is created and `handle_read` is called, create a request
handler for the location
    - For an EchoHandler, whatever parameters are given to it are unused, but
    this allows us to use one calling method to create every handler type
- Use the handler to `get_response`
- Delete the handler



### Header Files
All header files are located in [include/](#include)
