
#include "StdAfx.h"

#include "CodeEditorDefaults.h"
#include "CodeEditorPreferences.h"
#include "CodeEditor.h"

wxWindowID ID_INDENTINC = wxNewId();
wxWindowID ID_INDENTRED = wxNewId();
wxWindowID ID_SELECTLINE = wxNewId();
wxWindowID ID_FINDNEXT = wxNewId();
wxWindowID ID_REPLACE = wxNewId();
wxWindowID ID_REPLACENEXT = wxNewId();
wxWindowID ID_BRACEMATCH = wxNewId();
wxWindowID ID_GOTO = wxNewId();
wxWindowID ID_HILIGHTFIRST = wxNewId();
wxWindowID ID_HILIGHTLAST = wxNewId();
wxWindowID ID_DISPLAYEOL = wxNewId();
wxWindowID ID_INDENTGUIDE = wxNewId();
wxWindowID ID_LINENUMBER = wxNewId();
wxWindowID ID_LONGLINEON = wxNewId();
wxWindowID ID_WHITESPACE = wxNewId();
wxWindowID ID_FOLDTOGGLE = wxNewId();
wxWindowID ID_OVERTYPE = wxNewId();
wxWindowID ID_READONLY = wxNewId();
wxWindowID ID_WRAPMODEON = wxNewId();
wxWindowID ID_CHARSETANSI = wxNewId();
wxWindowID ID_CHARSETMAC = wxNewId();
wxWindowID ID_CHANGELOWER = wxNewId();
wxWindowID ID_CHANGEUPPER = wxNewId();
wxWindowID ID_CONVERTCR = wxNewId();
wxWindowID ID_CONVERTCRLF = wxNewId();
wxWindowID ID_CONVERTLF = wxNewId();

BEGIN_EVENT_TABLE(CodeEditor, wxStyledTextCtrl)
EVT_SIZE (CodeEditor::OnSize)
// Edit
EVT_MENU (wxID_CLEAR,		CodeEditor::OnEditClear)
EVT_MENU (wxID_CUT,			CodeEditor::OnEditCut)
EVT_MENU (wxID_COPY,		CodeEditor::OnEditCopy)
EVT_MENU (wxID_PASTE,		CodeEditor::OnEditPaste)
EVT_MENU (ID_INDENTINC,		CodeEditor::OnEditIndentInc)
EVT_MENU (ID_INDENTRED,		CodeEditor::OnEditIndentRed)
EVT_MENU (wxID_SELECTALL,	CodeEditor::OnEditSelectAll)
EVT_MENU (ID_SELECTLINE,	CodeEditor::OnEditSelectLine)
EVT_MENU (wxID_REDO,		CodeEditor::OnEditRedo)
EVT_MENU (wxID_UNDO,		CodeEditor::OnEditUndo)
// Find
EVT_MENU (wxID_FIND,		CodeEditor::OnFind)
EVT_MENU (ID_FINDNEXT,		CodeEditor::OnFindNext)
EVT_MENU (ID_REPLACE,		CodeEditor::OnReplace)
EVT_MENU (ID_REPLACENEXT,	CodeEditor::OnReplaceNext)
EVT_MENU (ID_BRACEMATCH,	CodeEditor::OnBraceMatch)
EVT_MENU (ID_GOTO,			CodeEditor::OnGoto)
// View
EVT_MENU_RANGE (ID_HILIGHTFIRST, ID_HILIGHTLAST, CodeEditor::OnHilightLang)
EVT_MENU (ID_DISPLAYEOL,	CodeEditor::OnDisplayEOL)
EVT_MENU (ID_INDENTGUIDE,	CodeEditor::OnIndentGuide)
EVT_MENU (ID_LINENUMBER,	CodeEditor::OnLineNumber)
EVT_MENU (ID_LONGLINEON,	CodeEditor::OnLongLineOn)
EVT_MENU (ID_WHITESPACE,	CodeEditor::OnWhiteSpace)
EVT_MENU (ID_FOLDTOGGLE,	CodeEditor::OnFoldToggle)
EVT_MENU (ID_OVERTYPE,		CodeEditor::OnSetOverType)
EVT_MENU (ID_READONLY,		CodeEditor::OnSetReadOnly)
EVT_MENU (ID_WRAPMODEON,	CodeEditor::OnWrapModeOn)
EVT_MENU (ID_CHARSETANSI,	CodeEditor::OnUseCharset)
EVT_MENU (ID_CHARSETMAC,	CodeEditor::OnUseCharset)
// Extra
EVT_MENU (ID_CHANGELOWER,	CodeEditor::OnChangeCase)
EVT_MENU (ID_CHANGEUPPER,	CodeEditor::OnChangeCase)
EVT_MENU (ID_CONVERTCR,		CodeEditor::OnConvertEOL)
EVT_MENU (ID_CONVERTCRLF,	CodeEditor::OnConvertEOL)
EVT_MENU (ID_CONVERTLF,		CodeEditor::OnConvertEOL)
// scintilla
EVT_STC_MARGINCLICK (-1, CodeEditor::OnMarginClick)
EVT_STC_CHARADDED (-1,   CodeEditor::OnCharAdded)
EVT_STC_UPDATEUI(-1, CodeEditor::OnUpdateUI)

