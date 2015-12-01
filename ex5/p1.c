#include <stdio.h>
#include <pthread.h>
#include <time.h>

int account_balance = 1000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// NOTE: 現在残高を出力
void *check_balance() {
  printf("-> あなたの現在の残高 ￥%d \n", account_balance);
}

// NOTE: 引数の分預金する
void *deposit(void *arg) {
  int *money = (int *)arg;

  pthread_mutex_lock(&lock);

  account_balance += *money;
  printf("預金 ￥%d \n", *money);
  check_balance();

  pthread_mutex_unlock(&lock);
}

// NOTE: 引数の分払戻する.もし預金残高が足りない場合は全額払戻する
void *withdraw(void *arg) {
  int *money = (int *)arg;
  int returning_money;

  pthread_mutex_lock(&lock);

  if (*money <= account_balance) {
    account_balance -= *money;
  } else {
    returning_money = account_balance;
    account_balance = 0;
  }
  printf("払戻 ￥%d \n", returning_money);
  check_balance();

  pthread_mutex_unlock(&lock);
}

int main() {
  pthread_t tid1, tid2, tid3, tid4, tid5, tid6;
  int arg1, arg2, arg3, arg4, arg5, arg6;

  srand((unsigned)time(NULL));

  arg1 = (rand() % 10) * 1000;
  arg2 = (rand() % 10) * 1000;
  arg3 = (rand() % 10) * 1000;
  arg4 = (rand() % 10) * 1000;
  arg5 = (rand() % 10) * 1000;
  arg6 = (rand() % 10) * 1000;

  pthread_create(&tid1, NULL, deposit, (void *)(&arg1));
  pthread_create(&tid2, NULL, withdraw, (void *)(&arg2));
  pthread_create(&tid3, NULL, deposit, (void *)(&arg3));
  pthread_create(&tid4, NULL, withdraw, (void *)(&arg4));
  pthread_create(&tid5, NULL, deposit, (void *)(&arg5));
  pthread_create(&tid6, NULL, withdraw, (void *)(&arg6));

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
  pthread_join(tid4, NULL);
  pthread_join(tid5, NULL);
  pthread_join(tid6, NULL);
}
