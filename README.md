# Key Value Store

Using btree disk base to implement key value store, build client server nonblocking I/O, threadpool, message queue.

## Build knowledge

> Note [linux system](./note/note.md), [networking](./note/network.md)


## Implement Key value store

B1. Follow [kvstore/README.md](kvstore/README.md) for run success

* Project structure:

```sh
.
├── client
│   ├── client.c
│   └── CMakeLists.txt
├── include
│   ├── bnode.h
│   ├── btree.h
│   ├── kvstore.h
│   ├── kvpair.h
│   ├── server.h
│   ├── thpool.h
│   └── utils.h
├── server
│   ├── CMakeLists.txt
│   └── server.c
└── src
    ├── bnode.c
    ├── btree.c
    ├── kvstore.c
    └── thpool.c
```

* [btree.h](./kvstore/include/btree.h): định nghĩa cấu trúc btree.
* [btree.c](./kvstore/src/btree.c) : các hàm hiện thực btree.
* [bnode.h](./kvstore/src/bnode.c) : định nghĩa cấu trúc bnode.
* [utils.h](./kvstore/include/utils.h) : các hàm đọc ghi bnode xuống file, và đọc ghi meta data để build tree.
* [kvpair.h](./kvstore/include/kvpair.h) : định nghĩa struct key value.
* [server.c](./kvstore/server/server.c) : hiện thực server, socket nonblocking, giao tiếp với btree.
* [client.c](./kvstore/client/client.c) : hiện thực client, cung cấp kvstore-cli và autotest.
* [thpool.c](./kvstore/src/thpool.c) : hiện thực threadpool, message queue.
* [kvstore.h](./kvstore/include/kvstore.h) : cung cấp interface giao tiếp với kvstore.
* [kvstore.c](./kvstore/src/kvstore.c) : hiện thực kết nối server, process các method insert, search, delete.

## Dùng kvstore đối với bên thứ ba:

* B2: chạy chương trình kvstore phía server

```sh
kvstore/server/sbuild $ ./server
```

* B3: import hai file **kvstore.h**, và **kvstore.c** vào project.

* B4: kết nối tới database server:

```c
/* giá trị db dùng để truyền vào các interface method sau này, HOST, PORT là hostname và port của dbserver, nếu db < 0, thì có lỗi xảy ra*/
int db = connect_kvstore(HOST, PORT);
/* trong đó key (char*) là khóa, value là giá trị trả về, trả về NULL nếu lỗi xảy ra.*/
char *value = get_kvstore(db, key);
/*hàm này xóa cặp key-value trong kvstore, trả về "OK" nếu thành công và "not exist!" nếu key không tồn tại*/
char *result = del_kvstore(db, key);
/*hàm này set giá trị key-value trong kvstore, nếu key đã tồn tại thì giá trị được ghi đè, hàm trả về "OK"*/
char *result = set_kvstore(db,key,value);
```