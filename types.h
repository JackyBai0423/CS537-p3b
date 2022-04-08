typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
typedef struct {
  int ticket;        // current ticket number being served
  int turn;          // next ticket number to be given
  // struct proc *proc;  
} lock_t;