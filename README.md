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

## Links
- [Build a simple HTTP server from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
- [Guide to network programming](https://web.archive.org/web/20130922070455/http://beej.us/guide/bgnet/output/html/multipage/index.html)
- [RFC 7230](https://www.rfc-editor.org/rfc/rfc7230.html)
- [RFC 7231](https://www.rfc-editor.org/rfc/rfc7231.html)
- [Intro to TCP/IP and Sockets](https://www.youtube.com/playlist?list=PLbtjxiXev6lqCUaPWVMXaKxrJtHRRxcpM)
- [Matching location blocks](https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms)
- [HTTP Headers](https://code.tutsplus.com/tutorials/http-headers-for-dummies--net-8039)

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