END_EVENT_TABLE()

CodeEditor::CodeEditor(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
	: wxStyledTextCtrl (parent, id, pos, size, style)
{
	mFileName = _T("");
	mlanguage = NULL;

	mLineNrID = 0;
	mLineNrMargin = TextWidth(wxSTC_STYLE_LINENUMBER, _T("99999"));
	mFoldingID = 1;
	mFoldingMargin = 16;
	mDividerID = 1;

	SetProperty(wxT("fold"), wxT("1"));
	SetFoldFlags(16);
	SetMarginType(mFoldingID, wxSTC_MARGIN_SYMBOL);
	SetMarginMask(mFoldingID, wxSTC_MASK_FOLDERS);
	SetMarginSensitive(mFoldingID, true);
	SetMarginWidth(mFoldingID, mFoldingMargin);

	MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDERSUB, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDERSUB, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(0x80, 0x80, 0x80));
	MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);
	MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, wxColour(0xff, 0xff, 0xff));
	MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, wxColour(0x80, 0x80, 0x80));

	// Set defaults, these should eventually be set via user prefs
	SetViewEOL(false);
	SetIndentationGuides(false);
	SetMarginWidth(mLineNrID, mLineNrMargin);
	//SetMarginWidth(mFoldingID, mFoldingMargin);
	//SetMarginSensitive(mFoldingID, true);
	SetEdgeMode(wxSTC_EDGE_LINE);
	//SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
	SetOvertype(false);
	SetReadOnly(false);
	SetWrapMode(wxSTC_WRAP_NONE);

	wxFont font(10, wxTELETYPE, wxNORMAL, wxNORMAL);
	StyleSetFont(wxSTC_STYLE_DEFAULT, font);

    StyleSetForeground (wxSTC_STYLE_DEFAULT, *wxBLACK);
    StyleSetBackground (wxSTC_STYLE_DEFAULT, *wxWHITE);
    StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

	StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);
	
	InitializePrefs("Lua");
	SetTabWidth(4);
	SetUseTabs(false);
	SetTabIndents(true);
	SetBackSpaceUnIndents(true);
	SetIndent(4);

	// Set visibility
	SetVisiblePolicy(wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP, 1);
	SetXCaretPolicy(wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1);
	SetYCaretPolicy(wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1);

	SetCaretLineVisible(true);
	SetCaretLineBackground(wxColour(225, 235, 224));

	// Markers
	MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(_T("BLACK")));
	MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(_T("WHITE")));
	MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(_T("BLACK")));
	MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(_T("WHITE")));
	MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
	MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_SHORTARROW);
	MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
	MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
	MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);

	// Clear wrong default keys
#if !defined(__WXGTK__)
	//CmdKeyClear(wxSTC_KEY_TAB, 0);
	CmdKeyClear(wxSTC_KEY_TAB, wxSTC_SCMOD_SHIFT);
