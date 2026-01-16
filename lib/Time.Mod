MODULE Time;

IMPORT
  C := CType, Unix, MathL;


TYPE
  Time* = Unix.timeval;


PROCEDURE Reset* (VAR time : Time);
  (* Clears time value, i.e. sets it to zero. *)
  BEGIN
    time. tv_sec := 0;
    time. tv_usec := 0
  END Reset;


PROCEDURE Add* (a, b : Time; VAR result : Time);
(* Adds a to b: result := b+a.
   post: result.tv_usec < 1000000 *)
  BEGIN
    result. tv_sec := b. tv_sec+a. tv_sec;
    result. tv_usec := b. tv_usec+a. tv_usec;
    IF (result. tv_usec >= 1000000) THEN
      INC (result. tv_sec);
      DEC (result. tv_usec, 1000000)
    END
  END Add;

PROCEDURE Sub* (a, b : Time; VAR result : Time);
(* Subtracts a from b: result := b-a.
   post: result.tv_usec >= 0 *)
  BEGIN
    result. tv_sec := b. tv_sec-a. tv_sec;
    result. tv_usec := b. tv_usec-a. tv_usec;
    IF (result. tv_usec < 0) THEN
      DEC (result. tv_sec);
      INC (result. tv_usec, 1000000)
    END
  END Sub;

PROCEDURE Mult* (a : Time; m : LONGREAL; VAR result : Time);
(* Multiply value in a m times: result := a*m.
   post: 0 <= result.tv_usec < 1000000 *)
  VAR
    int, frac : LONGREAL;
  BEGIN
    frac := MathL.modf ((a. tv_sec + a. tv_usec / 1.0D6)*m, int);
    result. tv_sec := ENTIER (int);
    result. tv_usec := SHORT (ENTIER (frac * 1.0D6)); (* XXX *)
    IF (result. tv_usec < 0) THEN
      DEC (result. tv_sec);
      INC (result. tv_usec, 1000000)
    ELSIF (result. tv_usec >= 1000000) THEN
      INC (result. tv_sec);
      DEC (result. tv_usec, 1000000)
    END
  END Mult;


PROCEDURE Cmp* (a, b : Time) : INTEGER;
  (* Compares 'a' to 'b'.  Result: Sign of 'b-a'. 
     -1: a>b;  0: a=b;  1: a<b *)
  BEGIN
    IF (a. tv_sec > b. tv_sec) OR
       (a. tv_sec = b. tv_sec) & (a. tv_usec > b. tv_usec) THEN
      RETURN -1
    ELSIF (a. tv_sec = b. tv_sec) & (a. tv_usec = b. tv_usec) THEN
      RETURN 0
    ELSE
      RETURN 1
    END
  END Cmp;


PROCEDURE GetSysTime* (VAR time : Time);
(* Read the current value of the system clock and store it in 'time'. *)
  VAR
    res : C.int;
  BEGIN
    res := Unix.gettimeofday (time, Unix.NULL)
  END GetSysTime;

PROCEDURE Since* (start : Time; VAR elapsed : Time);
(* Report in 'elapsed' the time that has passed since 'start'. *)
  VAR
    res : C.int;
    curr : Time;
  BEGIN
    res := Unix.gettimeofday (curr, Unix.NULL);
    Sub (start, curr, elapsed)
  END Since;
  
END Time.
