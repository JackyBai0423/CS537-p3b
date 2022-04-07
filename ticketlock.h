typedef struct lock_t {
  uint ticket;        // current ticket number being served
  uint turn;          // next ticket number to be given
  // struct proc *proc;  
} lock_t;