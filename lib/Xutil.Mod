MODULE Xutil ["C"] EXTERNAL [""];

IMPORT
  C := CType, X := X11;


(* 
 * Bitmask returned by XParseGeometry().  Each bit tells if the corresponding
 * value (x, y, width, height) was found in the parsed string.
 *)
CONST
  NoValue* = 00000H;
  XValue* = 00001H;
  YValue* = 00002H;
  WidthValue* = 00004H;
  HeightValue* = 00008H;
  AllValues* = 0000FH;
  XNegative* = 00010H;
  YNegative* = 00020H;
  
(*
 * new version containing base_width, base_height, and win_gravity fields;
 * used with WM_NORMAL_HINTS.
 *)
TYPE
  XSizeHintsPtr* = POINTER TO XSizeHints;
  XSizeHints* = RECORD
    flags*: X.ulongmask;        (* marks which fields in this structure are defined *)
    x*, y*: C.int;              (* obsolete for new window mgrs, but clients *)
    width*, height*: C.int;     (* should set so old wm's don't mess up *)
    min_width*, min_height*: C.int;
    max_width*, max_height*: C.int;
    width_inc*, height_inc*: C.int;
    min_aspect*, max_aspect*: RECORD
      x*: C.int;                (* numerator *)
      y*: C.int;                (* denominator *)
    END;
    base_width*, base_height*: C.int;(* added by ICCCM version 1 *)
    win_gravity*: C.int;        (* added by ICCCM version 1 *)
  END;
  
(*
 * The next block of definitions are for window manager properties that
 * clients and applications use for communication.
 *)
CONST
(* flags argument in size hints *)
  USPosition* = {0};            (* user specified x, y *)
  USSize* = {1};                (* user specified width, height *)
  PPosition* = {2};             (* program specified position *)
  PSize* = {3};                 (* program specified size *)
  PMinSize* = {4};              (* program specified minimum size *)
  PMaxSize* = {5};              (* program specified maximum size *)
  PResizeInc* = {6};            (* program specified resize increments *)
  PAspect* = {7};               (* program specified min and max aspect ratios *)
  PBaseSize* = {8};             (* program specified base for incrementing *)
  PWinGravity* = {9};           (* program specified window gravity *)
(* obsolete *)
  PAllHints* = PPosition+PSize+PMinSize+PMaxSize+PResizeInc+PAspect;

TYPE
  XWMHintsPtr* = POINTER TO XWMHints;
  XWMHints* = RECORD
    flags*: X.ulongmask;(* marks which fields in this structure are defined *)
    input*: X.Bool;   (* does this application rely on the window manager to
			get keyboard input? *)
    initial_state*: C.int;        (* see below *)
    icon_pixmap*: X.Pixmap;       (* pixmap to be used as icon *)
    icon_window*: X.Window;       (* window to be used as icon *)
    icon_x*, icon_y*: C.int;      (* initial position of icon *)
    icon_mask*: X.Pixmap;         (* icon mask bitmap *)
    window_group*: X.XID;         (* id of related window group *)
  END;

CONST
(* definition for flags of XWMHints *)
  InputHint* = {0};
  StateHint* = {1};
  IconPixmapHint* = {2};
  IconWindowHint* = {3};
  IconPositionHint* = {4};
  IconMaskHint* = {5};
  WindowGroupHint* = {6};
  AllHints* = InputHint+StateHint+IconPixmapHint+IconWindowHint+IconPositionHint+IconMaskHint+WindowGroupHint;
  XUrgencyHint* = {8};
(* definitions for initial window state *)
  WithdrawnState* = 0;          (* for windows that are not mapped *)
  NormalState* = 1;             (* most applications want to start this way *)
  IconicState* = 3;             (* application wants to start as an icon *)
  
(*
 * Obsolete states no longer defined by ICCCM
 *)
