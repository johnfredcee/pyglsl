#ifndef _CODEEDITOR_H_
#define _CODEEDITOR_H_



class CodeEditor : public wxStyledTextCtrl
{
public:
	CodeEditor(wxWindow *parent, wxWindowID id = -1,
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = wxVSCROLL
		);

	~CodeEditor();
	
	// Language/Lexer
	LanguageInfo const* GetLanguageInfo () {return mlanguage;};

	// Load/Save file
	bool LoadFile();
	bool LoadFile(const wxString &filename);
	bool SaveFile();
	bool SaveFile(const wxString &filename);
	bool Modified();
	wxString GetFilename() {return mFileName;};
	void SetFilename(const wxString &filename) { mFileName = filename; };
	bool InitializePrefs (const wxString &name);
	
private:
	void OnSize(wxSizeEvent &event);
	// Edit
	void OnEditRedo(wxCommandEvent &event);
	void OnEditUndo(wxCommandEvent &event);
	void OnEditClear(wxCommandEvent &event);
	void OnEditCut(wxCommandEvent &event);
	void OnEditCopy(wxCommandEvent &event);
	void OnEditPaste(wxCommandEvent &event);
	// Find
	void OnFind(wxCommandEvent &event);
	void OnFindNext(wxCommandEvent &event);
	void OnReplace(wxCommandEvent &event);
	void OnReplaceNext(wxCommandEvent &event);
	void OnBraceMatch(wxCommandEvent &event);
	void OnGoto(wxCommandEvent &event);
	void OnEditIndentInc(wxCommandEvent &event);
	void OnEditIndentRed(wxCommandEvent &event);
	void OnEditSelectAll(wxCommandEvent &event);
	void OnEditSelectLine(wxCommandEvent &event);
	// View
	void OnHilightLang(wxCommandEvent &event);
	void OnDisplayEOL(wxCommandEvent &event);
	void OnIndentGuide(wxCommandEvent &event);
	void OnLineNumber(wxCommandEvent &event);
	void OnLongLineOn(wxCommandEvent &event);
	void OnWhiteSpace(wxCommandEvent &event);
	void OnFoldToggle(wxCommandEvent &event);
	void OnSetOverType(wxCommandEvent &event);
	void OnSetReadOnly(wxCommandEvent &event);
	void OnWrapModeOn(wxCommandEvent &event);
	void OnUseCharset(wxCommandEvent &event);
	// Extra
	void OnChangeCase(wxCommandEvent &event);
	void OnConvertEOL(wxCommandEvent &event);
	// Styled text
	void OnMarginClick(wxStyledTextEvent &event);
	void OnCharAdded(wxStyledTextEvent &event);
	void OnUpdateUI(wxStyledTextEvent &event);


protected:
	wxChar GetLastNonWhitespaceChar(int position = -1);
	wxString GetLineIndentString(int line);
	int FindBlockStart(int position, wxChar blockStart, wxChar blockEnd, bool skipNested = true);

	void HighlightBraces();
	wxString DeterminePrefs (const wxString &filename);

private:
	// File
	wxString mFileName;

	// lanugage properties
	LanguageInfo const* mlanguage;

	// Margin variables
	int mLineNrID;
	int mLineNrMargin;
	int mFoldingID;
	int mFoldingMargin;
	int mDividerID;

	DECLARE_EVENT_TABLE()
};

#endif // _CODEEDITOR_H_

