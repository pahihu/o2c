#include "_OGCC.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pwd.h>


static ModuleId moduleId;


const CHAR Dos_pathSeperator = '/';

void Dos_Delete (const CHAR *file, BOOLEAN *err) {
  *err = (unlink(file) != 0);
}

void Dos_Rename (const CHAR *old, const CHAR *new, BOOLEAN *err) {
  *err = (rename(old,new) == -1);
}

void Dos_Copy (const CHAR *src, const CHAR *dest, BOOLEAN *err) {
  const LONGINT chunkSize = 32768;
  FILE *in, *out;
  CHAR buffer[32768];
  LONGINT wantRead, hasRead, hasWritten, inLen;
  struct stat attr;

  *err = FALSE;
  in = fopen(src, "r");
  if (fstat (fileno (in), &attr)) {
    inLen = attr. st_size;
  } else {
    inLen = -1;
  }
  if ((in != 0) && (inLen >= 0))  {
    out = fopen(dest, "w");
    if (out != 0)  {
      while (!(*err) && (inLen>0))  {
        if (inLen > chunkSize)  {
          wantRead = chunkSize;
        } else {
          wantRead = inLen;
        }
        hasRead = fread(buffer, 1, wantRead, in);
        if (hasRead == wantRead)  {
          hasWritten = fwrite(buffer, 1, hasRead, out);
          *err = (hasWritten != hasRead);
        } else {
          *err = TRUE;
        }
	inLen -= hasRead;
      }
      *err = ((fclose(out) != 0) || *err);
    } else {
      *err = TRUE;
    }
    *err = ((fclose(in) != 0) || *err);
  } else {
    *err = TRUE;
  }
}

BOOLEAN Dos_Exists (const CHAR *file) {
  return (access(file,F_OK) == 0);
}

void Dos_GetDate (const CHAR *file, struct timeval *date, BOOLEAN *err) {
  struct stat attr;
  
  *err = (stat (file, &attr) != 0);
  date->tv_sec = attr. st_mtime;
  date->tv_usec = 0;
  /* date->usec = attr. st_mtime_usec; */
}

void Dos_GetUserHome (CHAR *home, const CHAR *user) {
  struct passwd *entry;

  if (*user)
    entry=(void*)getpwnam(user);
  else
    entry=(void*)getpwuid(getuid());

  if (entry != NULL)
    strcpy(home, entry->pw_dir);  /* danguerous */
  else
    home[0] = '\000';
}

void Dos_GetCWD (CHAR *path) {
  getcwd(path, /* dangerous */ 1024);
}

void Dos_TmpFileName (CHAR *tmpnam_str) {
  CHAR buffer[512];
  
  if (tmpnam(buffer) != 0) {
    strcpy(tmpnam_str, buffer);  /* danguerous */
  } else {
    tmpnam_str[0] = '\000';
  }
}

void _init_Dos (void) {
  moduleId = add_module ("Dos");
}
