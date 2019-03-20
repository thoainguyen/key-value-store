Simple make
================
*A simple example about using make*

> NOTE: This is not a tutorial about using make. 

### Overview 
The structure of our directory is

```
├── app
│   ├── include
│   └── src
│       ├── main.cpp
│       └── makefile
├── build
├── libs
│   └── meta
│       ├── include
│       │   ├── data.h
│       └── src
│           ├── data.cpp
│           └── makefile
├── makefile
└── README.md
```

There's a `make` file in each `src` directory

### The app

The app uses library `meta`:

```cpp
#include "data.h"

int main(int argc, char const *argv[])
{
    Data* ptrData = new Data(3);
    ptrData->display();
    return 0;
}
```

Reading through all `makefile` to understand the internal

### Commands

**Compile**

To compile, issue the following command 

```sh
$ make all
```

Binaries are stored in directory `/build`:

```sh
$ build/app
Data 3
```

**Clean**

To clean all intermediate files, run the command below:

```sh
$ make clean
```

### References

- [A Simple Makefile Tutorial](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)

- [GNU make](https://www.gnu.org/software/make/manual/make.html)