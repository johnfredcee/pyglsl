#include "StdAfx.h"

#include <wx/treectrl.h>
#include <wx/progdlg.h>
#include <wx/kbdstate.h>

#include "AnimationPanel.h"
#include "AnimationListControl.h"
#include "ViewerFrame.h"

wxWindowID ANIMATION_PROPERTIES_ID     = wxNewId();
wxWindowID ID_PLAY_ANIMATION       = wxNewId();
wxWindowID ID_PAUSE_ANIMATION      = wxNewId();
wxWindowID ID_STOP_ANIMATION       = wxNewId();


IMPLEMENT_CLASS(AnimationPanel, wxPanel)

BEGIN_EVENT_TABLE(AnimationPanel, wxPanel)
    EVT_BUTTON(ID_PLAY_ANIMATION,						AnimationPanel::OnPlayAnimation)
    EVT_BUTTON(ID_PAUSE_ANIMATION,						AnimationPanel::OnPauseAnimation)
    EVT_BUTTON(ID_STOP_ANIMATION,						AnimationPanel::OnStopAnimation)
END_EVENT_TABLE()

using namespace aqua;


/**
 * Scene tree constructor
 * @param parent Parent window (usually main frame)
 * @param id     Id of window (usually dynamically assigned)
 * @param pos    Initial position
 * @param size   Initial size
 * @param style  Style (bog standard child window)
 * @param name   Name for identification purposes
 */
AnimationPanel::AnimationPanel(wxWindow* parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name) : wxPanel(parent, id, pos, size, style, name),  sizer_(NULL)
{
    wxGridSizer *buttonSizer  =	new wxGridSizer(1,3,5,5); // 1 row, 3 cols
    sizer_ =			new wxBoxSizer(wxVERTICAL);
    buttonPanel_ =		new wxPanel(this);
    playButton_ =		new wxButton(buttonPanel_, ID_PLAY_ANIMATION, "Play Animation");
	playButton_->Disable();
    pauseButton_ =		new wxButton(buttonPanel_, ID_PAUSE_ANIMATION,	"Pause Animation");
	pauseButton_->Disable();
    stopButton_ =		new wxButton(buttonPanel_, ID_STOP_ANIMATION, "Stop Animation");
	stopButton_->Disable();
    buttonSizer->Add(playButton_, 1);
    buttonSizer->Add(pauseButton_, 1);
    buttonSizer->Add(stopButton_, 1);
	animationList_ = new AnimationListControl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	sizer_->Add(animationList_, 10, wxEXPAND);
    sizer_->Add(buttonPanel_, 1);
    SetSizer(sizer_);
    Layout();
	return;
} 

AnimationPanel::~AnimationPanel() {
	
}

void AnimationPanel::OnPlayAnimation(wxCommandEvent& evt) 
{
	(void) evt;
}

void AnimationPanel::OnPauseAnimation(wxCommandEvent& evt) 
{
	(void) evt;
}

void AnimationPanel::OnStopAnimation(wxCommandEvent& evt) 
{
	(void) evt;
}
