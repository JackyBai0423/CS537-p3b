#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "x86.h"
#include "user.h"


#define PGSIZE 4096
char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
// thread library
void* unaligned_stack[2048];
int numOfThreads = 0;
int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2){
  // if(pgcount>=150){
  //   thread_join();
  // }
  // allocate stack for thread
  void *stack = malloc(PGSIZE*2);
  unaligned_stack[numOfThreads] = stack;
  numOfThreads++;
  // align stack to page boundary
  if((uint)stack%PGSIZE){
    stack = stack + (PGSIZE - ((uint)stack%PGSIZE));
  }
  return clone(start_routine, arg1, arg2, stack);
}

// thread join
int thread_join(){
  // calls join() to free user stack and return waited-for PID
  void *stack;
  int pid = join(&stack);
  if(pid != -1){
    for(int i = 0; i < 2048; i++){
      if(stack>=unaligned_stack[i] && stack<=unaligned_stack[i]+PGSIZE){
        free(unaligned_stack[i]);
        unaligned_stack[i] = 0;
        break;
      }
    }
    numOfThreads--;
  }
  return pid;
}

void
lock_init(lock_t *lock){
  lock->ticket = 0;
  lock->turn = 0;
}

void
lock_acquire(lock_t *lock){
  int myturn = fetch_and_add(&lock->ticket, 1);
  while(fetch_and_add(&lock->turn, 0)!= myturn);
}

void
lock_release(lock_t *lock){
  fetch_and_add(&lock->turn,1);
}