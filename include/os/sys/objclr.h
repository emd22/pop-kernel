#ifndef OBJCLR_H
#define OBJCLR_H

#define objclr(obj, sz) memset(obj, 0, sz)
#define objclr_r(arr, sz, obj_sz) int i_; for (i_ = 0; i_ < sz; i_++) objclr(&arr[i], obj_sz)

// void objclr(void *obj, int sz);

#endif