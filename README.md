# KVS - (key value store fresher implementation)

> This project is implemented in c language base on CLRS course (Chapter 13 Btree)

## Getting started

### Server

```sh
$ cd server
$ cd build
```

* Create data file, and config file (from /build)

```sh
$ touch meta.dat data.dat
```

* build project (from /build)

```sh
$ cmake ..
$ make
```

* run server main function

```sh
$ ./build/kvs-ser
```

### Client

```sh
$ cd client
$ make
```

* run client main function

```sh
$ ./client 127.0.0.1
```

### Command

* `set`, `get`, `del`

```sh
$ ./client 127.0.0.1
127.0.0.1:55000> set thoainh ThoaiNguyenHuynh
OK
127.0.0.1:55000> get thoainh
ThoaiNguyenHuynh
127.0.0.1:55000> del thoainh
(integer) 1
127.0.0.1:55000> get thoainh
(nul)
```