MODULE ConvTypes;

    (* result of a conversion operation *)
TYPE
  ConvResults* = SHORTINT;

CONST
    allRight* = 0;     (* data is as expected or as required *)
    outOfRange* = 1;   (* data cannot be represented         *)
    wrongFormat* = 2;  (* data not in expected format        *)
    noData* = 3;       (* no data or insufficient data       *)
    noRoom* = 4;       (* no room to store input data        *)
    (* note: the current string conversion functions don't 
       make use of all of the above codes. *)

    (* type and constants for alignment of text *)
TYPE
  Alignment* = SHORTINT;

CONST
  left* = 0;
  centre* = 1;
  right* = 2;

END ConvTypes.
