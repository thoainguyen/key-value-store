# Key Value Store

Using btree disk base to implement key value store, build client server nonblocking I/O, threadpool, message queue.

Implement key value store sử dụng btree làm storage engine lưu trữ dữ liệu xuống đĩa, xây dựng server nonblocking I/O, dùng thread pool, message queue.

## Xây dựng kiến thức

> Ghi chú: [linux system](./docs/kvstore-impl.md), [networking](./docs/network.md)


## Hướng dẫn cài đặt

B0. Theo hướng dẫn [kvstore/README.md](kvstore/README.md) để chạy thành công

* Cấu trúc project:

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

## Dùng kvstore đối với mã nguồn bên thứ ba:

* B1: Đầu tiên, chạy chương trình kvstore phía server như ở trên, để cung cấp kvstore service.
  * Có hai option `-c` và `-l`:
    * Dùng `-c` khi muốn tạo mới dữ liệu
    * Dùng `-l` khi muốn chạy kvstore trên dữ liệu cũ

```sh
kvstore/server/sbuild $ ./server
using `./server -c` for create new or `./server -l` for load%
```

* B2: copy hai file **kvstore.h**, và **kvstore.c** vào project bên thứ ba.

* B3: Các API hỗ trợ:

```c
/* giá trị db dùng để truyền vào các interface method sau này, HOST, PORT là hostname và port của dbserver, nếu db < 0, thì có lỗi xảy ra*/
int db = connect_kvstore(HOST, PORT);
/* trong đó key (char*) là khóa, value là giá trị trả về, trả về NULL nếu lỗi xảy ra.*/
char *value = get_kvstore(db, key);
/*hàm này xóa cặp key-value trong kvstore, trả về "OK" nếu thành công và "not exist!" nếu key không tồn tại*/
char *result = del_kvstore(db, key);
/*hàm này set giá trị key-value trong kvstore, nếu key đã tồn tại thì giá trị được ghi đè, hàm trả về "OK"*/
char *result = set_kvstore(db,key,value);
/*hàm này dùng để đóng kết nối kvstore, xóa dữ liệu */
void close_kvstore(int);
```
