#include <string.h>
#include <stddef.h>
#include <sys/types.h>

typedef int word;

#define wsize sizeof(word)
#define wmask (wsize-1)

void *memcpy(void *dst_, const void *src_, size_t length) {
    char *dst = dst_;
    const char *src = src_;
    size_t t;

    if (length == 0 || dst == src) {
        goto done;
    }

    /*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
    #define	TLOOP(s) if (t) TLOOP1(s)
    #define	TLOOP1(s) do { s; } while (--t)
	

    if ((size_t)dst < (size_t)src) {
        /*
         * Copy forward.
         */
        t = (size_t)src;	/* only need low bits */
        if ((t | (size_t)dst) & wmask) {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (size_t)dst) & wmask || length < wsize)
                t = length;
            else
                t = wsize - (t & wmask);
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    } else {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (size_t)src;
        if ((t | (size_t)dst) & wmask) {
            if ((t ^ (size_t)dst) & wmask || length <= wsize)
                t = length;
            else
                t &= wmask;
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }

done:
    return dst_;
}