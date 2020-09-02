#ifndef ANIMATIONPANEL_H
#define ANIMATIONPANEL_H

class AnimationListControl;

class AnimationPanel : public wxPanel
{
        DECLARE_DYNAMIC_CLASS(ScenePanel);
    public:

	/**
	 * Second stage constructor
	 * @param parent Parent widget
	 * @param id Window ID
	 * @param pos Window position
	 * @param size Window Size
	 * @param style Style flags for window
	 * @param name Name of window
	 */
		AnimationPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
					   const wxPoint& pos = wxDefaultPosition,
					   const wxSize& size = wxDefaultSize,
					   long style = wxTAB_TRAVERSAL,
					   const wxString& name = wxT("Animations in scene"));
		
        /** < D'Tor - to clean up our own resources  */
        virtual ~AnimationPanel();

    private:
		void OnPlayAnimation(wxCommandEvent& evt);

		void OnPauseAnimation(wxCommandEvent& evt);

		void OnStopAnimation(wxCommandEvent& evt);
		
		AnimationListControl* animationList_;
		
        //!< Panel With Control buttons
        wxPanel*   buttonPanel_;

        //!< Sizer to contain anim list
        wxBoxSizer *sizer_;

        // action buttons
        wxButton     *playButton_, *stopButton_, *pauseButton_;

        DECLARE_EVENT_TABLE();
};
#endif
