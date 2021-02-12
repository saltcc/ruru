## ruru is a tiny udp/dtls/sctp/data stack transport server

#### Prerequisites
* DTLS - [OpenSSL](https://www.openssl.org/)
* SCTP - [usrsctp](https://github.com/sctplab/usrsctp)

#### Building
```bash
mkdir build && cd build
cmake ..
make
```

#### platforms
* Linux(epoll)

#### Demo
* Server:EchoServer.cpp
* Client:EchoClient.cpp


#### License
BSD 2-Clause