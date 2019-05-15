MODULE Files["C"] EXTERNAL ["Files.c"];
(* see also [Oakwood Guidelines, revision 1A]
Module Files provides operations on files and the file directory.

Operations for unformatted input and output
In general, all operations must use the following format for external
representation:
- 'Little endian' representation (i.e., the least significant byte of a word is
  the one with the lowest address on the file).
- Numbers: SHORTINT 1 byte, INTEGER 2 bytes, LONGINT 4 bytes
- Sets: 4 bytes, element 0 is the least significant bit
- Booleans: single byte with FALSE = 0, TRUE = 1
- Reals: IEEE standard; REAL 4 bytes, LONGREAL 8 bytes
- Strings: with terminating 0X


Examples:
  VAR f: Files.File; r: Files.Rider; ch: CHAR;

Reading from an existing file:
  f := Files.Old ("xxx");
  IF f # NIL THEN
    Files.Set (r, f, 0);
    Files.Read (r, ch);
    WHILE ~ r.eof DO
      Files.Read (r, ch)
    END
  END

Writing to a new file yyy:
  f := Files.New ("yyy");
  Files.Set (r, f, 0);
  Files.WriteInt (r, 8);
  Files.WriteString (r, " bytes");
  Files.Register (f)
*)

(* Note: This module implements virtual file descriptors, i.e. an unlimited 
         number of files can be open at the same time.  These files share 
         the limited number of file descriptors provided by the operating 
         system. *)

IMPORT
  Rts,SYSTEM,C:=CType;

CONST
  (* If you change these constants, be sure
     to also change them in 'Files.c' to the
     same values! *)
   
  largeBlock* = 512;
  sizeBuffer* = (4*largeBlock);
  maxFilenameLen* = 256;

