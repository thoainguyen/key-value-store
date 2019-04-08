# Hoc socket programming in C

## Co ban

- `htonl` dùng cho network byte order convert tương tự cho `htons`, `ntohs`, `ntohl`.
- một socket cũng được coi là một file trong unix, cũng được gán cho một giá trị file descriptor `int fd`, một socket được mô tả bởi một struct phức tạp:

```c
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
}; 

// có thể cast sang struct sau, note: (IPv4 only--see struct sockaddr_in6 for IPv6)

struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};

// (IPv4 only--see struct in6_addr for IPv6)

// Internet address (a structure for historical reasons)
struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
```

- dùng `inet_pton` để gán chuỗi mô tả địa chỉ ip vào struct, ví dụ

```c
struct sockaddr_in sa; // IPv4
struct sockaddr_in6 sa6; // IPv6

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr)); // IPv6

//returns -1 on error, or 0 if the address is messed up, > 0 for using

// theo cách cũ là dùng hàm `inet_addr`
my_addr.sin_addr.s_addr = inet_addr(SERVER_IPV4_ADDR);
// lưu ý cách này ko hoạt động cho ipv6
```
- dùng `inet_ntop()` để in cấu trúc socket, ví dụ bên dưới

```c
// IPv4:

char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
struct sockaddr_in sa;      // pretend this is loaded with something

inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

printf("The IPv4 address is: %s\n", ip4);


// IPv6:

char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
struct sockaddr_in6 sa6;    // pretend this is loaded with something

inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);

printf("The address is: %s\n", ip6);

// cách cũ là dùng inet_ntoa()
```

- Instead of struct sockaddr_in use struct sockaddr_in6, being sure to add "6" to the fields as appropriate (see structs, above). There is no sin6_zero field.
- Instead of struct in_addr use struct in6_addr, being sure to add "6" to the fields as appropriate (see structs, above).
- Instead of inet_aton() or inet_addr(), use inet_pton().
- Instead of inet_ntoa(), use inet_ntop().
- Instead of gethostbyname(), use the superior getaddrinfo().
- Instead of gethostbyaddr(), use the superior getnameinfo() (although gethostbyaddr() can still work with IPv6).
- INADDR_BROADCAST no longer works. Use IPv6 multicast instead.

- code để show thông tin socket, một host có thể có nhiều tên miền hay địa chỉ ip do đó sẽ trả về một danh sách liên kết chứa các struct thông tin về địa chỉ ip đó.

```c
/*
** showip.c -- show IP addresses for a host given on the command line
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: showip hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n\n", argv[1]);

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res); // free the linked list

    return 0;
}
```

- Để bắt đầu kết nối socket chúng ta dùng hàm `socket` đầu tiên, theo prototype sau: 

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

- Domain thường là `PF_INET`, `PF_INET6`, dùng chúng khi gọi hàm `socket` và dùng `AF_INET` khi gán cho struct chứa thông tin về socket.


- Trong đó , type có thể là "hai" giá trị như sau
  - `SOCK_STREAM`
  - `SOCK_DGRAM` 

- protocol được gán là `0` cho loại socket trên

- có thể khởi tạo một socket descriptor như sau

```c
int s;
struct addrinfo hints, *res;

// do the lookup
// [pretend we already filled out the "hints" struct]
getaddrinfo("www.example.com", "http", &hints, &res);

// [again, you should do error-checking on getaddrinfo(), and walk
// the "res" linked list looking for valid entries instead of just
// assuming the first one is good (like many of these examples do.)
// See the section on client/server for real examples.]

s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

- socket trả về một file descriptor giống như khi chúng ta mở một open file, hoặc trả về -1 nếu có lỗi xảy ra.

- Khi đã có socket descriptor đều chúng ta cần làm là gán giá trị port cho nó, để xác định process nào trên host chạy chương trình làm việc với socket descriptor, như sau:

```c
struct addrinfo hints, *res;
int sockfd;

// first, load up address structs with getaddrinfo():

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

getaddrinfo(NULL, "3490", &hints, &res);

// make a socket:

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// bind it to the port we passed in to getaddrinfo():

bind(sockfd, res->ai_addr, res->ai_addrlen);
```

- hàm `bind` return -1 nếu thất bại và gán mã lỗi cho biến `errno`

```c
// !!! THIS IS THE OLD WAY !!!

int sockfd;
struct sockaddr_in my_addr;

sockfd = socket(PF_INET, SOCK_STREAM, 0);

my_addr.sin_family = AF_INET;
my_addr.sin_port = htons(MYPORT);     // short, network byte order
my_addr.sin_addr.s_addr = inet_addr("10.12.110.57");
memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr);
```

- dùng `setsockopt` để giải quyết trong trường hợp bị trùng port và trả về lỗi 

```c
int yes=1;
//char yes='1'; // Solaris people use this

// lose the pesky "Address already in use" error message
if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
} 
```
- gọi `connect` để phía client thiết lập kết nối đến máy chúng ta và set giá trị port cho local machine

```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen); 
```

- ở phía clinent chúng ta không cần gọi `bind` trước khi gọi `connect` bởi vì chúng ta không cần quan tâm giá trị port number

![](./image/Socket_server.png)

- bên phía server chúng ta gọi `listen` để chờ kết nối từ phía client

```c
int listen(int sockfd, int backlog);
```

- bên phía server hàm `accept` được gọi tiếp theo để đón nhận phía client kết nối đến, và theo prototype

```c
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); 
```

- trong đó giá trị trả về là file descriptor đại diện bên phía client, cũng như thông tin về socket bên phía client được gán vào giá trị struct sockaddr addr.

- sau tất cả các bước trên ta được như sau:

```c
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MYPORT "3490"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

