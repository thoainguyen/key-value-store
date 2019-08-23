# Key Value Store

> This project is implemented in c language base on CLRS course (Chapter 13 Btree)

## Getting started

### Server

```sh
$ cd server
$ mkdir sbuild && cd sbuild
```

* build project (from /sbuild)

```sh
$ cmake ..
$ make
```

* run server main function (from /sbuild)

```sh
$ ./server
```

### Client

```sh
$ cd client
$ mkdir cbuild && cd cbuild
```

* build project (from /cbuild)

```sh
$ cmake ..
$ make
```

* run client main function (from /build)

```sh
$ ./client
```


### Command


```sh
$ ./client usertest
127.0.0.1:55000> set thoainh ThoaiNguyenHuynh
OK 
127.0.0.1:55000> get thoainh
ThoaiNguyenHuynh
127.0.0.1:55000> del thoainh
(integer) 1
127.0.0.1:55000> get thoainh
(nul)
```
