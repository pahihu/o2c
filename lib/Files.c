#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h> 
#ifdef __MINGW32__
#define S_IRGRP		0
#define S_IWGRP		0
#define S_IROTH		0
#define S_IWOTH		0
#else
#include <sys/uio.h>
#endif
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>

#define NOPROTOTYPES
#include "Files.h"
#include "_OGCC.h"



/* constants from Files.Mod */
#define largeBlock 512
#define sizeBuffer (4*largeBlock)
#define maxFilenameLen 256

typedef CHAR (* FileName);

static int maxFdUsed = 10; /* max. number of physical files used */
static Files_File openFiles = NULL; /* there are no files at startup */
static int fdInUse = 0; /* no fd's used by this module at startup*/
static int swapBytes = 0; /* do not swap bytes on write */
BOOLEAN Files_permanentClose = FALSE;

/**/
/**/

#ifdef GC
static void finalize(void *obj, void *client_data);
#endif
/**/
/**/




static void CreateTmpName(CHAR *name)
     /* create a name for a temporal file */
{
  CHAR *tmp = NULL;
  
#ifdef __MINGW64__
  CHAR *pdir, dir[256];
  pdir = getenv("TMP");
  if (!pdir)
  {
    strcpy(dir, "/tmp"); pdir = dir;
  }
  tmp = (CHAR*)tempnam(pdir,"o2c");
#else
  tmp = (CHAR*)tmpnam(NULL);
#endif
  COPY(tmp,name,maxFilenameLen);
}


static void ClearBuffer(Files_Buffer *buf)
{
   buf->start = 0;
   buf->end = 0;
   buf->read = TRUE;
}

static Files_File NewFile(const CHAR *name)
   /* Allocate memory for a new File structure and initialize it. */
{
   Files_File newfile;
   /* Allocate the memory with MALLOC_ATOMIC instead of MALLOC
      because we keep track of the allocated memory ourself
      and want to be informed, when the last pointer to this
      location is removed.
      If we would use MALLOC, then a File will never be
      collected due to the fact that it is always in the
      global ring buffer, if it could still be open.
      */
   
   newfile = (Files_File)MALLOC_ATOMIC(sizeof(Files_FileDesc));
#ifdef GC
   {
     GC_finalization_proc ofn;
     void *ocd;

     GC_register_finalizer(newfile,finalize,NULL,&ofn,&ocd);
   }
#endif
   newfile->next = NULL;
   newfile->fd = -1;

   COPY(name,newfile->name,maxFilenameLen);
   CreateTmpName(newfile->tmpn);
   newfile->flagValid = FALSE;
   newfile->flagRead = FALSE;
   newfile->flagWrite = FALSE;
   newfile->flagMark = FALSE;
   newfile->flagReg = FALSE;
   newfile->flagKeepFd = FALSE;
   newfile->pos = -1;

   ClearBuffer(&(newfile->buffer));
   
   return newfile;
}

/* */

static void DeallocateFileDesc(Files_File file)
   /* Close the file associated with 'file' and release the 'fd' */
{
   if (file->flagValid == TRUE)
   {
      int res;
      do
	 res = close(file->fd);
      while ((res != 0) && (errno == EINTR));
      
      file->fd = -1;
      file->flagValid = FALSE;
      file->flagRead = FALSE;
      file->flagWrite = FALSE;
      file->flagMark = FALSE;
      fdInUse--;
   }
}

static void FreeFileDesc(void);

