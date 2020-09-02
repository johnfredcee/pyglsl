#include "StdAfx.h"

// ripped mostly from wxWidgets samples

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------


#include "CodeEditorDefaults.h"
#include "CodeEditorPreferences.h"
#include "CodeEditor.h"


//============================================================================
// declarations
//============================================================================

//----------------------------------------------------------------------------
//! style bits types
#define mySTC_STYLE_BOLD 1
#define mySTC_STYLE_ITALIC 2
#define mySTC_STYLE_UNDERL 4
#define mySTC_STYLE_HIDDEN 8

//----------------------------------------------------------------------------
//! language types
const CommonInfo g_CommonPrefs = {
    // editor functionality prefs
    true,  // syntaxEnable
    true,  // foldEnable
    true,  // indentEnable
    // display defaults prefs
    false, // overTypeInitial
    false, // readOnlyInitial
    false,  // wrapModeInitial
    false, // displayEOLEnable
    false, // IndentGuideEnable
    true,  // lineNumberEnable
    false, // longLineOnEnable
    false, // whiteSpaceEnable
};

//----------------------------------------------------------------------------
// keywordlists
// C++
const char* CppWordlist1 =
    "asm auto bool break case catch char class const const_cast "
    "continue default delete do double dynamic_cast else enum explicit "
    "export extern false float for friend goto if inline int long "
    "mutable namespace new operator private protected public register "
    "reinterpret_cast return short signed sizeof static static_cast "
    "struct switch template this throw true try typedef typeid "
    "typename union unsigned using virtual void volatile wchar_t "
    "while";
const char* CppWordlist2 =
    "file";
const char* CppWordlist3 =
    "a addindex addtogroup anchor arg attention author b brief bug c "
    "class code date def defgroup deprecated dontinclude e em endcode "
    "endhtmlonly endif endlatexonly endlink endverbatim enum example "
    "exception f$ f[ f] file fn hideinitializer htmlinclude "
    "htmlonly if image include ingroup internal invariant interface "
    "latexonly li line link mainpage name namespace nosubgrouping note "
    "overload p page par param post pre ref relates remarks return "
    "retval sa section see showinitializer since skip skipline struct "
    "subsection test throw todo typedef union until var verbatim "
    "verbinclude version warning weakgroup $ @ \"\" & < > # { }";

// Python
const char* PythonWordlist1 =
    "and assert break class continue def del elif else except exec "
    "finally for from global if import in is lambda None not or pass "
    "print raise return try while yield";
const char* PythonWordlist2 =
    "ACCELERATORS ALT AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON BEGIN "
    "BITMAP BLOCK BUTTON CAPTION CHARACTERISTICS CHECKBOX CLASS "
    "COMBOBOX CONTROL CTEXT CURSOR DEFPUSHBUTTON DIALOG DIALOGEX "
    "DISCARDABLE EDITTEXT END EXSTYLE FONT GROUPBOX ICON LANGUAGE "
    "LISTBOX LTEXT MENU MENUEX MENUITEM MESSAGETABLE POPUP PUSHBUTTON "
    "RADIOBUTTON RCDATA RTEXT SCROLLBAR SEPARATOR SHIFT STATE3 "
    "STRINGTABLE STYLE TEXTINCLUDE VALUE VERSION VERSIONINFO VIRTKEY";

const char* LuaWordlist1 = "and begin break do else elseif end false for function if in local nil not or repeat return then true until while G getfenv getmetatable ipairs loadlib next pairs pcall rawequal setfenv setmetatable xpcall string table math coroutine io os debug load module select";

const char* LuaWordlist2 = "string.byte string.char string.dump string.find string.len string.lower string.rep string.sub string.upper string.format string.gfind string.gsub table.concat table.foreach table.foreachi table.getn table.sort table.insert table.remove table.setn math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.deg math.exp math.floor math.frexp math.ldexp math.log math.log10 math.max math.min math.mod math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan string.gmatch string.match string.reverse table.maxn math.cosh math.fmod math.modf math.sinh math.tanh math.huge";
	
