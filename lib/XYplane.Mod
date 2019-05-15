MODULE XYplane;
(*
Module XYplane provides some basic facilities for graphics programming. Its
interface is kept as simple as possible and is therefore more suited for
programming exercises than for serious graphics applications.

XYplane provides a Cartesian plane of pixels that can be drawn and erased. The
plane is mapped to some location on the screen. The variables X and Y indicate
its lower left corner, W its width and H its height. All variables are
read-only. 

Note: 
  If linking to the X11 lib fails, then try to add the command line 
  option --ldflags <lpath> to the o2c invocation; a likely value
  for <lpath> is "-L/usr/X11/lib" or "-L/usr/lib/X11".  
  For Solaris you have to add "-lsocket" to the linker flags, too.
*)

IMPORT (*      $Id: OGenGCC.Mod,v 1.54 1996/05/12 08:46:12 oberon1 Exp $     *)

  Out, C := CType, X11, Xu := Xutil, SYSTEM;


CONST 
  erase* = 0;
  draw* = 1;
  sizeSet = MAX (SET)+1;

VAR
  X-, Y-, W-, H-: INTEGER;

  display: X11.DisplayPtr;
  window: X11.Window;
  fg, bg: X11.GC;

  initialized: BOOLEAN;  (* first call to Open sets this to TRUE *)
  image: X11.XImagePtr;
  map: POINTER TO ARRAY OF ARRAY OF SET;
  

PROCEDURE Error (msg: ARRAY OF CHAR);
  BEGIN
    Out.String ("Error: ");
    Out.String (msg);
    Out.Ln;
    HALT (1)
  END Error;

PROCEDURE Clear*;
(* Erases all pixels in the drawing plane.  *)
  VAR  
    x, y: INTEGER;
  BEGIN
    X11.XFillRectangle (display, window, bg, 0, 0, W+1, H+1);
    FOR y := 0 TO SHORT (LEN (map^, 0))-1 DO
      FOR x := 0 TO SHORT (LEN (map^, 1))-1 DO
        map[y, x] := {}
      END
    END;
    X11.XFlush (display)
  END Clear;

PROCEDURE Dot* (x, y, mode: INTEGER);
(* Dot(x, y, m) draws or erases the pixel at the coordinates (x, y) relative to
   the lower left corner of the plane. If m=draw the pixel is drawn, if m=erase
   the pixel is erased.  *)
  VAR
    dummy: C.int;
  BEGIN
    IF (x >= X) & (x < X+W) & (y >= Y) & (y < Y+H) THEN
      dummy := image. f. put_pixel(image, x, H-1-y, mode);
      CASE mode OF
      | draw:
        X11.XDrawPoint (display, window, fg, x, H-1-y)
      | erase:
        X11.XDrawPoint (display, window, bg, x, H-1-y)
      END;
      X11.XFlush (display); 
    END
  END Dot;

PROCEDURE IsDot* (x, y: INTEGER): BOOLEAN;
(* IsDot(x, y) returns TRUE if the pixel at the coordinates (x, y) relative to
   the lower left corner of the plane is drawn, otherwise it returns FALSE. *)
  BEGIN
    IF (x < X) OR (x >= X+W) OR (y < Y) OR (y >= Y+H) THEN
      RETURN FALSE
    ELSE
      RETURN (image. f. get_pixel (image, x, H-1-y) # erase)
    END
  END IsDot;

PROCEDURE Key* (): CHAR;
(* Reads the keyboard. If a key was pressed prior to invocation, its
   character value is returned, otherwise the result is 0X.  *)
  CONST
    sizeBuffer = 16;
  VAR
    event: X11.XEvent;
    buffer: ARRAY sizeBuffer OF C.char;
    keySym: X11.KeySym;
    numChars: C.int;
    
  PROCEDURE Redraw (x0, y0, w0, h0: INTEGER);
    BEGIN
      (* clip width and height to size of initial window *)
      IF (x0+w0 > W) THEN
        w0 := W-x0
      END;
      IF (y0+h0 > H) THEN
        h0 := H-y0
      END;
      IF (w0 > 0) & (h0 > 0) THEN
        X11.XPutImage (display, window, fg, image, x0, y0, x0, y0, w0, h0)
      END
    END Redraw;
  
  BEGIN
    WHILE initialized & 
          (X11.XEventsQueued (display, X11.QueuedAfterReading) > 0) DO
      X11.XNextEvent (display, event);
      IF (event. type = X11.KeyPress) THEN  
        numChars := Xu.XLookupString (SYSTEM.ADR (event), buffer, sizeBuffer, 
                                      keySym, NIL);
        IF (numChars > 0) THEN
          RETURN SYSTEM.VAL (CHAR, buffer[0])
        END
      ELSIF (event. type = X11.Expose) THEN
        Redraw (SHORT (event. xexpose. x), SHORT (event. xexpose. y), 
                SHORT (event. xexpose. width), SHORT (event. xexpose. height))
      END
    END;
    RETURN 0X
  END Key;

PROCEDURE Open*;
(* Initializes the drawing plane.  *)
  VAR
    screen: C.int;
    parent: C.int;  
    bgColor, fgColor: C.int;
    gcValue: X11.XGCValues;
    event: X11.XEvent;
    x, y: INTEGER;
  BEGIN
    IF ~initialized THEN
      initialized := TRUE;
      
      display := X11.XOpenDisplay (NIL);
      IF (display = NIL) THEN
        Error ("Couldn't open display")
      ELSE
        screen := X11.XDefaultScreen (display);
        X := 0; Y := 0;
        W := SHORT (X11.XDisplayWidth (display, screen));
        H := SHORT (X11.XDisplayHeight (display, screen));
        (* adjust ratio W:H to 3:4 [for no paritcular reason] *)
        IF (W > 3*H DIV 4) THEN
          W := 3*H DIV 4
        END;
        parent := X11.XRootWindow (display, screen);
        fgColor := X11.XBlackPixel (display, screen);
        bgColor := X11.XWhitePixel (display, screen);
        window := X11.XCreateSimpleWindow (display, parent, 0, 0, 
                                           W, H, 0, 0, bgColor);  
        X11.XStoreName (display, window, "XYplane");
        X11.XSelectInput (display, window, X11.KeyPressMask+X11.ExposureMask);
        X11.XMapWindow (display, window);
        X11.XFlush (display);

        NEW (map, H, (W+(sizeSet-1)) DIV sizeSet);
        FOR y := 0 TO SHORT (LEN (map^, 0))-1 DO
          FOR x := 0 TO SHORT (LEN (map^, 1))-1 DO
            map[y, x] := {}
          END
        END;
        image := X11.XCreateImage (display, 
                  X11.XDefaultVisual (display, X11.XDefaultScreen (display)),
                  1, X11.XYBitmap, 0, SYSTEM.ADR (map^), W, H, sizeSet, 0);

        (* wait until the window manager gives its ok to draw things *)
        X11.XMaskEvent (display, X11.ExposureMask, event);

        (* create graphic context to draw resp. erase a point *)
        gcValue. foreground := fgColor;
        gcValue. background := bgColor;
        fg := X11.XCreateGC (display, parent, 
                             X11.GCForeground+X11.GCBackground, gcValue);
        gcValue. foreground := bgColor;
        gcValue. background := fgColor;
        bg := X11.XCreateGC (display, parent, 
                             X11.GCForeground+X11.GCBackground, gcValue)
      END
    END
  END Open;

BEGIN
  initialized := FALSE;
  X := 0; Y := 0; W := 0; H := 0;
  image := NIL; map := NIL
END XYplane.
