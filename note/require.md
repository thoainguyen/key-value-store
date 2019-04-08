# Implement Key-value store

## 1. Mô tả

* Key Value store là một dạng storage rất hay được sử dụng trong hệ thống hiện nay.
* Rất nhiều db engine tổ chức lưu trữ data dưới dạng key-value như RocksDB, LevelDB, Redis,...
* Mục đích: Có cái nhìn tổng quan về hệ thống và cách hoạt động cơ bản của một Database Engine.

## 2. Bài toán

### 1.1 Key Value Store

#### a. Kiến thức về cấu trúc dữ liệu

* Yêu cầu:
  * Nắm rõ kiến thức về cấu trúc dữ liệu lưu trữ và lý do nó được sử dụng trong các Database Engine, phân biệt Data Structure và Database
  * Các DB Engine sử dụng nhiều Data Structure khác nhau, tùy vào từng khả năng của từng loại Database, nhiệm vụ của các bạn là tìm hiểu cơ bản các Data Structure hay được sử dụng và mục đích sử dụng của chúng.
* Gợi ý:
  * Một vài cấu trúc dữ liệu hay gặp như Slip List, Red-Black Tree, B-Tree, B+ Tree, LSM Tree,... trong đó cá Data Structure như LSM Tree, Skip List, B Tree được sử dụng trong các DB Engine như Postgres, Rocks DB hay một số DBMS khác.
  * Tham khảo [Data Structure](http://staff.ustc.edu.cn/~csli/graduate/algorithms/book6/toc.htm)
* Cách thực hiện:
  * Đọc và ghi chú những đặc điểm hay ho của từng Data Structure
  * Chọn một cấu trúc dữ liệu minh họa (lưu trữ dữ liệu dạng Key-value để phục vụ cho mục đích lưu trữ)
  * Cài đặt một cấu trúc dữ liệu nhằm mục đích lưu trữ dữ liệu dưới dạng key-value, cần nắm rõ về loại data structure sử dụng

#### b. Kiến thức về Linux System

* Yêu cầu:
  * Ôn lại kiến thức cơ bản của Linux System như File System, I/O Blocking, Non-Blocking, Networking, Network Byte Order, Memory Management, Page Table, User/Kernel Space, Semaphore, Mutex,...
  * Tìm hiểu các kiến thức nâng cao về:
  * IPC: Signal, Shared Memory, Pipes,...
  * Tìm hiểu nhóm các API trong POSIX, POSIX Message Queue,...
  * Nắm rõ các System call để làm việc với hệ thống.
* Tham khảo:
  * Blocking vs Non-Blocking I/O
  * IPC
  * Sách về Linux System Programming
  * POSIX
* Kết quả cần đạt được:
  * Nắm vững kiến thức về Linux System.
  * Ghi chú đầy đủ, tốt nhất nên có code demo chạy được.

#### c. Kiến thức về kỹ thuật lập trình

* Ôn lại kiến thực về Pointer, Function Pointer, OOP (nếu dùng C++ làm ngôn ngữ thể hiện)
* Xử lý chuỗi

#### d. Công cụ hỗ trợ

* Do làm việc với bộ nhớ nên những debuger của IDE sẽ không giúp ích được nhiều do đó nên tìm hiểu và sử dụng GDB (GNU debuger), sau này có thể sử dụng nó cho Golang.
* Valgrind công cụ để check Memory Leak
* Cmake để hỗ trợ tạo makefile


### 1.2 Key Value Store service

* Yêu cầu:
  * Sau khi hòan thành `1.1` hãy xây dựng cho mình một Key-Value Store Service tương tự như RocksDB.
  * Sử dụng cấu trúc dữ liệu đã implement ở `a` và kiến thức ở `b,c,d` để thực hiện.

* Kết quả cần đạt được:
  * Server đáp ứng cùng một lúc nhiều client tham gia.
  * Dữ liệu được ghi xuống đĩa an toàn và hiệu quả
  * Client cần phải được xây dựng rõ ràng, xây dựng các test case.

## 3. Ngoài lề

* Khuyến khích sử dụng C tuần hoặc C++, tránh viết lẫn lộn giữa C và C++.
* Xây dựng Project Structure rõ ràng, để người đọc code dễ hình dung hơn.
* Nên tìm hiểu và sử dụng những kĩ thuật để tối ưu hóa cho service
