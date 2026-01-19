#ifndef __Unix_
#define __Unix_

#include "_OGCC.h"
#include "CType.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

typedef LONGINT Unix_fd;
#if 1
#define Unix_timeval timeval
#else
typedef struct Unix_timeval {
  LONGINT tv_sec;
  LONGINT tv_usec;
} Unix_timeval;
#endif
typedef void (* Unix_Proc) (void);
extern LONGINT Unix_stdin, Unix_stdout, Unix_stderr;
extern LONGINT Unix_Errno (void);
extern BOOLEAN Unix_is_hugeval (LONGREAL x);
#if 0
extern LONGINT printf (const CHAR (* template), ...);
extern LONGINT sprintf (CHAR (* Unix_s), const CHAR (* template), ...);
extern void exit (LONGINT status);
extern LONGINT system (const CHAR (* command));
extern LONGINT gettimeofday (struct Unix_timeval (* Unix_tp), LONGINT tzp);
extern LONGINT settimeofday (struct Unix_timeval (* Unix_tp), LONGINT tzp);
extern LONGINT fputc (LONGINT c, LONGINT stream);
extern LONGINT fputs (const CHAR (* s), LONGINT stream);
extern LONGINT fgetc (LONGINT stream);
extern LONGINT fgets (CHAR (* Unix_s), LONGINT count, LONGINT stream);
extern LONGINT strtol (const CHAR (* string), LONGINT (* Unix_tailptr), LONGINT base);
extern LONGREAL strtod (const CHAR (* string), LONGINT (* Unix_tailptr));
extern LONGINT fopen (const CHAR (* filename), const CHAR (* opentype));
extern LONGINT fclose (LONGINT stream);
extern LONGINT unlink (const CHAR (* filename));
extern LONGINT rename (const CHAR (* oldname), const CHAR (* newname));
extern LONGINT ftell (LONGINT stream);
extern LONGINT fseek (LONGINT stream, LONGINT offset, LONGINT whence);
extern LONGINT fflush (LONGINT stream);
extern LONGINT fread (LONGINT data, LONGINT size, LONGINT count, LONGINT stream);
extern LONGINT fwrite (LONGINT data, LONGINT size, LONGINT count, LONGINT stream);
extern LONGINT feof (LONGINT stream);
extern LONGINT access (const CHAR (* filename), LONGINT how);
extern LONGINT getuid (void);
extern LONGINT getgid (void);
extern LONGINT tmpnam (CHAR (* Unix_result));
extern LONGINT getcwd (CHAR (* Unix_buffer), LONGINT sizeBuffer);
extern LONGINT time (LONGINT result);
extern LONGINT mktemp (CHAR (* Unix_template));
extern LONGINT tmpfile (void);
extern LONGINT open (const CHAR (* path), LONGINT pflag, ...);
extern LONGINT close (LONGINT fildes);
extern LONGINT lseek (LONGINT fildes, LONGINT offset, LONGINT whence);
extern LONGINT ftruncate (LONGINT fildes, LONGINT length);
extern LONGINT read (LONGINT fildes, LONGINT buf, LONGINT nbyte);
extern LONGINT write (LONGINT fildes, LONGINT buf, LONGINT nbyte);
#endif

extern void _init_Unix (void);

#endif
