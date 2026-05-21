#ifndef _MISC_H
#define _MISC_H

#include <stddef.h>
#if defined(__cplusplus)
extern "C" {
#endif
    extern size_t _cur_time;
    size_t get_epoch();
    char* which(const char* cmd);
    void disable_erc();
    void enable_erc();
#if defined(__cplusplus)
}
#endif

#endif