#endif
	CmdKeyClear('A', wxSTC_SCMOD_CTRL);
#if !defined(__WXGTK__)
	CmdKeyClear('C', wxSTC_SCMOD_CTRL);
#endif
	CmdKeyClear('D', wxSTC_SCMOD_CTRL);
	CmdKeyClear('D', wxSTC_SCMOD_SHIFT | wxSTC_SCMOD_CTRL);
	CmdKeyClear('F', wxSTC_SCMOD_ALT | wxSTC_SCMOD_CTRL);
	CmdKeyClear('L', wxSTC_SCMOD_CTRL);
	CmdKeyClear('L', wxSTC_SCMOD_SHIFT | wxSTC_SCMOD_CTRL);
	CmdKeyClear('T', wxSTC_SCMOD_CTRL);
	CmdKeyClear('T', wxSTC_SCMOD_SHIFT | wxSTC_SCMOD_CTRL);
	CmdKeyClear('U', wxSTC_SCMOD_CTRL);
	CmdKeyClear('U', wxSTC_SCMOD_SHIFT | wxSTC_SCMOD_CTRL);
#if !defined(__WXGTK__)
	CmdKeyClear('V', wxSTC_SCMOD_CTRL);
	CmdKeyClear('X', wxSTC_SCMOD_CTRL);
#endif
	CmdKeyClear('Y', wxSTC_SCMOD_CTRL);
#if !defined(__WXGTK__)
	CmdKeyClear('Z', wxSTC_SCMOD_CTRL);
#endif

	UsePopUp(0);
	SetLayoutCache(wxSTC_CACHE_PAGE);
	SetBufferedDraw(1);
}

CodeEditor::~CodeEditor() 
{

}

wxChar CodeEditor::GetLastNonWhitespaceChar(int position /* = -1 */)
{
	if (position == -1)
		position = GetCurrentPos();

	int count = 0; // Used to count the number of blank lines
	bool foundlf = false; // For the rare case of CR's without LF's
	while (position)
	{
		wxChar c = wxChar(GetCharAt(--position));
		int style = GetStyleAt(position);
		bool inComment = style == wxSTC_C_COMMENT ||
			style == wxSTC_C_COMMENTDOC ||
			style == wxSTC_C_COMMENTDOCKEYWORD ||
			style == wxSTC_C_COMMENTDOCKEYWORDERROR ||
			style == wxSTC_C_COMMENTLINE ||
			style == wxSTC_C_COMMENTLINEDOC;
		if (c == wxT('\n'))
		{
			count++;
			foundlf = true;
		}
		else if (c == wxT('\r') && !foundlf)
			count++;
		else
			foundlf = false;
		if (count > 1) return 0; // Don't over-indent
		if (!inComment && c != wxT(' ') && c != wxT('\t') && c != wxT('\n') && c != wxT('\r'))
			return c;
	}

	return 0;
}

wxString CodeEditor::GetLineIndentString(int line)
{
	int currLine = (line == -1) ? LineFromPosition(GetCurrentPos()) : line;

	wxString text = GetLine(currLine);
	int length = (int)text.Length();
	wxString indent;
	for (int i = 0; i < length; ++i)
	{
		if (text[i] == wxT(' ') || text[i] == wxT('\t'))
			indent << text[i];
		else
			break;
	}

	return indent;
}

int CodeEditor::FindBlockStart(int position, wxChar blockStart, wxChar blockEnd, bool skipNested /* = true */)
{
	(void) skipNested;
	
	int level = 0;
	wxChar ch = wxChar(GetCharAt(position));
	while (ch)
	{
		if (ch == blockEnd)
			++level;

		else if (ch == blockStart)
		{
			if (level == 0) return position;
			--level;
		}

		--position;

		ch = wxChar(GetCharAt(position));
	}

	return -1;
}

