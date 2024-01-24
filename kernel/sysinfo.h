#ifndef _SYSINFO_H
#define _SYSINFO_H

struct sysinfo {
  uint64 freemem;   // amount of free memory (bytes)
  uint64 nproc;     // number of process
};

#endif