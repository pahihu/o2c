#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define NOPROTOTYPES
#include "_OGCC.h"

#if defined(sun) && !defined(__svr4__)
extern int on_exit(void (*procp)(void),void* arg);
#else
extern int atexit(void (*func)(void));
#endif



#define MAX_TERM_PROCS 32	/* maximum number of termination procs */

static ModuleId moduleId;
INTEGER Rts_exitCode;
LONGINT Rts_argc;
CHAR **Rts_argv;
void (*Rts_termProcs[MAX_TERM_PROCS])(void);
volatile int /*sig_atomic_t*/ Rts_noSignalHandlerInProgress = 1;



INTEGER Rts_ArgNumber (void) {
  return (INTEGER) (Rts_argc-1);
}

void Rts_GetArg (INTEGER num, CHAR *arg) {
  INTEGER i = -1;

  do {
    i++;
    arg[i] = Rts_argv[num][i];
  } while (arg[i]);
  arg[i+1] = (CHAR)0;
}

void Rts_Terminate (void) {
  exit((int) Rts_exitCode);
}

void Rts_Error (const CHAR *msg) {
  fprintf (stderr, "*** Error: %s\n", msg);
  exit(1);
}

void Rts_Assert (BOOLEAN expr, const CHAR *msg) {
  if (!expr) {
    fprintf (stderr, "*** Assert: %s\n", msg);
    exit(1);
  }
}

void Rts_TerminationProc (void (*func)(void)) {
  int i;

  for (i = 0; i != MAX_TERM_PROCS; i++) {
    if (!(Rts_termProcs[i])) {
      Rts_termProcs[i] = func;
      return;
    }
  }

  fprintf (stderr, "[Rts] Couldn't add termination proc, aborting.\n");
  exit(1);
}

INTEGER Rts_System (const CHAR *command) {
  return (INTEGER) system(command);
}


static void Rts_RunTermProcs (void) {
  int i;
  void (*ptr)(void);

  for (i = 0; i != MAX_TERM_PROCS; i++) {
    ptr = Rts_termProcs[i];
    if (ptr) {
      Rts_termProcs[i] = NULL;
      (*ptr)();
    }
  }
}

static void Rts_SignalHandler (int sig) {
  signal(sig, SIG_DFL);  /* install default handler, necessary for SunOS 4 */
  if (Rts_noSignalHandlerInProgress) {
    Rts_noSignalHandlerInProgress = 0;
    Rts_RunTermProcs();
  }
  kill(getpid(), sig);   /* raise signal to call default handler */
}

static void Rts_CatchSignal (int sig) {
  void (*func)(int); 

  func = signal (sig, Rts_SignalHandler);
  if (func == SIG_IGN) {
    signal (sig, SIG_IGN);
  }
}

void _init_Rts (void) {
  int i, result;
  
  moduleId = add_module ("Rts");
  Rts_exitCode = 0;
  for (i=0; i != MAX_TERM_PROCS; i++) {
    Rts_termProcs[i] = NULL;
  }

  /* error signals */
  Rts_CatchSignal(SIGFPE);
  Rts_CatchSignal(SIGILL);
  Rts_CatchSignal(SIGSEGV);
  Rts_CatchSignal(SIGBUS);
  Rts_CatchSignal(SIGABRT);
  /* termination signals */
  Rts_CatchSignal(SIGHUP);
  Rts_CatchSignal(SIGINT);
  Rts_CatchSignal(SIGQUIT);
  Rts_CatchSignal(SIGTERM);
  /* normal program exit */
#if defined(sun) && !defined(__svr4__)
  result = on_exit(&Rts_RunTermProcs,(void*)0);
#else
  result = atexit(&Rts_RunTermProcs);
#endif
  if (result) {
    fprintf (stderr, "[Rts] Couldn't install termination handler, aborting.\n");
    exit(1);
  }
}
