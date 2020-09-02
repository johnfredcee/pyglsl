#if !defined(CODEPANELCOLLECTION_H_INCLUDED)
#define CODEPANELCOLLECTION_H_INCLUDED

class wxConfigBase;
class wxWindow;

#include "Locator.h"
#include "wxWindowCollection.h"
#include "CodeEditor.h"
#include "CodePanel.h"
#include "Config.h"


class CodePanelCollection : public	wxWindowCollection<CodePanel> {
public:
	CodePanelCollection(wxWindow* parent) : wxWindowCollection<CodePanel>(wxT("CodePanels"), parent) {
		
	}

	~CodePanelCollection() {
	}
};

#endif