CONST
  DontCareState* = 0;           (* don't know or care *)
  ZoomState* = 2;               (* application wants to start zoomed *)
  InactiveState* = 4;           (* application believes it is seldom used; *)
                                (* some wm's may put it on inactive menu *)
(*
 * new structure for manipulating TEXT properties; used with WM_NAME, 
 * WM_ICON_NAME, WM_CLIENT_MACHINE, and WM_COMMAND.
 *)
TYPE
  XTextPropertyPtr* = POINTER TO XTextProperty;
  XTextProperty* = RECORD
    value*: C.charPtr1d;        (* same as Property routines *)
    encoding*: X.Atom;          (* prop type *)
    format*: C.int;             (* prop data format: 8, 16, or 32 *)
    nitems*: C.longint;         (* number of data items in value *)
  END;

CONST
  XNoMemory* = 1;
  XLocaleNotSupported* = 2;
  XConverterNotFound* = 3;
  
CONST  (* enum XICCEncodingStyle *)
  XStringStyle* = 0;
  XCompoundTextStyle* = 1;
  XTextStyle* = 2;
  XStdICCTextStyle* = 3;

TYPE
  XICCEncodingStyle* = C.enum1;
  XIconSizePtr* = POINTER TO XIconSize;
  XIconSize* = RECORD
    min_width*, min_height*: C.int;
    max_width*, max_height*: C.int;
    width_inc*, height_inc*: C.int;
  END;
  XClassHintPtr* = POINTER TO XClassHint;
  XClassHint* = RECORD
    res_name*: C.charPtr1d;
    res_class*: C.charPtr1d;
  END;
  
(*
 * These macros are used to give some sugar to the image routines so that
 * naive people are more comfortable with them.
 *)
(* can't define any macros here *)

(*
 * Compose sequence status structure, used in calling XLookupString.
 *)
TYPE
  XComposeStatusPtr* = POINTER TO XComposeStatus;
  XComposeStatus* = RECORD
    compose_ptr*: X.XPointer;     (* state table pointer *)
    chars_matched*: C.int;      (* match state *)
  END;
  
(*
 * Keysym macros, used on Keysyms to test for classes of symbols
 *)
(* can't define any macros here *)

(*
 * opaque reference to Region data type 
 *)
TYPE
  XRegion* = RECORD END;
  Region* = POINTER TO XRegion;
  
(* Return values from XRectInRegion() *)
CONST
  RectangleOut* = 0;
  RectangleIn* = 1;
  RectanglePart* = 2;
  
(*
 * Information used by the visual utility routines to find desired visual
 * type from the many visuals a display may support.
 *)
TYPE
  XVisualInfoPtr* = POINTER TO XVisualInfo;
  XVisualInfo* = RECORD
    visual*: X.VisualPtr;
    visualid*: X.VisualID;
    screen*: C.int;
    depth*: C.int;
    class*: C.int;
    red_mask*: X.ulongmask;
    green_mask*: X.ulongmask;
    blue_mask*: X.ulongmask;
    colormap_size*: C.int;
    bits_per_rgb*: C.int;
  END;

CONST
  VisualNoMask* = 00H;
  VisualIDMask* = 01H;
  VisualScreenMask* = 02H;
  VisualDepthMask* = 04H;
  VisualClassMask* = 08H;
  VisualRedMaskMask* = 010H;
  VisualGreenMaskMask* = 020H;
  VisualBlueMaskMask* = 040H;
  VisualColormapSizeMask* = 080H;
  VisualBitsPerRGBMask* = 0100H;
  VisualAllMask* = 01FFH;
  
(*
 * This defines a window manager property that clients may use to
 * share standard color maps of type RGB_COLOR_MAP:
 *)
TYPE
  XStandardColormapPtr* = POINTER TO XStandardColormap;
  XStandardColormap* = RECORD
    colormap*: X.Colormap;
    red_max*: C.longint;
    red_mult*: C.longint;
    green_max*: C.longint;
    green_mult*: C.longint;
    blue_max*: C.longint;
    blue_mult*: C.longint;
    base_pixel*: C.longint;
    visualid*: X.VisualID;        (* added by ICCCM version 1 *)
    killid*: X.XID;               (* added by ICCCM version 1 *)
  END;

CONST
  ReleaseByFreeingColormap* = 1;(* for killid field above *)
  
(*
 * return codes for XReadBitmapFile and XWriteBitmapFile
 *)
CONST
  BitmapSuccess* = 0;
  BitmapOpenFailed* = 1;
  BitmapFileInvalid* = 2;
  BitmapNoMemory* = 3;
  
  
(****************************************************************
 *
 * Context Management
 *
 ****************************************************************)
(* Associative lookup table return codes *)
CONST
  XCSUCCESS* = 0;               (* No error. *)
  XCNOMEM* = 1;                 (* Out of memory *)
  XCNOENT* = 2;                 (* No entry in table *)

TYPE
  XContext* = C.int;
  
  
(* The following declarations are alphabetized. *)
PROCEDURE XAllocClassHint* (): XClassHintPtr;
PROCEDURE XAllocIconSize* (): XIconSizePtr;
PROCEDURE XAllocSizeHints* (): XSizeHintsPtr;
PROCEDURE XAllocStandardColormap* (): XStandardColormapPtr;
PROCEDURE XAllocWMHints* (): XWMHintsPtr;
PROCEDURE XClipBox* (
    r: Region;
    VAR rect_return: X.XRectangle);
PROCEDURE XCreateRegion* (): Region;
PROCEDURE XDefaultString* (): C.charPtr1d;
PROCEDURE XDeleteContext* (
    display: X.DisplayPtr;
    rid: X.XID;
    context: XContext): C.int;
PROCEDURE XDestroyRegion* (
    r: Region);
PROCEDURE XEmptyRegion* (
    r: Region);
PROCEDURE XEqualRegion* (
    r1: Region;
    r2: Region);
PROCEDURE XFindContext* (
    display: X.DisplayPtr;
    rid: X.XID;
    context: XContext;
    VAR data_return: X.XPointer): C.int;
PROCEDURE XGetClassHint* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR class_hints_return: XClassHint): X.Status;
PROCEDURE XGetIconSizes* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR size_list_return: XIconSize;
    VAR count_return: C.int): X.Status;
PROCEDURE XGetNormalHints* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR hints_return: XSizeHints): X.Status;
PROCEDURE XGetRGBColormaps* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR stdcmap_return: XStandardColormap;
    VAR count_return: C.int;
    property: X.Atom): X.Status;
