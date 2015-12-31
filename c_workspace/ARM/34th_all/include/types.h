#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;


typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned int		size_t;

typedef char *va_list;

#define NULL  0




/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */



#endif /* _LINUX_TYPES_H */