void CodeEditor::HighlightBraces()
{
	int currPos = GetCurrentPos();
	int newPos = BraceMatch(currPos);
	if (newPos == wxSTC_INVALID_POSITION)
	{
		if(currPos > 0)
			newPos = BraceMatch(--currPos);
	}

	wxChar ch = wxChar(GetCharAt(currPos));
	if (ch == wxT('{') || ch == wxT('[') || ch == wxT('(') ||
		ch == wxT('}') || ch == wxT(']') || ch == wxT(')'))
	{
		if (newPos != wxSTC_INVALID_POSITION)
		{
			BraceHighlight(currPos, newPos);
		}
		else
		{
			BraceBadLight(currPos);
		}
	}
	else BraceHighlight(-1, -1);

	Refresh(false);
}

wxString CodeEditor::DeterminePrefs (const wxString &filename) {

    LanguageInfo const* curInfo;

    // determine language from filepatterns
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++) {
        curInfo = &g_LanguagePrefs [languageNr];
        wxString filepattern = curInfo->filepattern;
        filepattern.Lower();
        while (!filepattern.empty()) {
            wxString cur = filepattern.BeforeFirst (';');
            if ((cur == filename) ||
                (cur == (filename.BeforeLast ('.') + _T(".*"))) ||
                (cur == (_T("*.") + filename.AfterLast ('.')))) {
                return curInfo->name;
            }
            filepattern = filepattern.AfterFirst (';');
        }
    }
    return wxEmptyString;

}