PROCEDURE XGetSizeHints* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR hints_return: XSizeHints;
    property: X.Atom): X.Status;
PROCEDURE XGetStandardColormap* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR colormap_return: XStandardColormap;
    property: X.Atom): X.Status;
PROCEDURE XGetTextProperty* (
    display: X.DisplayPtr;
    window: X.Window;
    VAR text_prop_return: XTextProperty;
    property: X.Atom): X.Status;
PROCEDURE XGetVisualInfo* (
    display: X.DisplayPtr;
    vinfo_mask: X.ulongmask;
    vinfo_template: XVisualInfoPtr;
    VAR nitems_return: C.int): XVisualInfoPtr;
PROCEDURE XGetWMClientMachine* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR text_prop_return: XTextProperty): X.Status;
PROCEDURE XGetWMHints* (
    display: X.DisplayPtr;
    w: X.Window): XWMHintsPtr;
PROCEDURE XGetWMIconName* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR text_prop_return: XTextProperty): X.Status;
PROCEDURE XGetWMName* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR text_prop_return: XTextProperty): X.Status;
PROCEDURE XGetWMNormalHints* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR hints_return: XSizeHints;
    VAR supplied_return: C.longint): X.Status;
PROCEDURE XGetWMSizeHints* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR hints_return: XSizeHints;
    VAR supplied_return: C.longint;
    property: X.Atom): X.Status;
PROCEDURE XGetZoomHints* (
    display: X.DisplayPtr;
    w: X.Window;
    VAR zhints_return: XSizeHints): X.Status;
PROCEDURE XIntersectRegion* (
    sra, srb, dr_return: Region);  (* ??? *)
PROCEDURE XConvertCase* (
    sym: X.KeySym;
    VAR lower: X.KeySym;
    VAR upper: X.KeySym);
PROCEDURE XLookupString* (
    event_struct: X.XKeyEventPtr;
    VAR buffer_return: ARRAY OF C.char;
    bytes_buffer: C.int;
    VAR keysym_return: X.KeySym;
    status_in_out: XComposeStatusPtr): C.int;
PROCEDURE XMatchVisualInfo* (
    display: X.DisplayPtr;
    screen: C.int;
    depth: C.int;
    class: C.int;
    VAR vinfo_return: XVisualInfo): X.Status;
PROCEDURE XOffsetRegion* (
    r: Region;
    dx: C.int;
    dy: C.int);
PROCEDURE XPointInRegion* (
    r: Region;
    x: C.int;
    y: C.int): X.Bool;
PROCEDURE XPolygonRegion* (
    points: ARRAY OF X.XPoint;
    n: C.int;
    fill_rule: C.int): Region;
PROCEDURE XRectInRegion* (
    r: Region;
    x: C.int;
    y: C.int;
    width: C.int;
    height: C.int): C.int;
PROCEDURE XSaveContext* (
    display: X.DisplayPtr;
    rid: X.XID;
    context: XContext;
    data: ARRAY OF C.char): C.int;
PROCEDURE XSetClassHint* (
    display: X.DisplayPtr;
    w: X.Window;
    class_hints: XClassHintPtr);
PROCEDURE XSetIconSizes* (
    display: X.DisplayPtr;
    w: X.Window;
    size_list: XIconSizePtr;
    count: C.int);
PROCEDURE XSetNormalHints* (
    display: X.DisplayPtr;
    w: X.Window;
    hints: XSizeHintsPtr);
PROCEDURE XSetRGBColormaps* (
    display: X.DisplayPtr;
    w: X.Window;
    stdcmaps: XStandardColormapPtr;
    count: C.int;
    property: X.Atom);
PROCEDURE XSetSizeHints* (
    display: X.DisplayPtr;
    w: X.Window;
    hints: XSizeHintsPtr;
    property: X.Atom);
PROCEDURE XSetStandardProperties* (
    display: X.DisplayPtr;
    w: X.Window;
    window_name: ARRAY OF C.char;
    icon_name: ARRAY OF C.char;
    icon_pixmap: X.Pixmap;
    argv: C.charPtr2d;
    argc: C.int;
    hints: XSizeHintsPtr);
