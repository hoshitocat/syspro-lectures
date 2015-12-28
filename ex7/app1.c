#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

char *num_str[] = { "one", "two", "three", "four", "five", "six", "seven",
  "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen",
  "fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty",
  "twenty one", "twenty two", "twenty three", "twenty four", "twenty five",
  "twenty six", "twenty seven", "twenty eight", "twenty nine", "thirty" };

int dequeue_count = 0;
int enqueue_count = 0;

struct entry {
  struct entry *next;
  void *data;
};

struct list {
  struct entry *head;
  struct entry **tail;
  pthread_cond_t notempty;
  int size;
};

struct list *list_init(void) {
  struct list *list;

  list = malloc(sizeof *list);
  if (list == NULL)
    return (NULL);
  list->head = NULL;
  list->tail = &list->head;
  list->size = 0;
  return (list);
}

int list_enqueue(struct list *list, void *data) {
  struct entry *e;

  e = malloc(sizeof *e);
  if (e == NULL)
    return (1);
  e->next = NULL;
  e->data = data;
  pthread_mutex_lock(&lock);
  *list->tail = e;
  list->tail = &e->next;
  list->size++;
  pthread_cond_signal(&list->notempty);
  pthread_mutex_unlock(&lock);
  return (0);
}

struct entry *list_dequeue(struct list *list) {
  struct entry *e;

  pthread_mutex_lock(&lock);
  while (list->head == NULL)
    pthread_cond_wait(&list->notempty, &lock);
  if (list->head == NULL) {
    pthread_mutex_unlock(&lock);
    return(NULL);
  }
  e = list->head;
  list->head = e->next;
  list->size--;
  if (list->head == NULL)
    list->tail = &list->head;
  pthread_mutex_unlock(&lock);
  return (e);
}

struct entry *list_traverse
(struct list *list, int (*func)(void *, void *), void *user) {
  struct entry **prev, *n, *next;

  if (list == NULL)
    return (NULL);

  prev = &list->head;
  for (n = list->head; n != NULL; n = next) {
    next = n->next;
    switch (func(n->data, user)) {
      case 0:
        /* continues */
        prev = &n->next;
        break;
      case 1:
        /* delete the entry */
        *prev = next;
        if (next == NULL)
          list->tail = prev;
        return (n);
      case -1:
      default:
        /* traversal stops */
        return (NULL);
    }
  }
  return (NULL);
}

int print_entry(void *e, void *u) {
  printf("%s\n", (char *)e);
  return (0);
}

int delete_entry(void *e, void *u) {
  char *c1 = e, *c2 = u;

  return (!strcmp(c1, c2));
}

// NOTE: 30エントリ追加するための関数
void *put_thirty_entries(void *arg) {
  int i = 0;
  struct list *list = (struct list *)arg;
  for (i = 0; i < 30; i++) {
    enqueue_count++;
    list_enqueue(list, strdup(num_str[i]));
    printf("Enqueue %d回目 : %s\n", enqueue_count, num_str[i]);
  }
}

// NOTE: 10エントリ取り出す関数
void *get_ten_entries(void *arg) {
  struct list *list = (struct list *)arg;
  struct entry *entry;
  int i = 0;

  for (i = 0; i < 10; i++) {
    entry = list_dequeue(list);
    dequeue_count++;
    printf("Dequeue %d回目 : %s\n", dequeue_count, (char *)entry->data);
    free(entry->data);
    free(entry);
  }
}

int main() {
  struct list *list;
  struct entry *entry;

  // NOTE: 30エントリ追加するためのスレッド2つ
  pthread_t put_thirty_entries1, put_thirty_entries2;
  // NOTE: 10エントリ取り出すためのスレッド6つ
  pthread_t get_ten_entries1, get_ten_entries2, get_ten_entries3,
            get_ten_entries4, get_ten_entries5, get_ten_entries6;

  list = list_init();

  pthread_create(&put_thirty_entries1, NULL, put_thirty_entries, (void *)(list));
  pthread_create(&get_ten_entries1, NULL, get_ten_entries, (void *)(list));
  pthread_create(&get_ten_entries2, NULL, get_ten_entries, (void *)(list));
  pthread_create(&get_ten_entries3, NULL, get_ten_entries, (void *)(list));
  pthread_create(&get_ten_entries4, NULL, get_ten_entries, (void *)(list));
  pthread_create(&get_ten_entries5, NULL, get_ten_entries, (void *)(list));
  pthread_create(&put_thirty_entries2, NULL, put_thirty_entries, (void *)(list));
  pthread_create(&get_ten_entries6, NULL, get_ten_entries, (void *)(list));

  pthread_join(put_thirty_entries1, NULL);
  pthread_join(get_ten_entries1, NULL);
  pthread_join(get_ten_entries2, NULL);
  pthread_join(get_ten_entries3, NULL);
  pthread_join(get_ten_entries4, NULL);
  pthread_join(get_ten_entries5, NULL);
  pthread_join(put_thirty_entries2, NULL);
  pthread_join(get_ten_entries6, NULL);

  /* entry list */
  list_traverse(list, print_entry, NULL);

  return (0);
}