bool CodeEditor::InitializePrefs (const wxString &name) {

    // initialize styles
    StyleClearAll();
    LanguageInfo const* curInfo = NULL;

    // determine language
    bool found = false;
    int languageNr;
    for (languageNr = 0; languageNr < g_LanguagePrefsSize; languageNr++) {
        curInfo = &g_LanguagePrefs [languageNr];
        if (curInfo->name == name) {
            found = true;
            break;
        }
    }
    if (!found) return false;

    // set lexer and language
    SetLexer (curInfo->lexer);
    mlanguage = curInfo;

    // set margin for line numbers
    SetMarginType (mLineNrID, wxSTC_MARGIN_NUMBER);
    StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (_T("DARK GREY")));
    StyleSetBackground (wxSTC_STYLE_LINENUMBER, *wxWHITE);
    SetMarginWidth (mLineNrID, 0); // start out not visible

    // default fonts for all styles!
    int Nr;
    for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++) {
        wxFont font (10, wxMODERN, wxNORMAL, wxNORMAL);
        StyleSetFont (Nr, font);
    }

    // set common styles
    StyleSetForeground (wxSTC_STYLE_DEFAULT, wxColour (_T("DARK GREY")));
    StyleSetForeground (wxSTC_STYLE_INDENTGUIDE, wxColour (_T("DARK GREY")));

    // initialize settings
    if (g_CommonPrefs.syntaxEnable) {
        int keywordnr = 0;
        for (Nr = 0; Nr < STYLE_TYPES_COUNT; Nr++) {
            if (curInfo->styles[Nr].type == -1) continue;
            const StyleInfo &curType = g_StylePrefs [curInfo->styles[Nr].type];
            wxFont font (curType.fontsize, wxMODERN, wxNORMAL, wxNORMAL, false,
                         curType.fontname);
            StyleSetFont (Nr, font);
            if (curType.foreground) {
                StyleSetForeground (Nr, wxColour (curType.foreground));
            }
            if (curType.background) {
                StyleSetBackground (Nr, wxColour (curType.background));
            }
            StyleSetBold (Nr, (curType.fontstyle & mySTC_STYLE_BOLD) > 0);
            StyleSetItalic (Nr, (curType.fontstyle & mySTC_STYLE_ITALIC) > 0);
            StyleSetUnderline (Nr, (curType.fontstyle & mySTC_STYLE_UNDERL) > 0);
            StyleSetVisible (Nr, (curType.fontstyle & mySTC_STYLE_HIDDEN) == 0);
            StyleSetCase (Nr, curType.lettercase);
            const char *pwords = curInfo->styles[Nr].words;
            if (pwords) {
                SetKeyWords (keywordnr, pwords);
                keywordnr += 1;
            }
        }
    }

    // set margin as unused
    SetMarginType (mDividerID, wxSTC_MARGIN_SYMBOL);
    SetMarginWidth (mDividerID, 0);
    SetMarginSensitive (mDividerID, false);

    // folding
    SetMarginType (mFoldingID, wxSTC_MARGIN_SYMBOL);
    SetMarginMask (mFoldingID, wxSTC_MASK_FOLDERS);
    StyleSetBackground (mFoldingID, *wxWHITE);
    SetMarginWidth (mFoldingID, 0);
    SetMarginSensitive (mFoldingID, false);
    if (g_CommonPrefs.foldEnable) {
        SetMarginWidth (mFoldingID, curInfo->folds != 0? mFoldingMargin: 0);
        SetMarginSensitive (mFoldingID, curInfo->folds != 0);
        SetProperty (_T("fold"), curInfo->folds != 0? _T("1"): _T("0"));
        SetProperty (_T("fold.comment"),
                     (curInfo->folds & mySTC_FOLD_COMMENT) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.compact"),
                     (curInfo->folds & mySTC_FOLD_COMPACT) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.preprocessor"),
                     (curInfo->folds & mySTC_FOLD_PREPROC) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.html"),
                     (curInfo->folds & mySTC_FOLD_HTML) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.html.preprocessor"),
                     (curInfo->folds & mySTC_FOLD_HTMLPREP) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.comment.python"),
                     (curInfo->folds & mySTC_FOLD_COMMENTPY) > 0? _T("1"): _T("0"));
        SetProperty (_T("fold.quotes.python"),
                     (curInfo->folds & mySTC_FOLD_QUOTESPY) > 0? _T("1"): _T("0"));
    }
    SetFoldFlags (wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED |
                  wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

    // set spaces and indention
    SetTabWidth (4);
    SetUseTabs (false);
    SetTabIndents (true);
    SetBackSpaceUnIndents (true);
    SetIndent (g_CommonPrefs.indentEnable? 4: 0);

    // others
    SetViewEOL (g_CommonPrefs.displayEOLEnable);
    SetIndentationGuides (g_CommonPrefs.indentGuideEnable);
    SetEdgeColumn (80);
    SetEdgeMode (g_CommonPrefs.longLineOnEnable? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
    SetViewWhiteSpace (g_CommonPrefs.whiteSpaceEnable?
                       wxSTC_WS_VISIBLEALWAYS: wxSTC_WS_INVISIBLE);
    SetOvertype (g_CommonPrefs.overTypeInitial);
    SetReadOnly (g_CommonPrefs.readOnlyInitial);
    SetWrapMode (g_CommonPrefs.wrapModeInitial?
                 wxSTC_WRAP_WORD: wxSTC_WRAP_NONE);

    return true;
}


//----------------------------------------------------------------------------
// Common event handlers
void CodeEditor::OnSize(wxSizeEvent& event)
{
	int x = GetClientSize().x + mLineNrMargin + mFoldingMargin;

	if (x > 0) SetScrollWidth(x);

	event.Skip();
}

// Edit event handlers
void CodeEditor::OnEditRedo(wxCommandEvent &WXUNUSED(event))
{
	if (!CanRedo()) return;

	Redo();
}

void CodeEditor::OnEditUndo(wxCommandEvent &WXUNUSED(event))
{
	if (!CanUndo()) return;

	Undo();
}

void CodeEditor::OnEditClear(wxCommandEvent &WXUNUSED(event))
{
	if (GetReadOnly()) return;

	Clear();
}

void CodeEditor::OnEditCut(wxCommandEvent &WXUNUSED(event))
{
	if (GetReadOnly() || (GetSelectionEnd()-GetSelectionStart() <= 0)) return;

	Cut();
}

void CodeEditor::OnEditCopy(wxCommandEvent &WXUNUSED(event))
{
	if (GetSelectionEnd()-GetSelectionStart() <= 0) return;

	Copy();
}

void CodeEditor::OnEditPaste(wxCommandEvent &WXUNUSED(event))
{
	if(!CanPaste()) return;

	Paste();
}

void CodeEditor::OnFind(wxCommandEvent &WXUNUSED(event))
{
}

void CodeEditor::OnFindNext(wxCommandEvent &WXUNUSED(event))
{
}

void CodeEditor::OnReplace(wxCommandEvent &WXUNUSED(event))
{
}

void CodeEditor::OnReplaceNext(wxCommandEvent &WXUNUSED(event))
{
}

void CodeEditor::OnBraceMatch(wxCommandEvent &WXUNUSED(event))
{
	int min = GetCurrentPos();
	int max = BraceMatch(min);
	if (max > (min+1))
	{
		BraceHighlight(min + 1, max);
		SetSelection(min + 1, max);
	}
	else
	{
		BraceBadLight(min);
	}
}

void CodeEditor::OnGoto(wxCommandEvent &WXUNUSED(event))
{
}

void CodeEditor::OnEditIndentInc(wxCommandEvent &WXUNUSED(event))
{
	CmdKeyExecute(wxSTC_CMD_TAB);
}

void CodeEditor::OnEditIndentRed(wxCommandEvent &WXUNUSED(event))
{
	CmdKeyExecute(wxSTC_CMD_DELETEBACK);
}

void CodeEditor::OnEditSelectAll(wxCommandEvent &WXUNUSED(event))
{
	SetSelection(0, GetLength());
}

void CodeEditor::OnEditSelectLine(wxCommandEvent &WXUNUSED(event))
{
	int lineStart = PositionFromLine(GetCurrentLine());
	int lineEnd = PositionFromLine(GetCurrentLine() + 1);
	SetSelection(lineStart, lineEnd);
}

void CodeEditor::OnHilightLang(wxCommandEvent &event)
{
	InitializePrefs(g_LanguagePrefs [event.GetId() - ID_HILIGHTFIRST].name);
}

void CodeEditor::OnDisplayEOL(wxCommandEvent &WXUNUSED(event))
{
	SetViewEOL (!GetViewEOL());
}

void CodeEditor::OnIndentGuide(wxCommandEvent &WXUNUSED(event))
{
	SetIndentationGuides(!GetIndentationGuides());
}

void CodeEditor::OnLineNumber(wxCommandEvent &WXUNUSED(event))
{
	SetMarginWidth(mLineNrID, GetMarginWidth(mLineNrID) == 0 ? mLineNrMargin : 0);
}

void CodeEditor::OnLongLineOn(wxCommandEvent &WXUNUSED(event))
{
	SetEdgeMode(GetEdgeMode() == 0? wxSTC_EDGE_LINE: wxSTC_EDGE_NONE);
}

void CodeEditor::OnWhiteSpace(wxCommandEvent &WXUNUSED(event))
{
	SetViewWhiteSpace (GetViewWhiteSpace() == 0 ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE);
}

void CodeEditor::OnFoldToggle(wxCommandEvent &WXUNUSED(event))
{
	ToggleFold(GetFoldParent(GetCurrentLine()));
}

void CodeEditor::OnSetOverType(wxCommandEvent &WXUNUSED(event))
{
	SetOvertype(!GetOvertype());
}

void CodeEditor::OnSetReadOnly(wxCommandEvent &WXUNUSED(event))
{
	SetReadOnly(!GetReadOnly());
}

void CodeEditor::OnWrapModeOn(wxCommandEvent &WXUNUSED(event))
{
	SetWrapMode(GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
}

void CodeEditor::OnUseCharset(wxCommandEvent &event)
{
	(void) event;
	/*
	int Nr;
	int charset = GetCodePage();

	switch (event.GetId())
	{
		case ID_CHARSETANSI: { charset = wxSTC_CHARSET_ANSI; break; }
		case ID_CHARSETMAC: { charset = wxSTC_CHARSET_ANSI; break; }
	}

	for (Nr = 0; Nr < wxSTC_STYLE_LASTPREDEFINED; Nr++)
	{
		StyleSetCharacterSet(Nr, charset);
	}

	SetCodePage(charset);
	*/
}

void CodeEditor::OnChangeCase(wxCommandEvent &event)
{
	int id = event.GetId();
	if(id == ID_CHANGELOWER) CmdKeyExecute(wxSTC_CMD_LOWERCASE);
	else if(id == ID_CHANGEUPPER) CmdKeyExecute(wxSTC_CMD_UPPERCASE);
}

void CodeEditor::OnConvertEOL(wxCommandEvent &event)
{
	int eolMode = GetEOLMode();

	const int id = event.GetId();
	if(id == ID_CONVERTCR) eolMode = wxSTC_EOL_CR;
	else if(id == ID_CONVERTCRLF) eolMode = wxSTC_EOL_CRLF;
	else if(id == ID_CONVERTLF) eolMode = wxSTC_EOL_LF;

	ConvertEOLs(eolMode);
	SetEOLMode(eolMode);
}

void CodeEditor::OnMarginClick(wxStyledTextEvent &event)
{
	if (event.GetMargin() == 1)
	{
		int lineClick = LineFromPosition(event.GetPosition());
		int levelClick = GetFoldLevel(lineClick);
		if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
		{
			ToggleFold (lineClick);
		}
	}
}

void CodeEditor::OnCharAdded(wxStyledTextEvent &event)
{
	char ch = (char) event.GetKey();
	int currentLine = GetCurrentLine();
	int pos = GetCurrentPos();

	if (ch == wxT('\n') && currentLine > 0)
	{
		BeginUndoAction();

		wxString indent = GetLineIndentString(currentLine - 1);

		wxChar b = GetLastNonWhitespaceChar();
		if(b == wxT('{'))
		{
			if(GetUseTabs())
				indent << wxT("\t");
			else
				indent << wxT("    ");
		}

		InsertText(pos, indent);
		GotoPos((int)(pos + indent.Length()));
		ChooseCaretX();

		EndUndoAction();
	}
	else if(ch == wxT('}'))
	{
		BeginUndoAction();

		wxString line = GetLine(currentLine);
		line.Trim(false);
		line.Trim(true);
		if(line.Matches(wxT("}")))
		{
			pos = GetCurrentPos() - 2;
			pos = FindBlockStart(pos, wxT('{'), wxT('}'));

			if(pos != -1)
			{
				wxString indent = GetLineIndentString(LineFromPosition(pos));
				indent << wxT('}');
				DelLineLeft();
				DelLineRight();
				pos = GetCurrentPos();
				InsertText(pos, indent);
				GotoPos((int)(pos + indent.Length()));
				ChooseCaretX();
			}
		}

		EndUndoAction();
	}
}

void CodeEditor::OnUpdateUI(wxStyledTextEvent &event)
{
	(void) event;
	HighlightBraces();
}

bool CodeEditor::LoadFile() 
{
	// Get filname
	if (!mFileName)  {
		wxFileDialog dlg (this, _T("Open file"), _T(""), _T(""),
			_T("Any file (*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
		if (dlg.ShowModal() != wxID_OK) return false;
		mFileName = dlg.GetPath();
	}

	// Load file
	return LoadFile(mFileName);
}

bool CodeEditor::LoadFile(const wxString &filename)
{
	// Load file in edit and clear undo
	if (!filename.IsEmpty()) mFileName = filename;
	if (!wxStyledTextCtrl::LoadFile(mFileName)) return false;

	// Determine lexer language
	wxFileName fname(mFileName);
	InitializePrefs(DeterminePrefs(fname.GetFullName()));
	return true;
}

bool CodeEditor::SaveFile()
{
	// Return if no change
	if (!Modified()) return true;

	// Get file name
	if (!mFileName)
	{
		wxFileDialog dlg (this, _T("Save file"), _T(""), _T(""), _T("Any file (*)|*"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (dlg.ShowModal() != wxID_OK) return false;
		mFileName = dlg.GetPath();
	}

	// Save file
	return SaveFile(mFileName);
}

bool CodeEditor::SaveFile(const wxString &filename)
{
	// Return if no change
	if (!Modified()) return true;

	return wxStyledTextCtrl::SaveFile(filename);
}

bool CodeEditor::Modified()
{
	// Return modified state
	return (GetModify() && !GetReadOnly());
}
