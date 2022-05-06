# boost asio https client

## specification
we need to implement below

1. HTTPS(secure) send request (GET, POST and so on)
2. HTTPS(secure) receive response (multi-data type whatever must be loaded on the live data)
3. Cache what have received, for increasing cache hit rate makes app faster
4. Multi-threading, should be send a request or receive a reponse simultanuesly

- HTTPS Request instance

## Question for myself
- Does it need to handle one socket for multiple request/response?
- How can be implemented if it needed?
  - for session deque?