PROCEDURE XSetTextProperty* (
    display: X.DisplayPtr;
    w: X.Window;
    text_prop: XTextPropertyPtr;
    property: X.Atom);
PROCEDURE XSetWMClientMachine* (
    display: X.DisplayPtr;
    w: X.Window;
    text_prop: XTextPropertyPtr);
PROCEDURE XSetWMHints* (
    display: X.DisplayPtr;
    w: X.Window;
    wm_hints: XWMHintsPtr);
PROCEDURE XSetWMIconName* (
    display: X.DisplayPtr;
    w: X.Window;
    text_prop: XTextPropertyPtr);
PROCEDURE XSetWMName* (
    display: X.DisplayPtr;
    w: X.Window;
    text_prop: XTextPropertyPtr);
PROCEDURE XSetWMNormalHints* (
    display: X.DisplayPtr;
    w: X.Window;
    hints: XSizeHintsPtr);
PROCEDURE XSetWMProperties* (
    display: X.DisplayPtr;
    w: X.Window;
    window_name: XTextPropertyPtr;
    icon_name: XTextPropertyPtr;
    argv: C.charPtr2d;
    argc: C.int;
    normal_hints: XSizeHintsPtr;
    wm_hints: XWMHintsPtr;
    class_hints: XClassHintPtr);
PROCEDURE XmbSetWMProperties* (
    display: X.DisplayPtr;
    w: X.Window;
    window_name: ARRAY OF C.char;
    icon_name: ARRAY OF C.char;
    argv: C.charPtr2d;
    argc: C.int;
    normal_hints: XSizeHintsPtr;
    wm_hints: XWMHintsPtr;
    class_hints: XClassHintPtr);
PROCEDURE XSetWMSizeHints* (
    display: X.DisplayPtr;
    w: X.Window;
    hints: XSizeHintsPtr;
    property: X.Atom);
PROCEDURE XSetRegion* (
    display: X.DisplayPtr;
    gc: X.GC;
    r: Region);
PROCEDURE XSetStandardColormap* (
    display: X.DisplayPtr;
    w: X.Window;
    colormap: XStandardColormapPtr;
    property: X.Atom);
PROCEDURE XSetZoomHints* (
    display: X.DisplayPtr;
    w: X.Window;
    zhints: XSizeHintsPtr);
PROCEDURE XShrinkRegion* (
    r: Region;
    dx: C.int;
    dy: C.int);
PROCEDURE XStringListToTextProperty* (
    list: C.charPtr2d;
    count: C.int;
    VAR text_prop_return: XTextProperty): X.Status;
PROCEDURE XSubtractRegion* (
    sra, srb, dr_return: Region);  (* ??? *)
PROCEDURE XmbTextListToTextProperty* (
    display: X.DisplayPtr;
    list: C.charPtr2d;
    count: C.int;
    style: XICCEncodingStyle;
    VAR text_prop_return: XTextProperty): C.int;
PROCEDURE XwcTextListToTextProperty* (
    display: X.DisplayPtr;
    list: ARRAY OF X.wchar_t;
    count: C.int;
    style: XICCEncodingStyle;
    VAR text_prop_return: XTextProperty): C.int;
PROCEDURE XwcFreeStringList* (
    list: X.wcharPtr2d);
PROCEDURE XTextPropertyToStringList* (
    text_prop: XTextPropertyPtr;
    VAR list_return: C.charPtr2d;
    VAR count_return: C.int): X.Status;
PROCEDURE XTextPropertyToTextList* (
    display: X.DisplayPtr;
    text_prop: XTextPropertyPtr;
    VAR list_return: C.charPtr2d;
    VAR count_return: C.int): X.Status;
PROCEDURE XwcTextPropertyToTextList* (
    display: X.DisplayPtr;
    text_prop: XTextPropertyPtr;
    VAR list_return: X.wcharPtr2d;
    VAR count_return: C.int): X.Status;
PROCEDURE XUnionRectWithRegion* (
    rectangle: X.XRectanglePtr;
    src_region: Region;
    dest_region_return: Region);  (* ??? *)
PROCEDURE XUnionRegion* (
    sra, srb, dr_return: Region);  (* ??? *)
PROCEDURE XWMGeometry* (
    display: X.DisplayPtr;
    screen_number: C.int;
    user_geometry: ARRAY OF C.char;
    default_geometry: ARRAY OF C.char;
    border_width: C.int;
    hints: XSizeHintsPtr;
    VAR x_return: C.int;
    VAR y_return: C.int;
    VAR width_return: C.int;
    VAR height_return: C.int;
    VAR gravity_return: C.int): C.int;
PROCEDURE XXorRegion* (
    sra, srb, dr_return: Region);  (* ??? *)

END Xutil.
