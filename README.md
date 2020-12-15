# Webserv

Project coding a non-blocking HTTP server in C++ according to the RFC specifications 7230-7235. The primary function of a web server is to store, process and deliver web pages to clients. The server will listen on one or more ports for HTTP Request that at a minimum look like this:

```
GET / HTTP/1.1
Host: localhost
```
## Dependencies
Needs cmake to compile. Run `cmake . ; make`. 
To compile with googletests: `cmake -DTEST=ON . ; make`. 
To compile with bonus: `cmake -DBONUS=ON . ; make`

## Implemented methods:
- [x] GET
- [x] HEAD
- [x] POST
- [x] PUT
- [x] DELETE

## Implemented HTTP Headers:
- [x] Accept-Charsets
- [x] Accept-Language
- [x] Allow
- [x] Authorization
- [x] Content-Language
- [x] Content-Length
- [x] Content-Location
- [x] Content-Type
- [x] Date
- [x] Host
- [x] Last-Modified
- [x] Location
- [x] Referer
- [x] Retry-After
- [x] Server
- [x] Transfer-Encoding
- [x] User-Agent
- [x] WWW-Authenticate

## BONUSES:

- [x] workers as processes
- [x] plugins
  - [x] gzip
  - [x] deflate
  - [x] add in config
  - [x] load through terminal
- [x] Make routes work with regexp
- [x] Configure a proxy to an other http/https server