int main(void)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    // !! don't forget your error checking for these calls !!

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, MYPORT, &hints, &res);

    // make a socket, bind it, and listen on it:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    // now accept an incoming connection:

    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    // ready to communicate on socket descriptor new_fd!
    .
    .
    .
```

- tiếp theo dùng hàm `recv` và `send` để gửi gói tin qua lại khi dùng `SOCK_STREAM` ngoài ra dùng `sendto` và `recvfrom` khi dùng `SOCK_DGRAM`

- dùng `send` theo prototype sau 
```c
int send(int sockfd, const void *msg, int len, int flags);
```

- trong đó flag thường được gán là 0, bytesend sẽ trả về số byte mà gửi được và ví dụ:

```c
char *msg = "Beej was here!";
int len, bytes_sent;
.
len = strlen(msg);
bytes_sent = send(sockfd, msg, len, 0);
.
```

- thông thường nếu package nhỏ khoảng dưới 1k thì có thể sẽ send hết trong một lần gọi.

- tương tự chúng ta cũng có hàm `recv` theo proto sau:

```c
int recv(int sockfd, void *buf, int len, int flags);
```

- và đặc biệt `recv` trả về `0` sẽ nói rằng host mà chúng ta kết nối đã bị ngắt

- cuối cùng là đóng socket khi hoàn thành nhiệm vụ

```c
close(sockfd); 
```

- để tránh trường hợp ngắt socket một cách đột ngột khi gọi close chúng ta có một giải pháp khác là dùng hàm shutdown, hàm này tương tự close nhưng sẽ chờ đến một thời điểm thích hợp để close file, nhưu sau

```c
int shutdown(int sockfd, int how); 
//0 : Further receives are disallowed
//1 : Further sends are disallowed
//2 : Further sends and receives are disallowed (like close())
// function return 0 if success, otherwise return -1
```

- dùng `getpeername` để lấy thông tin về phía bên kia

```c
#include <sys/socket.h>

int getpeername(int sockfd, struct sockaddr *addr, int *addrlen); 
```


> Tham khao tu http://beej.us/guide/bgnet/html/single/bgnet.html#inet_ntopman


## Nâng cao

- Các function bị block ở trên là `accept`, `recv`, nếu mà chúng ta không muốn nó bị blocking thì dùng function `fcntl()`

- Ví dụ bên dưới :

```c
#include <unistd.h>
#include <fcntl.h>
.
sockfd = socket(PF_INET, SOCK_STREAM, 0);
fcntl(sockfd, F_SETFL, O_NONBLOCK);
. 
```

- Nếu chúng ta cố gắng đọc dữ liệu từ một socket mà không có sẵn dữ liệu vơi chế độ nonblock thì nó không block mà sẽ trả về `-1` và thiết lập biến `errno` cho `EAGAIN` hoặc là `EWOULDBLOCK`, nếu chúng ta chạy vòng lặp để chờ cho đến khi nào dữ liệu sẵn sàng thì sẽ rất lãng phí gọi là vấn đề busy waiting. Thay vì đó chúng ta quản lý một tập hợp các file descriptor và gọi hàm select để lựa chọn luồng dũ liệu nào ready.

- `select` là cơ chế Synchronous I/O Multiplexing, trong trường hợp bạn là server và bạn muốn lắng nghe tất cả những connection và giữ trạng thái đọc từ connection mà bạn đã kết nối.

- `select` cung cấp cơ chế quản lý nhiều socket và monitor được socket nào sẵn sàng để đọc, sẵn sàng để ghi, hay là sinh ra lỗi.

>The function monitors "sets" of file descriptors; in particular readfds, writefds, and exceptfds. If you want to see if you can read from standard input and some socket descriptor, sockfd, just add the file descriptors 0 and sockfd to the set readfds. The parameter numfds should be set to the values of the highest file descriptor plus one. In this example, it should be set to sockfd+1, since it is assuredly higher than standard input (0).

- đi kèm hàm `select` là tập hợp các mascro định nghĩa như sau

prototype | target
---|---
FD_SET(int fd, fd_set *set); |Add fd to the set.
FD_CLR(int fd, fd_set *set); |Remove fd from the set.
FD_ISSET(int fd, fd_set *set);|Return true if fd is in the set.
FD_ZERO(fd_set *set);| Clear all entries from the set.

- đặc tả timeval để set timeout cho việc chờ, set NULL để luôn luôn chờ cho thằng đầu tiên trả về

- ví dụ dùng select như sau:

```c
/*
** selectserver.c -- a cheezy multiperson chat server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"   // port we're listening on

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
```

- ta có thể định nghĩa hàm `sendall` và sử dụng như sau:

```c
#include <sys/types.h>
#include <sys/socket.h>

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

char buf[10] = "Beej!";
int len;

len = strlen(buf);
if (sendall(s, buf, &len) == -1) {
    perror("sendall");
    printf("We only sent %d bytes because of the error!\n", len);
}
```