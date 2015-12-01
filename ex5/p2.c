#include <stdio.h>
#include <pthread.h>

#define QSIZE 10

int put_num = 1;
int get_num = 1;

typedef struct {
  pthread_mutex_t buf_lock;
  int start;
  int num_full;
  pthread_cond_t notfull;
  pthread_cond_t notempty;
  void *data[QSIZE];
} circ_buf_t;

void print_data(circ_buf_t *cbp) {
  int i;

  for (i = 0; i < QSIZE; i++) {
    printf("data[%d] -> %s\n", i, (char *)(cbp->data[i]));
  }
}

// NOTE: エントリを追加する関数
void put_cb_data(circ_buf_t *cbp, void *data) {
  pthread_mutex_lock(&cbp->buf_lock);

  if (put_num == 60)
    printf(" %2d 回目 追加 <- 追加終了 \n", put_num);
  else
    printf(" %2d 回目 追加 \n", put_num);
  put_num++;
  while (QSIZE <= cbp->num_full) {
    pthread_cond_wait(&cbp->notfull, &cbp->buf_lock);
    if ( cbp->num_full == QSIZE ) {
      printf("\nエントリがいっぱいのため取り出し待ち状態\n");
      printf("現在の巡回バッファ\n");
      printf("num_full -> %d\n", cbp->num_full);
      print_data(cbp);
      printf("\n");
    }
  }
  cbp->data[(cbp->start + cbp->num_full) % QSIZE] = data;
  cbp->num_full++;

  pthread_cond_signal(&cbp->notempty);
  pthread_mutex_unlock(&cbp->buf_lock);
}

// NOTE: エントリを取り出す関数
void *get_cb_data(circ_buf_t *cbp) {
  void *data;

  if (get_num == 60)
    printf(" %2d 回目 取り出し <- 取り出し終了 \n", get_num);
  else
    printf(" %2d 回目 取り出し \n", get_num);
  get_num++;
  pthread_mutex_lock(&cbp->buf_lock);
  while (cbp->num_full <= 0) {
    pthread_cond_wait(&cbp->notempty, &cbp->buf_lock);
    if (cbp->num_full == 0) {
      printf("\nエントリが空のため追加待ち状態\n");
      printf("現在の巡回バッファ\n");
      printf("num_full -> %d\n", cbp->num_full);
      print_data(cbp);
      printf("\n");
    }
  }
  data = cbp->data[cbp->start];
  cbp->data[cbp->start] = NULL;
  cbp->start = (cbp->start + 1) % QSIZE;
  cbp->num_full--;

  pthread_cond_signal(&cbp->notfull);
  pthread_mutex_unlock(&cbp->buf_lock);
  return data;
}

// NOTE: 30エントリ追加する関数
void *put_thirty_entries(void *arg) {
  char *entry = "Hello, World!!";
  int i;

  for (i = 0; i < 30; i++) {
    put_cb_data((circ_buf_t *)arg, (void *)entry);
  }
}

// NOTE: 10エントリ取り出す関数
void *get_ten_entries(void *arg) {
  char *entry;
  int i;

  for (i = 0; i < 10; i++) {
    entry = (char *)get_cb_data((circ_buf_t *)arg);
  }
}

int main() {
  circ_buf_t cbp = {
    PTHREAD_MUTEX_INITIALIZER, 0, 0, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL
  };
  // NOTE: 30エントリ追加するスレッド2つ
  pthread_t put_thirty_entries1, put_thirty_entries2;
  // NOTE: 10エントリ取り出すスレッドを6つ
  pthread_t get_ten_entries1, get_ten_entries2, get_ten_entries3,
            get_ten_entries4, get_ten_entries5, get_ten_entries6;

  pthread_create(&get_ten_entries1, NULL, get_ten_entries, (void *)(&cbp));
  pthread_create(&get_ten_entries2, NULL, get_ten_entries, (void *)(&cbp));
  pthread_create(&get_ten_entries3, NULL, get_ten_entries, (void *)(&cbp));
  pthread_create(&put_thirty_entries1, NULL, put_thirty_entries, (void *)(&cbp));
  pthread_create(&get_ten_entries4, NULL, get_ten_entries, (void *)(&cbp));
  pthread_create(&put_thirty_entries2, NULL, put_thirty_entries, (void *)(&cbp));
  pthread_create(&get_ten_entries5, NULL, get_ten_entries, (void *)(&cbp));
  pthread_create(&get_ten_entries6, NULL, get_ten_entries, (void *)(&cbp));

  pthread_join(get_ten_entries1, NULL);
  pthread_join(get_ten_entries2, NULL);
  pthread_join(get_ten_entries3, NULL);
  pthread_join(put_thirty_entries1, NULL);
  pthread_join(get_ten_entries4, NULL);
  pthread_join(put_thirty_entries2, NULL);
  pthread_join(get_ten_entries5, NULL);
  pthread_join(get_ten_entries6, NULL);

  printf("\n処理後の巡回バッファ\n");
  printf("num_full -> %d\n", cbp.num_full);
  print_data(&cbp);
}