//----------------------------------------------------------------------------
//! languages
const LanguageInfo g_LanguagePrefs [] = {
    // C++
    {"C++",
     "*.c;*.cc;*.cpp;*.cxx;*.cs;*.h;*.hh;*.hpp;*.hxx;*.sma",
     wxSTC_LEX_CPP,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_COMMENT, NULL},
      {mySTC_TYPE_COMMENT_LINE, NULL},
      {mySTC_TYPE_COMMENT_DOC, NULL},
      {mySTC_TYPE_NUMBER, NULL},
      {mySTC_TYPE_WORD1, CppWordlist1}, // KEYWORDS
      {mySTC_TYPE_STRING, NULL},
      {mySTC_TYPE_CHARACTER, NULL},
      {mySTC_TYPE_UUID, NULL},
      {mySTC_TYPE_PREPROCESSOR, NULL},
      {mySTC_TYPE_OPERATOR, NULL},
      {mySTC_TYPE_IDENTIFIER, NULL},
      {mySTC_TYPE_STRING_EOL, NULL},
      {mySTC_TYPE_DEFAULT, NULL}, // VERBATIM
      {mySTC_TYPE_REGEX, NULL},
      {mySTC_TYPE_COMMENT_SPECIAL, NULL}, // DOXY
      {mySTC_TYPE_WORD2, CppWordlist2}, // EXTRA WORDS
      {mySTC_TYPE_WORD3, CppWordlist3}, // DOXY KEYWORDS
      {mySTC_TYPE_ERROR, NULL}, // KEYWORDS ERROR
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     mySTC_FOLD_COMMENT | mySTC_FOLD_COMPACT | mySTC_FOLD_PREPROC},
    // Python
    {"Python",
     "*.py;*.pyw",
     wxSTC_LEX_PYTHON,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_COMMENT_LINE, NULL},
      {mySTC_TYPE_NUMBER, NULL},
      {mySTC_TYPE_STRING, NULL},
      {mySTC_TYPE_CHARACTER, NULL},
      {mySTC_TYPE_WORD1, PythonWordlist1}, // KEYWORDS
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLE
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLEDOUBLE
      {mySTC_TYPE_DEFAULT, NULL}, // CLASSNAME
      {mySTC_TYPE_DEFAULT, PythonWordlist2}, // DEFNAME
      {mySTC_TYPE_OPERATOR, NULL},
      {mySTC_TYPE_IDENTIFIER, NULL},
      {mySTC_TYPE_DEFAULT, NULL}, // COMMENT_BLOCK
      {mySTC_TYPE_STRING_EOL, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     mySTC_FOLD_COMMENTPY | mySTC_FOLD_QUOTESPY},
	{"Lua",
	 "*.lua",
	 wxSTC_LEX_LUA,
	 {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_COMMENT_LINE, NULL},
      {mySTC_TYPE_NUMBER, NULL},
      {mySTC_TYPE_STRING, NULL},
      {mySTC_TYPE_CHARACTER, NULL},
      {mySTC_TYPE_WORD1, LuaWordlist1}, // KEYWORDS
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLE
      {mySTC_TYPE_DEFAULT, NULL}, // TRIPLEDOUBLE
      {mySTC_TYPE_DEFAULT, NULL}, // CLASSNAME
      {mySTC_TYPE_WORD2, LuaWordlist2}, // DEFNAME
      {mySTC_TYPE_OPERATOR, NULL},
      {mySTC_TYPE_IDENTIFIER, NULL},
      {mySTC_TYPE_DEFAULT, NULL}, // COMMENT_BLOCK
      {mySTC_TYPE_STRING_EOL, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     mySTC_FOLD_COMMENT | mySTC_FOLD_COMPACT | mySTC_FOLD_PREPROC},	 
    // * (any)
    {wxTRANSLATE(DEFAULT_LANGUAGE),
     "*.*",
     wxSTC_LEX_PROPERTIES,
     {{mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {mySTC_TYPE_DEFAULT, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL},
      {-1, NULL}},
     0},
    };

const int g_LanguagePrefsSize = WXSIZEOF(g_LanguagePrefs);

//----------------------------------------------------------------------------
//! style types
const StyleInfo g_StylePrefs [] = {
    // mySTC_TYPE_DEFAULT
    {_T("Default"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD1
    {_T("Keyword1"),
     _T("BLUE"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_WORD2
    {_T("Keyword2"),
     _T("DARK BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD3
    {_T("Keyword3"),
     _T("CORNFLOWER BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD4
    {_T("Keyword4"),
     _T("CYAN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD5
    {_T("Keyword5"),
     _T("DARK GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_WORD6
    {_T("Keyword6"),
     _T("GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT
    {_T("Comment"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_DOC
    {_T("Comment (Doc)"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_LINE
    {_T("Comment line"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMENT_SPECIAL
    {_T("Special comment"),
     _T("FOREST GREEN"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_CHARACTER
    {_T("Character"),
     _T("KHAKI"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_CHARACTER_EOL
    {_T("Character (EOL)"),
     _T("KHAKI"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_STRING
    {_T("String"),
     _T("BROWN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_STRING_EOL
    {_T("String (EOL)"),
     _T("BROWN"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_DELIMITER
    {_T("Delimiter"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_PUNCTUATION
    {_T("Punctuation"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_OPERATOR
    {_T("Operator"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_BOLD, 0},

    // mySTC_TYPE_BRACE
    {_T("Label"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_COMMAND
    {_T("Command"),
     _T("BLUE"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_IDENTIFIER
    {_T("Identifier"),
     _T("BLACK"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_LABEL
    {_T("Label"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_NUMBER
    {_T("Number"),
     _T("SIENNA"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_PARAMETER
    {_T("Parameter"),
     _T("VIOLET"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_REGEX
    {_T("Regular expression"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_UUID
    {_T("UUID"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_VALUE
    {_T("Value"),
     _T("ORCHID"), _T("WHITE"),
     _T(""), 10, mySTC_STYLE_ITALIC, 0},

    // mySTC_TYPE_PREPROCESSOR
    {_T("Preprocessor"),
     _T("GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_SCRIPT
    {_T("Script"),
     _T("DARK GREY"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_ERROR
    {_T("Error"),
     _T("RED"), _T("WHITE"),
     _T(""), 10, 0, 0},

    // mySTC_TYPE_UNDEFINED
    {_T("Undefined"),
     _T("ORANGE"), _T("WHITE"),
     _T(""), 10, 0, 0}

    };

const int g_StylePrefsSize = WXSIZEOF(g_StylePrefs);
