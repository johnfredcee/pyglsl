#ifndef ANIMATIONLISTCONTROL_H
#define ANIMATIONLISTCONTROL_H


class AnimationListControl : public wxListCtrl {

    public:
        AnimationListControl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr);


        virtual ~AnimationListControl();
};

#endif
