/////trj//////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;

typedef uint8_t             byte_t;

typedef int					bool_t;
typedef int64_t				mtime_t;

typedef void* (thread_func)(void*);

extern int begin_thread(thread_func func,void* arg);
