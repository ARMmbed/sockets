# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

## [v1.1.4] - 2016-03-16
### Added
* Configuration file to support test coverage generation

## [v1.1.3] - 2016-03-02
### Improved
- Updated sockets tests to work with new mbed-greentea infrastructure

## [v1.1.2] - 2016-03-01
### Updated
- Updated mbed-drivers dependency to 1.0.0

## [v1.1.1]
### Improved
- Documentation now defines expected error codes
- Error codes returned match documentation
- Additional checks for failing conditions

### Added
- A new TCPStream constructor for use with ```accept()```
- The previous TCPStream constructor used with ```accept()``` is now deprecated

## [v1.1.0]
### Improved
- SocketAddr now formats both IPv4 and IPv6 addresses
- SocketAddr now interprets text addresses into internal representation with the setAddr API
- Added an API to enable/disable Nagle's algorithm on TCP Streams

[Unreleased] https://github.com/ARMmbed/sockets/compare/v1.1.4...HEAD
[v1.1.4] https://github.com/ARMmbed/sockets/compare/v1.1.3...v1.1.4
[v1.1.3] https://github.com/ARMmbed/sockets/compare/v1.1.2...v1.1.3
[v1.1.2] https://github.com/ARMmbed/sockets/compare/v1.1.1...v1.1.2
[v1.1.1] https://github.com/ARMmbed/sockets/compare/v1.1.0...v1.1.1
[v1.1.0] https://github.com/ARMmbed/sockets/compare/v1.0.2...v1.1.0
