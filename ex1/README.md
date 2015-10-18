# システムプログラミング演習1 （メモリ管理）

http://www.ialab.cs.tsukuba.ac.jp/~maeda/class/syspro/ex1.html

## 課題内容: 任意順序の動的メモリ割り付け・解放
下のプログラム（関数alloc2とafree2）は、C言語の標準ライブラリmallocとfreeとほぼ同様に、
任意の順序でメモリの割り付け、解放が行えるものである。ただし、メモリ領域はやはり、予め
静的に確保した配列から少しずつ切り出して割りつける。

#### alloc2.h
```C:alloc2.h
#ifndef ALLOC2_H
#define ALLOC2_H

#define ALLOCSIZE (1024 * 1024)

extern void *alloc2(int n);
extern void afree2(void *p);

#endif
```

#### alloc2.c
```C:alloc2.c
#include "alloc2.h"

typedef double ALIGN;   /* Force alignment */

union header { /* Header for free block */
  struct h {
    union header *ptr;    /* Next free block */
    int size;     /* Size of this free space (including header itself) */
  } s;
  ALIGN x;      /* Force alignment of the block */
};

typedef union header HEADER;

static HEADER allocbuf[ALLOCSIZE / sizeof(HEADER)]  /* Memory area for alloc */
= { { {allocbuf,  (ALLOCSIZE / sizeof(HEADER))} }};

static HEADER *allocp = allocbuf; /* Last block allocated */

void *alloc2(int nbytes)    /* Return pointer to nbytes block */
{
  HEADER *p, *q;
  int nunits = 1 + (nbytes + sizeof(HEADER) - 1) / sizeof(HEADER);

  for (q = allocp, p = q->s.ptr; ; q = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits) { /* Just */
        if (p == q) {  /* We have only one element */
          return 0;
        }
        q->s.ptr = p->s.ptr;
      } else {      /* Allocate tail */
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      allocp = q;
      return (void *)(p + 1);
    }
    if (p == allocp) {
      return 0;
    }
  }
}

void afree2(void *ap)   /* Free space pointed to by p */
{
  HEADER *p, *q;
  p = (HEADER *)ap - 1;
  for (q = allocp; !(p > q && p < q->s.ptr); q = q->s.ptr) {
    if (q >= q->s.ptr && (p > q || p < q->s.ptr)) {
      break;
    }
  }
  if (p + p->s.size == q->s.ptr) { /* Merge to upward */
    p->s.size += q->s.ptr->s.size;
    p->s.ptr = q->s.ptr->s.ptr;
  } else {
    p->s.ptr = q->s.ptr;
  }
  if (q + q->s.size == p) { /* Merge to downward */

    /* FILL HERE */

  } else {
    q->s.ptr = p;
  }
  allocp = q;
}
```

### 取り組む内容
1. 上の `/* FILL HERE */` の部分を適切なプログラムに置き換えて関数 `alloc2` を完成させなさい。
2. `alloc2` と `afree2` の動作をテストするプログラムをファイル `main2.c` として作り、
  `alloc2.c` のコンパイル結果とリンクしてテストを行いなさい。
  > `alloc2` と `main2.c` を提出しなさい

##### テスト項目
```
・上限を超えない範囲で割り付け・解放を多数回繰り返しても失敗しない
・LIFO順でない割り付け・解放を多数回繰り返しても失敗しない
・上限を越えた割り付けを行うと、失敗する
・割り付けを受けて、まだ解放されていない領域は、互いに重なっていない
（上限が例題とは異なることに注意。上限のバイト数はあまり厳密に調べないでよいので、
「十分におさまる範囲なら成功する」「大幅に越える範囲だと失敗する」ことを調べる。）
```

なお、テストプログラムを作る上では、上記の機能使用の他、下記のような点に注意すること
```
・人間が目でたくさんの出力を読んで、成功、失敗を判断しなければいけないようなテストは
ダメ。ひとめで成功か失敗かわかること。
・ほんの数バイトとか数回ではテストにならないかもしれない。可能なら、領域全体をテストしよう
・main.cのテストは、オリジナリティを発揮すること。他の人と変数名だけ違うとか、
コンパイルすると同じ機械語になるようなプログラムはカンニングとみなす。
・インタフェースを意識しよう。テストするプログラムは、明記された仕様だけを利用すること。
内部でどうやっているかを利用してはいけない。連続してallocすると、連続した領域が返って
来るなどと仮定してはいけない
・ライブラリ中にテストやデバッグ用のprintfを残したまま提出してはいけない
・明示的にそのように指定されない限り、複数のファイルをtarとかzipとかでまとめてアップロードしてはいけない
```