static int GetFileDesc(Files_File file, int new)
   /* allocate a file descriptor for 'file'.
      'new' reflects the open mode:
        TRUE: Files.New (create new file)
	FALSE: Files.Open (open existing file)
   */
{
   int fd = -1;
   FileName fn = NULL;

/* Have a look on the number of file descriptors
   we have allocated.
   If we have already 'maxFdUsed' file descriptors,
   then try to release some of them */
   
   if (fdInUse == maxFdUsed)
      FreeFileDesc();
   
/* reset the state of the file to be opened */   
   file->flagValid = FALSE;
   file->flagMark = FALSE;
   file->flagRead = FALSE;
   file->flagWrite = FALSE;
   
   if (file->flagReg == TRUE)
      /* file is registered, i.e. use the 'name' field to determine
	 the physical file */
      fn = file->name;
   else
      /* file was not yet registered, so use the 'tmpn' field
	 as the name of the physical file */
      fn = file->tmpn;
   
   if (new == TRUE)
   {
      /* Create a new file. The Unix permissions for the new file are
         set to rw-rw-rw */
      do
	 fd = open((char*)fn,(O_CREAT|O_TRUNC|O_RDWR),(S_IREAD|S_IWRITE|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
      while ((fd == -1) && (errno == EINTR));
      
      if ((fd == -1) && ((errno == EMFILE) || (errno == ENFILE)) && (fdInUse != 0))
      {
	/* close one file and try again */
	FreeFileDesc();
	return GetFileDesc(file,new);
      }
      else if (fd == -1)
	{
	  return FALSE;
	}
      else
      {
	 file->fd = fd;
	 file->flagValid = TRUE;
	 file->flagRead = TRUE;
	 file->flagWrite = TRUE;
      }
   }
   else
   {
      /* try to open the file for reading/writing */
      do
	 fd = open((char*)fn,(O_RDWR));
      while ((fd == -1) && (errno == EINTR));
      
      if ((fd == -1) && ((errno == EMFILE) || (errno == ENFILE)) && (fdInUse != 0))
      {
	 FreeFileDesc();
	 return GetFileDesc(file,new);
      }
      else if (fd != -1)
      {
	 file->fd = fd;
	 file->flagValid = TRUE;
	 file->flagRead = TRUE;
	 file->flagWrite = TRUE;
      }
      else
      { /* Open failed for reading/writing, so we try to open the file
           just for reading */
	 do
	    fd = open((char*)fn,(O_RDONLY)); 
	 while ((fd == -1) && (errno == EINTR));
	 
	 if ((fd == -1) && ((errno == EMFILE) || (errno == ENFILE)) && (fdInUse != 0))
	 {
	    FreeFileDesc();
	    return GetFileDesc(file,new);
	 }
	 else if (fd != -1)
	 {
	    file->fd = fd;
	    file->flagValid = TRUE;
	    file->flagRead = TRUE;
	    file->flagWrite = FALSE;
	 }
	 else
	    return FALSE;
      }
   }
   fdInUse++;
   lseek(file->fd,file->pos,SEEK_SET);
   return TRUE;
}


static void AllocateFileDesc(Files_File file)
{
   if (file->flagValid == TRUE)
      file->flagMark = FALSE; /* touch */
   else if (GetFileDesc(file,FALSE) == FALSE)
   {
      fprintf(stderr,"[Files] Failed to reopen file %s\n",file->name);
      Rts_Terminate(); 
   }
}


static void FreeFileDesc(void)
{
   Files_File walk = openFiles;
   do
   {
      if ((walk->flagValid == TRUE) && (walk->flagKeepFd == FALSE))
      {
	 if (walk->flagMark == TRUE)
	 {
	    DeallocateFileDesc(walk);
	    openFiles = walk;
	    return;
	 }
	 else
	    walk->flagMark = TRUE;
      }
      walk = walk->next;
   } while (walk != openFiles);
   FreeFileDesc();
}

/* */


static void InsertFile(Files_File file)
{
   if (openFiles == NULL)
   {
      openFiles = file;
      file->next=file;
   }
   else
     {
     Files_File oldEnd = openFiles;
     
     while (oldEnd->next != openFiles)
       oldEnd = oldEnd->next;
     
     oldEnd->next = file;
     file->next = openFiles;
   }
}

#ifdef GC
static BOOLEAN StillInUse(Files_File file)
{
  Files_File walk = openFiles;

  while ((walk != NULL) && (walk != file) && (walk->next != openFiles))
    walk = walk->next;
  return (file == walk);
}
#endif


static void RemoveFile(Files_File file)
{
   DeallocateFileDesc(file);
   if (openFiles != NULL)
   {
      if (file->next == file)
	 openFiles = NULL;
      else
      {
	Files_File walk = openFiles;

	 while ((walk->next != file) && (walk->next != openFiles))
	    walk = walk->next;

	 walk->next = file->next;
	 if (file == openFiles)
	    openFiles = file->next;
      }
   }
   else
      fprintf(stderr,"Unbalanced Close on Files\n");
}

/* */

static LONGINT WriteToFile(Files_File f, LONGINT pos, BYTE *adr, LONGINT n)
   /* WriteToFile (f, pos, adr, n) writes the first n bytes starting at
      memory address adr to file f at position pos.
      f.pos is updated accordingly.
      Result is 0 on success and the number of bytes that could
      not be written on failure. */
{
   int res;
   
   AllocateFileDesc (f);
   if (f->flagWrite == TRUE)  /* fd is valid */
   {
      res = lseek (f->fd, pos, SEEK_SET);
      do
	 res = write (f->fd, adr, n);
      while ((res == -1) && (errno == EINTR));
      if (res == -1)  /* write error */
	 res = 0;
      f->pos = pos+res;
      return (n-res);
   }
   else
      return n;
}

static LONGINT ReadFromFile(Files_File f, LONGINT pos, BYTE *adr, LONGINT n)
   /* ReadFromFile (f, pos, buf, n) reads the first n bytes at
      position pos from file f and stores them at memory address adr.
      f.pos is updated accordingly.
      Result is 0 on success and the number of bytes that could
      not be read on failure. */
{
   long int res;
   
   AllocateFileDesc (f);
   if (f->flagRead == TRUE) /* fd is valid */
   {
      res = lseek (f->fd, pos, SEEK_SET);
      do
	 res = read (f->fd, adr, n);
      while ((res == -1) && (errno == EINTR));
      if (res == -1) /* read error */
	 res = 0;

      f->pos = pos+res;
      return (n-res);
   }
   else
      return n;
}

static void Flush(Files_File f)
{
   LONGINT res;
   
   if ((f->buffer.read == FALSE) && (f->buffer.end != f->buffer.start))
   {
      /* write buffer to file if necessary */
      res = WriteToFile(f,f->buffer.start,&(f->buffer.data[0]),(f->buffer.end - f->buffer.start));
      f->buffer.end = f->buffer.start; /* clear buffer */
   }
}

/* */

static void SwapOrder(BYTE x[], LONGINT len)
{
   if (swapBytes)
   {
      LONGINT i,j;
      BYTE c;
      /* swap the bytes  */
      i=0;
      j = len-1;
      while (i < j)
      {
	 c=x[i]; x[i]=x[j]; x[j]=c;
	 i++;
	 j--;
      }
   }
}

static void InitSwap(BYTE x[])
{
   swapBytes=(x[0] != 1);
/*
   if (swapBytes)
      fprintf(stderr,"Files.InitSwap: big endian system\n");
   else
      fprintf(stderr,"Files.InitSwap: little endian system\n");
*/
}

/* */







Files_File Files_Base(struct Files_Rider *Files_r)
{
   if (Files_r != NULL)
      return Files_r->file;
   else
      return NULL;
}

static void CloseDown(Files_File f)
     /* CloseDown (f) writes back the file buffers of f. The file is no more
	accessible by its handle f or the struct Files_Riders positioned on it. */
{
  INTEGER res;
  
#ifdef GC
  if (StillInUse(f))
    {
#endif
      Flush(f);
      DeallocateFileDesc(f);
      /* remove it from the internal tracking list */
      RemoveFile(f);
      if (f->flagReg == FALSE)
	/* File was not registered, so delete the temporary file. */
	Files_Delete(f->tmpn, &res);
#ifdef GC
    }
/*  else 
    {
      fprintf(stderr,"Not in use\n");
    }
*/
#endif
}

void Files_Close(Files_File *f)
/* Close (f) writes back the file buffers of f. The file is still accessible by
   its handle f and the struct Files_Riders positioned on it. If a file is not modified it
   is not necessary to close it.
   
   Note: The above holds only for permanentClose=FALSE, otherwise the buffers
   are flushed and the file handle is deallocated (and `f' is set to NIL).
   All struct Files_Riders on this file will become invalid.  This behaviour and the
   variable permanentClose are not part of the Oakwood guidelines. */
{
   Flush(*f);
   if (Files_permanentClose)
   {
     
      CloseDown (*f);
#ifdef GC
      {
	GC_finalization_proc ofn;
	void *ocd;
	
	GC_register_finalizer(*f,NULL,NULL,&ofn,&ocd);
      }
#endif
      DISPOSE(*f); 
   }
   else
   {
      DeallocateFileDesc (*f);
   }
}


void Files_Delete(const CHAR *name, INTEGER *Files_res)
{
   *Files_res = (-unlink((char*)name));
}

int get_o2time (long int sec, LONGINT *t, LONGINT *d)
{
   time_t clock;
   struct tm *timestamp;
   
   clock = sec;
   timestamp = localtime(&clock);
   if (timestamp != NULL) {
      (*t) = (timestamp->tm_hour * 4096) +
	 (timestamp->tm_min * 64) +
	 (timestamp->tm_sec);
      (*d)=((timestamp->tm_year + 1900) * 512) +
	 ((timestamp->tm_mon+1) * 32) +
	 (timestamp->tm_mday);
      return 0;
   }
   else
      return -1;
}

void Files_GetDate(Files_File f, LONGINT *t, LONGINT *d)
   /* GetDate (f, t, d) returns the time t and date d of the
      last modification of file f.
      The encoding is:
      hour = t DIV 4096; minute = t DIV 64 MOD 64; second = t MOD 64;
      year = d DIV 512; month = d DIV 32 MOD 16; day = d MOD 32. */
{
   struct stat attr;
   int res;
   
   AllocateFileDesc(f);
   res = fstat(f->fd,&attr);

   if (res == 0)
   {
      if (get_o2time(attr.st_mtime,t,d) != 0)
      {
	 (*t)=0;
	 (*d)=0;
      }
   }
   else
   {
      (*t)=0;
      (*d)=0;
   }   
}


LONGINT Files_Length(Files_File f)
{
   int res;
   struct stat attr;
   FileName name;

   if (f->flagReg)
      name = f->name;
   else
      name = f->tmpn;
   
   res = stat ((char*)name, &attr);
   if (res != 0)
      return -1;
   else
      return attr.st_size;
}


Files_File Files_New(const CHAR *name)
{
   Files_File f = NULL;

   f = NewFile(name);

   if (GetFileDesc(f,TRUE) == TRUE)
   {
      InsertFile(f);
      return f;
   }
   else
   {
      fprintf(stderr,"Files.New: Could not create %s for read/write\n",f->tmpn);
      return NULL;
   }
}


Files_File Files_Old(const CHAR *name)
{
   Files_File f = NULL;

   f = NewFile(name);
   f->flagReg = TRUE;

   if (GetFileDesc(f,FALSE) == TRUE)
   {
      InsertFile(f);
      return f;
   }
   else
      return NULL;
}


LONGINT Files_Pos(struct Files_Rider *Files_r)
{
   if (Files_r != NULL)
      return Files_r->pos;
   else
      return -1;
}

void Files_Purge(Files_File f)
{
   int res;
   
   AllocateFileDesc(f);
   res = ftruncate(f->fd,0);
   /* clear buffer */
   ClearBuffer(&(f->buffer));
}

void Files_ReadBytes(struct Files_Rider *Files_r, BYTE *Files_x, LONGINT n)
{
   Files_File f;
   LONGINT n0;
   
   f = Files_r->file;
   if (f->buffer.read && (f->buffer.start <= Files_r->pos) && (Files_r->pos+n <= f->buffer.end))
    {   
       /* all required bytes are in the file buffer */
      MOVE (&(f->buffer.data[Files_r->pos-f->buffer.start]),Files_x, n);
      Files_r->pos+= n;
      Files_r->res = 0;
    }
    else if (n < largeBlock)
    { /* block small enough to update buffer */
       /* refill buffer */
       Flush (f);
       n0 = sizeBuffer - ReadFromFile (f, Files_r->pos, &(f->buffer.data[0]), sizeBuffer);
       /* n0 is the number of bytes that were actually read into buffer */
       f->buffer.start = Files_r->pos;
       f->buffer.end = Files_r->pos+n0;
       /* not more than n0 bytes can be taken from the buffer */
       if (n > n0)
       {
	  Files_r->res = n-n0;
	  n = n0;
	  Files_r->eof = TRUE;
       }
       else
       {
	  Files_r->res = 0;
       }
       MOVE (&(f->buffer.data[0]),Files_x, n);
       Files_r->pos += n;
    }
   else
   {
      /* bytes not in buffer and block too large to use buffer */
      Flush (f);
      Files_r->res = ReadFromFile (Files_r->file, Files_r->pos, Files_x, n);
      if (Files_r->res != 0)
      { /* couldn't read n bytes: probably end of file */
	 Files_r->eof = TRUE;
      }
      Files_r->pos = f->pos;
   }
}

void Files_WriteBytes(struct Files_Rider *Files_r, BYTE *Files_x, LONGINT n)
{
   Files_File f;

   f = Files_r->file;
   if ((!f->buffer.read) &&
       (f->buffer.start <= Files_r->pos) && (Files_r->pos <= f->buffer.end) &&
       (Files_r->pos+n <= f->buffer.start+sizeBuffer))
   { /* bytes fit into buffer */
      MOVE (Files_x,&(f->buffer.data[Files_r->pos-f->buffer.start]), n);
      Files_r->pos += n;
      Files_r->res = 0;
      if (Files_r->pos > f->buffer.end)
      {
	 f->buffer.end = Files_r->pos;
      }
   }
   else if (n < largeBlock)
   {
      /* block small enough to put it into the buffer */
      Flush (f);
      f->buffer.read = FALSE;
      f->buffer.start = Files_r->pos;
      Files_r->pos += n;
      Files_r->res = 0;
      f->buffer.end = Files_r->pos;
      MOVE (Files_x, &(f->buffer.data[0]), n);
   }
   else
   {
      /* block to large, write directly to file */
      Flush (f);
      Files_r->res = WriteToFile (f, Files_r->pos, Files_x, n);
      Files_r->pos = f->pos;
   }
}

static Files_File regFile = NULL;

void Files_Register(Files_File f)
   /* Register (f) enters the file f into the directory together with the name
      provided in the operation New that created f. The file buffers are
      written back. Any existing mapping of this name to another file is
      overwritten. */
{
#define bufSize  (32 * 1024)
   
   LONGINT oldpos;
   INTEGER dummy;
   int res;
   BYTE buffer[bufSize];
   BOOLEAN err;
   LONGINT n;
   
   if (regFile == NULL)
   {
      regFile = NewFile(f->name);
/*      COPY(f->name,regFile->name,maxFilenameLen); */
      regFile->flagReg = TRUE;
   }

   if (f->flagReg == FALSE)
   {
      Flush(f);
      oldpos = f->pos;
      COPY(f->name,regFile->name,maxFilenameLen);
      regFile->fd = -1;
      regFile->pos = 0;
      regFile->buffer.start = 0;
      regFile->buffer.end = 0;

      /* try to open old file (whose contents are to be copied into to
         registered file) and the new file, which will reside in the
         registered file's place */
      AllocateFileDesc(f);
      f->flagKeepFd = TRUE; /* ensure that f keeps it's fd */
      if (GetFileDesc(regFile,TRUE) == TRUE)
	 /* new file could be opened, so 'f' becomes registered */
      {
	 InsertFile(regFile);
         f->flagReg = TRUE;
      }
      f->flagKeepFd = FALSE;  /* make f's fd available again */

      if ((regFile->flagValid == FALSE) ||
	  (regFile->flagWrite == FALSE) ||
	  (f->flagValid == FALSE) ||
	  (f->flagRead == FALSE))
      {
	 fprintf(stderr,"[Files] Unable to register file %s\n",(char*)regFile->name);
	 Rts_Terminate();
      }

      /* copy the whole file */
      /* set both positions to start of file */
      res = lseek(f->fd,0,SEEK_SET);
      res = lseek(regFile->fd,0,SEEK_SET);
      err = FALSE;
      while(1)
      {
	 do
	    n = read (f->fd, &buffer, bufSize);
	 while ((n == -1) && (errno == EINTR));
	 if (n == -1)
	 {
	    err = TRUE;
	    goto _loopend;
	 }
	 do
	    n = write (regFile->fd, &buffer, n);
	 while ((n == -1) && (errno == EINTR));
	 if (n == -1)
	 {
	    err = TRUE;
	    goto _loopend;
	 }
	 if (n != bufSize)
	    goto _loopend;
      }
      _loopend:
      if (err == TRUE)
      {
	 Files_Delete(f->name,&dummy); /* an error occured during registration, remove file */
	 fprintf(stderr,"*** Warning: [Files] Unable to register file %s\n",regFile->name);
	 f->pos = oldpos; /* set the old position */
	 f->flagReg =FALSE;
      }
      else
	{
	 CloseDown(regFile); /* Remove 'regFile' from tracking list */
	 Files_Delete(f->tmpn,&dummy); /* delete the old temporary file */
	 DeallocateFileDesc(f);
	 f->pos = oldpos; /* set the old position */
	 AllocateFileDesc(f);  /* try to get a file descriptor for 'f' */
      }
   }
}

void Files_Rename(const CHAR *old, const CHAR *new, INTEGER *Files_res)
{
   *Files_res = -(rename((char*)old,(char*)new));
}

void Files_Set(struct Files_Rider *Files_r, Files_File f, LONGINT pos)
{
   if (Files_r != NULL)
   {
      Files_r->file=f;
      AllocateFileDesc(f);
      Flush(f);
      Files_r->eof = FALSE;
      Files_r->pos = pos;
   }
   else
      fprintf(stderr,"Files.Set: struct Files_Rider not valid\n");
}

static void WriteBytesSwap(struct Files_Rider *Files_r, BYTE *Files_x, LONGINT n)
{
   SwapOrder(Files_x,n);
   Files_WriteBytes(Files_r,Files_x,n);
}

static void ReadBytesSwap(struct Files_Rider *Files_r, BYTE *Files_x, LONGINT n)
{
   Files_ReadBytes(Files_r,Files_x,n);
   SwapOrder(Files_x,n);
}



void Files_Write (struct Files_Rider *Files_r, BYTE x)
   /* Write (r, x) writes the byte x to struct Files_Rider r and advances r accordingly. */
{
   if ((Files_r->file->buffer.read == FALSE) &&
       (Files_r->file->buffer.start <= Files_r->pos) &&
       (Files_r->pos <= Files_r->file->buffer.end) &&
       (Files_r->file->buffer.end < (Files_r->file->buffer.start+sizeBuffer)))
      /* previous operation was a write, the current byte is in the buffer
         and the buffer won't overflow is a byte is appended */
   {
      Files_r->file->buffer.data[(Files_r->pos - Files_r->file->buffer.start)] = x;
      Files_r->pos++;
      if (Files_r->file->buffer.end < Files_r->pos)
	 Files_r->file->buffer.end = Files_r->pos;
   }
   else
      Files_WriteBytes(Files_r,&x,sizeof(BYTE));
}

void Files_WriteBool (struct Files_Rider *Files_r, BOOLEAN x)
{
   if (x == TRUE)
      Files_Write(Files_r,'\001');
   else
      Files_Write(Files_r,'\000');
}

void Files_WriteInt (struct Files_Rider *Files_r, INTEGER x)
{
   WriteBytesSwap(Files_r,(BYTE*)&x,sizeof(INTEGER));
}

void Files_WriteLInt (struct Files_Rider *Files_r, LONGINT x)
{
   WriteBytesSwap(Files_r,(BYTE*)&x,sizeof(LONGINT));
}

void Files_WriteLReal (struct Files_Rider *Files_r, LONGREAL x)
{
   WriteBytesSwap(Files_r,(BYTE*)&x,sizeof(LONGREAL));
}

void Files_WriteNum (struct Files_Rider *Files_r, LONGINT x)
{
   while ((x< -64) || (x >63))
   {
      Files_Write(Files_r, (CHAR)(MODLI(x, 128, -1)+128));
      x = DIVLI(x, 128, -1);
   }
   Files_Write(Files_r,(CHAR)(MODLI(x, 128, -1)));
}

void Files_WriteReal (struct Files_Rider *Files_r, REAL x)
{
   WriteBytesSwap(Files_r,(BYTE*)&x,sizeof(REAL));
}

void Files_WriteSet (struct Files_Rider *Files_r, SET x)
{
   WriteBytesSwap(Files_r,(BYTE*)&x,sizeof(SET));
}

void Files_WriteString (struct Files_Rider *Files_r, const CHAR *Files_x)
{
   LONGINT i=-1;
   do
   {
      i++;
      Files_Write(Files_r,(BYTE)Files_x[i]);
   } while (Files_x[i] != '\000');
}

void Files_Read (struct Files_Rider *Files_r, BYTE *Files_x)
{
   Files_ReadBytes(Files_r,Files_x,sizeof(BYTE));
}


void Files_ReadBool (struct Files_Rider *Files_r, BOOLEAN *Files_x)
{
   BYTE y;
   Files_Read(Files_r,&y);
   if (y == 1)
      *Files_x=TRUE;
   else
      *Files_x=FALSE;
}

void Files_ReadInt (struct Files_Rider *Files_r, INTEGER *Files_x)
{
   ReadBytesSwap(Files_r,(BYTE*)Files_x,sizeof(INTEGER));
}

void Files_ReadLInt (struct Files_Rider *Files_r, LONGINT *Files_x)
{
   ReadBytesSwap(Files_r,(BYTE*)Files_x,sizeof(LONGINT));
}

void Files_ReadLReal (struct Files_Rider *Files_r, LONGREAL *Files_x)
{
   ReadBytesSwap(Files_r,(BYTE*)Files_x,sizeof(LONGREAL));
}

void Files_ReadNum (struct Files_Rider *Files_r, LONGINT *Files_x)
{
   SHORTINT _s = 0;
   CHAR _ch;
   LONGINT _n = 0;

   Files_Read(Files_r, &(_ch));
   while (((INTEGER)_ch >= 128) && (Files_r->eof == FALSE))
   {
      INCLI(_n, ASHI((INTEGER) _ch-128, _s, -1), -1);
      INCSI(_s, 7, -1);
      Files_Read(Files_r, &(_ch));
   }

   if (Files_r->eof == TRUE) 
   {
      Files_r->res = 1; /* indicate error */
      (* Files_x) = 0;   /* asure defined state */
      fprintf(stderr,"[Files.ReadNum] End of file reached in '%s'\n",(char*)Files_r->file->name);
/*      Rts_Terminate(); */
   }
   else 
   {
      Files_r->res = 0;
      (* Files_x) = _n+ASHI(MODI((INTEGER) _ch, 64, -1)-(DIVI((INTEGER) _ch, 64, -1))*64, _s, -1);
   }
}

void Files_ReadReal (struct Files_Rider *Files_r, REAL *Files_x)
{
   ReadBytesSwap(Files_r,(BYTE*)Files_x,sizeof(REAL));
}

void Files_ReadSet (struct Files_Rider *Files_r, SET *Files_x)
{
   ReadBytesSwap(Files_r,(BYTE*)Files_x,sizeof(SET));
}

void Files_ReadString (struct Files_Rider *Files_r, CHAR *Files_x)
{
   LONGINT i = -1;
   do
   {
      i++;
      Files_Read(Files_r,&(Files_x[i]));
   } while ((Files_x[i] != '\000') && (Files_r->eof == FALSE));


   if (Files_r->eof == TRUE) 
   {
      Files_r->res = 1; /* indicate error */
      Files_x[0] = '\000'; /* asure 'usable' state */
      fprintf(stderr,"[Files.ReadString] End of file reached in '%s'\n",(char*)Files_r->file->name);
/*      Rts_Terminate(); */
   }
   else
      Files_r->res = 0;
}

#ifdef GC
static void finalize(void *obj, void *client_data)
{
  CloseDown((Files_File)obj);
}
#endif

void Cleanup(void)
  /* Closes all ever opened files which are still valid. */
{
  while (openFiles != NULL)
    {
      CloseDown(openFiles);
    }
}


static int moduleId;

void _init_Files (void)
{
   INTEGER swapTest = 1;
   moduleId = add_module ("Files"); 
   InitSwap((BYTE*)&swapTest);
   fdInUse = 0;
   Rts_TerminationProc(Cleanup);
}