TYPE
  Buffer =
    RECORD
      (* the buffer contains valid data from the file positions 
         start <= pos < end, data[i] corresponds to file[start+i] *)
      data: ARRAY sizeBuffer OF SYSTEM.BYTE;
      start,end: LONGINT;
      read: BOOLEAN;  (* TRUE iff the last operation on the file was a read *)
    END;
    
  File* = POINTER TO FileDesc;
  FileDesc* = 
    RECORD
      next: File;
      fd: C.int;(* the file's fd *)
      name,tmpn: ARRAY maxFilenameLen OF CHAR;
      flagMark,flagValid,flagRead,flagWrite,flagReg,flagKeepFd: BOOLEAN;
      pos: LONGINT;(* the position within the file *)
      buffer: Buffer;
    END;
    
  Rider* = 
    RECORD 
      file: File;
      eof-: BOOLEAN;
      res-: INTEGER;
      pos: LONGINT;
    END;


VAR
  permanentClose*["Files_permanentClose"]: BOOLEAN; (* default: FALSE *)
  (* [not Oakwood compliant] If set to TRUE, the behaviour of Files.Close
     is changed to deallocate the memory block that contains the file handle
     (and the file buffer).  See procedure Close for details. *)


PROCEDURE Old* ["Files_Old"] (name: ARRAY OF CHAR): File;
(* Old (fn) searches the name fn in the directory and returns the
   corresponding file.  If the name is not found, it returns NIL. *)

PROCEDURE New* ["Files_New"] (name : ARRAY OF CHAR): File;
(* New (fn) creates and returns a new file. The name fn is remembered for the
   later use of the operation Register.  The file is only entered into the
   directory when Register is called. *)

PROCEDURE Delete* ["Files_Delete"] (name: ARRAY OF CHAR; VAR res: INTEGER);
(* Delete (fn, res) removes the directory entry for the file fn without
   deleting the file. If res=0 the file has been successfully deleted.
   If there are variables referring to the file while Delete is called,
   they can still be used. *)
   
PROCEDURE Length* ["Files_Length"] (f: File): LONGINT;
(* Length (f) returns the number of bytes in file f. *)

PROCEDURE Close* ["Files_Close"] (VAR f: File);
(* Close (f) writes back the file buffers of f. The file is still accessible by
   its handle f and the riders positioned on it. If a file is not modified it
   is not necessary to close it. 
   Note: The above holds only for permanentClose=FALSE, otherwise the buffers
     are flushed and the file handle is deallocated (and `f' is set to NIL).
     All riders on this file will become invalid.  This behaviour and the
     variable permanentClose are not part of the Oakwood guidelines. *)

PROCEDURE Register* ["Files_Register"] (f: File);
(* Register (f) enters the file f into the directory together with the name
   provided in the operation New that created f. The file buffers are written
   back. Any existing mapping of this name to another file is overwritten. *)

PROCEDURE Purge* ["Files_Purge"] (f: File);
(* Purge (f) resets the length of file f to 0. *)

PROCEDURE Rename* ["Files_Rename"] (old, new: ARRAY OF CHAR; VAR res: INTEGER);
(* Rename (oldfn, newfn, res) renames the directory entry oldfn to newfn.
   If res=0 the file has been successfully renamed. If there are variables
   referring to the file while Rename is called, they can still be used. *)

PROCEDURE GetDate* ["Files_GetDate"] (f : File; VAR t, d: LONGINT);
(* GetDate (f, t, d) returns the time t and date d of the last modification of
   file f.
   The encoding is:
     hour = t DIV 4096; minute = t DIV 64 MOD 64; second = t MOD 64;
     year = d DIV 512; month = d DIV 32 MOD 16; day = d MOD 32. *)

PROCEDURE Set* ["Files_Set"] (VAR r: Rider; f: File; pos: LONGINT);
(* Set (r, f, pos) sets the rider r to position pos in file f. The field r.eof
   is set to FALSE.  The operation requires that 0 <= pos <= Length (f). *)

PROCEDURE Pos* ["Files_Pos"] (VAR r: Rider): LONGINT;
(* Pos (r) returns the position of the rider r. *)

PROCEDURE Base* ["Files_Base"] (VAR r: Rider): File;
(* Base (r) returns the file to which the rider r has been set. *)



PROCEDURE ReadBytes* ["Files_ReadBytes"] (VAR r: Rider; VAR x: ARRAY OF SYSTEM.BYTE; n: LONGINT);
(* ReadBytes (r, buf, n) reads n bytes into buffer buf starting at the
   rider position r. The rider is advanced accordingly. If less than n bytes
   could be read, r.res contains the number of requested but unread bytes. *)

PROCEDURE WriteBytes* ["Files_WriteBytes"] (VAR r: Rider; VAR x: ARRAY OF SYSTEM.BYTE; n: LONGINT);
(* WriteBytes (r, buf, n) writes the first n bytes from buf to rider r and
   advances r accordingly. r.res contains the number of bytes that could not
   be written (e.g., due to a disk full error). *)



PROCEDURE Read* ["Files_Read"] (VAR r: Rider; VAR x: SYSTEM.BYTE);
(* Read (r, x) reads the next byte x from rider r and advances r
   accordingly. *)

PROCEDURE ReadInt* ["Files_ReadInt"] (VAR r: Rider; VAR x: INTEGER);
(* ReadInt (r, i) read a integer number i from rider r and advance r
   accordingly. *)

PROCEDURE ReadLInt* ["Files_ReadLInt"] (VAR r: Rider; VAR x: LONGINT);
(* ReadLInt (r, i) read a long integer number i from rider r and advance r
   accordingly. *)

PROCEDURE ReadReal* ["Files_ReadReal"] (VAR r: Rider; VAR x: REAL);
(* ReadReal (r, x) read a real number x from rider r and advance r
   accordingly. *)

PROCEDURE ReadLReal* ["Files_ReadLReal"] (VAR r: Rider; VAR x: LONGREAL);
(* ReadLReal (r, x) read a long real number x from rider r and advance r
   accordingly. *)

PROCEDURE ReadNum* ["Files_ReadNum"] (VAR r: Rider; VAR x: LONGINT);
(* ReadNum (r, i) reads an integer number i from rider r and advances r
   accordingly.  The number i is compactly encoded. *)

PROCEDURE ReadString* ["Files_ReadString"] (VAR r: Rider; VAR x: ARRAY OF CHAR);
(* ReadString (r, s) reads a sequence of characters (including the terminating
   0X) from rider r and returns it in s. The rider is advanced accordingly.
   The actual parameter corresponding to s must be long enough to hold the
   character sequence plus the terminating 0X *)

PROCEDURE ReadSet* ["Files_ReadSet"] (VAR r: Rider; VAR x: SET);
(* ReadSet (r, s) reads a set s from rider r and advances r accordingly. *)

PROCEDURE ReadBool* ["Files_ReadBool"] (VAR r: Rider; VAR x: BOOLEAN);
(* ReadBool (r, b) reads a Boolean value b from rider r and advances r
   accordingly. *)



PROCEDURE Write* ["Files_Write"] (VAR r: Rider; x: SYSTEM.BYTE);
(* Write (r, x) writes the byte x to rider r and advances r accordingly. *)

PROCEDURE WriteInt* ["Files_WriteInt"] (VAR r: Rider; x: INTEGER);
(* WriteInt (r, i) write the integer number i to rider r and advance r
   accordingly. *)

PROCEDURE WriteLInt* ["Files_WriteLInt"] (VAR r: Rider; x: LONGINT);
(* WriteLInt (r, i) write the long integer number i
   to rider r and advance r accordingly. *)

PROCEDURE WriteReal* ["Files_WriteReal"] (VAR r: Rider; x: REAL);
(* WriteReal (r, x) write the real number x to rider r and advance r
   accordingly. *)

PROCEDURE WriteLReal* ["Files_WriteLReal"] (VAR r: Rider; x: LONGREAL);
(* WriteLReal (r, x) write the long real number x to rider r and advance r
   accordingly. *)

PROCEDURE WriteNum* ["Files_WriteNum"] (VAR r: Rider; x: LONGINT);
(* WriteNum (r, i) writes the integer number i to rider r and advances r
   accordingly.  The number i is compactly encoded. *)

PROCEDURE WriteString* ["Files_WriteString"] (VAR r: Rider; x: ARRAY OF CHAR);
(* WriteString (r, s) writes the sequence of characters s (including the
   terminating 0X) to rider r and advances r accordingly. *)

PROCEDURE WriteSet* ["Files_WriteSet"] (VAR r: Rider; x: SET);
(* WriteSet (r, s) writes the set s to rider r and advances r accordingly. *)

PROCEDURE WriteBool* ["Files_WriteBool"] (VAR r: Rider; x: BOOLEAN);
(* WriteBool (r, b) writes the Boolean value b to rider r and advances r
   accordingly. *)

END Files.
