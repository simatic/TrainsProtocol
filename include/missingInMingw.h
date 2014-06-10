#ifndef _MISSINGINMINGW_H_
#define _MISSINGINMINGW_H_
#ifdef WINDOWS

#include <io.h>
struct iovec
{
  void*   iov_base;
  size_t  iov_len;
};

ssize_t writev(int fd, const struct iovec* iov, int count);

#endif

#endif /* _MISSINGINMINGW_H_ */

