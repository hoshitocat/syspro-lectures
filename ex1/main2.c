#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "alloc2.h"

#define TRUE 0
#define FALSE -1
#define NUMBER_OF_TEST_COUNT 1000 /* テスト回数 */

void testcase1(void *mtable[]);
void testcase2(void *mtable[]);
void testcase3(void *mtable[]);
void testcase4(void *mtable[]);

void shuffle_arr(int arr[]);
void swap_arr(int arr[], int x, int y);
void allfree(void *mtable[]);

int main() {
  void *mtable[1000];

  testcase1(mtable);
  // testcase2(mtable);
  testcase3(mtable);
  testcase4(mtable);

  return 0;
}

/* 上限をこえない範囲での割り付け・解放を多数回繰り返しても失敗しない */
void testcase1(void *mtable[]) {
  int max = 1000;
  int current_test_count, i, demand_mem_size;

  printf("上限をこえない範囲での割り付けを多数回繰り返しても失敗しない\n");

  srand((unsigned)time(NULL));

  for ( current_test_count = 0; current_test_count < NUMBER_OF_TEST_COUNT; current_test_count++ ) {
    max = 1000, i = 0, demand_mem_size = 0;
    for ( demand_mem_size = rand() % 10 + 1;  1 <= (max - demand_mem_size); i++ ) {
      mtable[i] = alloc2(demand_mem_size * 1000);
      if ( mtable[i] == 0 ) {
        printf("Test Case 1 failed !! ;(\n\n");
        return;
      }

      max -= demand_mem_size;
      demand_mem_size = rand() % 10 + 1;
    }

    for ( i -= 1; 0 <= i; i-- ) { afree2(mtable[i]); }
  }
  printf("# Test Case 1 success !! :)\n\n");
}

/* LIFO順でない割り付け・解放を多数回繰り返しても失敗しない */
void testcase2(void *mtable[]) {
  int max = 1000;
  int current_test_count, i, j;
  int uniq_num_i[1000];

  for ( i = 0; i < 1000; i++ ) { uniq_num_i[i] = i; }

  printf("LIFO順でない割り付け・解放を多数回繰り返しても失敗しない\n");

  srand( (unsigned) time(NULL) );

  for ( current_test_count = 0; current_test_count < NUMBER_OF_TEST_COUNT; current_test_count++ ) {
    shuffle_arr(uniq_num_i);
    max = 1000;
    for ( i = 0; i < 1000; i++ ) { mtable[i] = alloc2(1000); }
    /* ランダムの箇所を解放する */
    for ( i = 0; i < 500; i++ ) { afree2( mtable[ uniq_num_i[i] ] ); }
    /* ランダムの箇所を割り付けする */
    for ( i = 0; i < 500; i++ ) { mtable[ uniq_num_i[i] ] = alloc2(1000); }
    allfree( mtable );
  }
  printf("# Test Case 2 success !! :)\n\n");
}

/* 上限を越えた割り付けを行うと、失敗する */
void testcase3(void *mtable[]) {
  int max = 1000;
  int current_test_count, i, demand_mem_size;

  printf("上限を越えた割り付けを行うと、失敗する\n");

  srand((unsigned)time(NULL));

  for ( current_test_count = 0; current_test_count < NUMBER_OF_TEST_COUNT; current_test_count++ ) {
    max = 1000, i = 0, demand_mem_size = 0;
    for ( demand_mem_size = rand() % 10 + 1;  1 <= (max - demand_mem_size); i++ ) {
      mtable[i] = alloc2(demand_mem_size * 1000);
      if ( mtable[i] == 0 ) {
        printf("Test Case 3 failed !! ;(\n\n");
        return;
      }

      max -= demand_mem_size;
      demand_mem_size = rand() % 10 + 1;
    }

    /* 上限を越えた越えた割り付け */
    mtable[i + 1] = alloc2(100000);
    if ( mtable[i + 1] != 0 ) {
      printf("Test Case 3 failed !! ;(\n\n");
      return;
    }

    for ( i -= 1; 0 <= i; i-- ) { afree2(mtable[i]); }
  }
  printf("# Test Case 3 success !! :)\n\n");
}

/* 割り付けを受けて，まだ解放されていない領域は，互いに重なっていない */
void testcase4(void *mtable[]) {
  int max = 1000;
  int current_test_count, demand_mem_size;
  int i, j, k;

  printf("割り付けを受けて，まだ解放されていない領域は，互いに重なっていない\n");

  srand((unsigned)time(NULL));

  for ( current_test_count = 0; current_test_count < NUMBER_OF_TEST_COUNT; current_test_count++ ) {
    max = 1000, i = 0, demand_mem_size = 0;
    for ( demand_mem_size = rand() % 10 + 1;  1 <= (max - demand_mem_size); i++ ) {
      mtable[i] = alloc2(demand_mem_size * 1000);
      if ( mtable[i] == 0 ) {
        printf("Test Case 4 failed !! ;(\n\n");
        return;
      }

      max -= demand_mem_size;
      demand_mem_size = rand() % 10 + 1;
    }

    for ( j = 0; j <= i; j++ ) {
      for ( k = 0; k < j; k++ ) {
        if ( j != k) {
          int distance = mtable[j] - table[k];
          if ( distance < 0 ) {
            distance = -distance;
          }
          if ( distance < 1000 ) {
            printf("Test Case 4 failed !! ;(\n\n");
            return;
          }
        }
      }
    }

    for ( i -= 1; 0 <= i; i-- ) { afree2(mtable[i]); }
  }
  printf("# Test Case 4 success !! :)\n\n");
}

void shuffle_arr(int arr[]) {
  int i;
  srand( (unsigned) time(NULL) );
  for ( i = 0; i < 1000; i++ ) {
    swap_arr(arr, rand() % 1000, rand() % 1000);
  }
}

void swap_arr(int arr[], int x, int y) {
  int tmp = arr[x];
  arr[x] = arr[y];
  arr[y] = tmp;
}

void allfree(void *mtable[]) {
  int i;
  for ( i = 0; i < 1000; i++ ) {
    if ( mtable[i] != NULL )
      afree2( mtable[i] );
  }
}
