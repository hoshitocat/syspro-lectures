#include <sys/mman.h>
#include <stdio.h>

#include "alloc3.h"

// #define NULL 0 /* これないとコンパイル時にエラーになる */

typedef double ALIGN;  /* Force alignment */

union header {         /* Header for free block */
  struct h {
    union header *ptr; /* Next free block */
    int size;          /* Size of this free space (including header itself) */
    int realbytes;
  } s;
  ALIGN x;             /* Force alignment of the block */
};

typedef union header HEADER;

static HEADER allocbuf = { { &allocbuf, 1, (int)sizeof(HEADER) } };
static HEADER *allocp = &allocbuf; /* Last block allocated */

static void *morecore(int nbytes, int *realbytes)
{
  void *cp;
  *realbytes = (nbytes + ALLOC_UNIT - 1) / ALLOC_UNIT * ALLOC_UNIT;
  cp = mmap(NULL, *realbytes, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, 0, 0);
  if (cp == (void *)-1) { return NULL; }
  return cp;
}

void *alloc3(int nbytes)           /* Return pointer to nbytes block */
{
  HEADER *prev_p = allocp;
  HEADER *current_p = prev_p->s.ptr;
  HEADER *p, *q;
  int realbytes;
  void *free_p;

  /*
  ** 引数nbytesがHEADERいくつ分に相当するサイズであるか計算
  ** 自分自身のHEADER分も含めるので、+1 している
  ** したがって、実際に割り付けに必要なサイズはnunitsとなる
  */
  int nunits = 1 + (nbytes + sizeof(HEADER) - 1) / sizeof(HEADER);

  /* 現在確保されているメモリ領域に割り付けられる領域があるか探索する */
  do {
    /*
    ** 空き領域がひとつしかない場合
    ** かつ、その領域のサイズが要求領域サイズと同じまたは少ない場合
    ** 必ずOSにメモリを要求する必要がある
    */
    if ( current_p == prev_p && current_p->s.size <= nunits )
      break;

    if ( nunits <= current_p->s.size ) {
      if ( nunits == current_p->s.size) {
        prev_p->s.ptr = current_p->s.ptr;
      } else {
        current_p->s.size -= nunits;
        current_p += current_p->s.size;
        current_p->s.size = nunits;
      }
      allocp = prev_p;
      return (void *)(current_p + 1);
    }

    prev_p = current_p;
    current_p = current_p->s.ptr;
  } while ( prev_p != allocp );

  free_p = morecore( nunits * sizeof(HEADER), &realbytes );
  p = (HEADER *)free_p;
  p->s.realbytes = realbytes;
  p->s.size = nunits;
  afree3( (void *)((HEADER *)free_p + 1) );

  for (q = allocp, p = q->s.ptr; ; q = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits) {
        q->s.ptr = p->s.ptr;
      } else {			/* Allocate tail */
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      allocp = q;
      return (void *)(p + 1);
    }
  }

  return NULL;
}

void afree3(void *ap)		/* Free space pointed to by p */
{
  HEADER *p, *q;
  p = (HEADER *)ap - 1;
  for (q = allocp; !(p > q && p < q->s.ptr); q = q->s.ptr) {
    if (q >= q->s.ptr && (p > q || p < q->s.ptr)) {
      break;
    }
  }

  /* Merge to upward */
  if (p + p->s.size == q->s.ptr) {
    p->s.size += q->s.ptr->s.size;
    p->s.ptr = q->s.ptr->s.ptr;
  } else {
    p->s.ptr = q->s.ptr;
  }

  /* Merge to downward */
  if (q + q->s.size == p) {
    q->s.size += p->s.size;
  } else {
    q->s.ptr = p;
  }

  if ( q <= p ) {
    allocp = q;
  } else {
    allocp = p;
  }
}

