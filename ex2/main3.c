#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "alloc3.h"

/* LIFO順でない割り付け・解放を多数回繰り返しても失敗しない． */
void testcase1(void *mem_table[]);

/* 合計で数MB程度の領域を割り付けてもエラーにならない． */
void testcase2(void *mem_table[]);

/* 割り付けしたメモリ領域全体に値を書き込んでもエラーにならない． */
void testcase3(void *mem_table[]);

/* 割り付けを受けて，まだ解放されていない領域は，互いに重なっていない． */
void testcase4(void *mem_table[]);

int main() {
  void *mem_table[1000];

  testcase1(mem_table);
  testcase2(mem_table);
  testcase3();
  testcase4(mem_table);

  return 0;
}

/* LIFO順でない割り付け・解放を多数回繰り返しても失敗しない． */
void testcase1(void *mem_table[]) {
  int num_of_trials = 1000;
  int current_p;
  int i = 0;

  printf(" Test Case 1: ");
  // NOTE: 乱数のseedの初期化
  srand((unsigned)time(NULL));

  // 最初適当数割り付けを行う
  for ( i = 0; i < 1000; i++ ) {
    mem_table[i] = alloc3( 1000 );
    if ( mem_table[i] == NULL ) { printf("failed memory allocation !!\n"); }
  }

  for ( i = 0; i < num_of_trials; i++) {
    current_p = rand() % 1000;
    if ( mem_table[current_p] == NULL ) {
      mem_table[current_p] = alloc3( 1000 );
      if ( mem_table[current_p] == NULL ) { printf("failed memory allocation !!\n"); }
    } else {
      afree3( mem_table[current_p] );
      mem_table[current_p] = NULL;
    }
  }

  // 最後mem_tableを元に戻す
  for ( i = 0; i < 1000; i++ ) {
    if ( mem_table[i] != NULL ) {
      afree3( mem_table[i] );
      mem_table[i] = NULL;
    }
  }

  printf("successed :) \n");

  return;
}

/* 合計で数MB程度の領域を割り付けてもエラーにならない． */
void testcase2(void *mem_table[]) {
  int size_of_alloc = 0; // for文毎の割り付けサイズ
  int sum_of_alloc = 0; // 割り付け合計(size_of_allocの合計サイズ)
  int maximum_alloc = pow(1024, 2); // 割り付けテストを行う最大値
  int i;

  printf(" Test Case 2: ");
  // NOTE: 乱数のseedの初期化
  srand((unsigned)time(NULL));

  // NOTE: 10MB以上割り付けを行う
  for ( i = 0; sum_of_alloc < maximum_alloc; sum_of_alloc += size_of_alloc, i++ ) {
    size_of_alloc = (rand() % 10 + 1) * 1024;

    mem_table[i] = alloc3(size_of_alloc);
    if ( mem_table[i] == NULL ) {
      printf("failed memory allocation !!\n");
      return;
    }
  }

  printf("successed :) \n");

  return;
}

/* 割り付けしたメモリ領域全体に値を書き込んでもエラーにならない． */
void testcase3(void *mem_table[]) {
}

/* 割り付けを受けて，まだ解放されていない領域は，互いに重なっていない． */
void testcase4(void *mem_table[]) {
}
