#ifndef __Unix_
#define __Unix_

#include "_OGCC.h"
#include "CType.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

typedef INTEGER Unix_fd;
#if 1
#define Unix_timeval timeval
#else
typedef struct Unix_timeval {
  LONGINT tv_sec;
  INTEGER tv_usec;
} Unix_timeval;
#endif
typedef void (* Unix_Proc) (void);
extern INTEGER Unix_stdin, Unix_stdout, Unix_stderr;
extern INTEGER Unix_Errno (void);
extern BOOLEAN Unix_is_hugeval (LONGREAL x);
#if 0
extern INTEGER printf (const CHAR (* template), ...);
extern INTEGER sprintf (CHAR (* Unix_s), const CHAR (* template), ...);
extern void exit (INTEGER status);
extern INTEGER system (const CHAR (* command));
extern INTEGER gettimeofday (struct Unix_timeval (* Unix_tp), LONGINT tzp);
extern INTEGER settimeofday (struct Unix_timeval (* Unix_tp), LONGINT tzp);
extern INTEGER fputc (INTEGER c, INTEGER stream);
extern INTEGER fputs (const CHAR (* s), INTEGER stream);
extern INTEGER fgetc (INTEGER stream);
extern INTEGER fgets (CHAR (* Unix_s), INTEGER count, LONGINT stream);
extern LONGINT strtol (const CHAR (* string), LONGINT (* Unix_tailptr), INTEGER base);
extern LONGREAL strtod (const CHAR (* string), LONGINT (* Unix_tailptr));
extern INTEGER fopen (const CHAR (* filename), const CHAR (* opentype));
extern INTEGER fclose (LONGINT stream);
extern INTEGER unlink (const CHAR (* filename));
extern INTEGER rename (const CHAR (* oldname), const CHAR (* newname));
extern LONGINT ftell (LONGINT stream);
extern INTEGER fseek (LONGINT stream, LONGINT offset, INTEGER whence);
extern INTEGER fflush (LONGINT stream);
extern LONGINT fread (LONGINT data, LONGINT size, LONGINT count, LONGINT stream);
extern LONGINT fwrite (LONGINT data, LONGINT size, LONGINT count, LONGINT stream);
extern INTEGER feof (LONGINT stream);
extern INTEGER access (const CHAR (* filename), INTEGER how);
extern INTEGER getuid (void);
extern INTEGER getgid (void);
extern LONGINT tmpnam (CHAR (* Unix_result));
extern LONGINT getcwd (CHAR (* Unix_buffer), INTEGER sizeBuffer);
extern LONGINT time (LONGINT result);
extern LONGINT mktemp (CHAR (* Unix_template));
extern LONGINT tmpfile (void);
extern INTEGER open (const CHAR (* path), INTEGER pflag, ...);
extern INTEGER close (INTEGER fildes);
extern LONGINT lseek (INTEGER fildes, LONGINT offset, INTEGER whence);
extern INTEGER ftruncate (INTEGER fildes, LONGINT length);
extern LONGINT read (INTEGER fildes, LONGINT buf, LONGINT nbyte);
extern LONGINT write (INTEGER fildes, LONGINT buf, LONGINT nbyte);
#endif

extern void _init_Unix (void);

#endif
