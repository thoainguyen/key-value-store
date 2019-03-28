# KVS - (key value store fresher implementation)

> This project is implemented in c language base on CLRS course (Chapter 13 Btree)

## Getting started

### Server

```sh
$ cd server
$ cd build
```

* build project (from /build)

```sh
$ cmake ..
$ make
```

* run server main function (from /build)

```sh
$ ./server
```

* run with other port

```sh
$ ./server -p 9087
```

### Client

```sh
$ cd client
$ cd build
```

* build project (from /build)

```sh
$ cmake ..
$ make
```

* run server main function (from /build)

```sh
$ ./client
```

* run with other port

```sh
$ ./client -p 9087
```

* run with other port & host

```sh
$ ./client -p 9087 -h 10.23.0.4
```

### Command

* `set`, `get`, `del`

```sh
$ ./client
127.0.0.1:55000> set thoainh ThoaiNguyenHuynh
OK
127.0.0.1:55000> get thoainh
ThoaiNguyenHuynh
127.0.0.1:55000> del thoainh
(integer) 1
127.0.0.1:55000> get thoainh
(nul)
```