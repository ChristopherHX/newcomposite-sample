#pragma once
#include_next <fcntl.h>
#define F_LINUX_SPECIFIC_BASE   1024
/*
 * Set/Get seals
 */
#define F_ADD_SEALS     (F_LINUX_SPECIFIC_BASE + 9)
#define F_GET_SEALS     (F_LINUX_SPECIFIC_BASE + 10)

/*
 * Types of seals
 */
#define F_SEAL_SEAL     0x0001  /* prevent further seals from being set */
#define F_SEAL_SHRINK   0x0002  /* prevent file from shrinking */
#define F_SEAL_GROW     0x0004  /* prevent file from growing */
#define F_SEAL_WRITE    0x0008  /* prevent writes */
#define F_SEAL_FUTURE_WRITE     0x0010  /* prevent future writes while mapped */
/* (1U << 31) is reserved for signed error codes */
