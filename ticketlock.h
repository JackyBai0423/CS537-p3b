typedef struct ticketlock {
  uint ticket;        // current ticket number being served
  uint turn;          // next ticket number to be given
  struct proc *proc;  
} ticketlock;