#if !defined(PLAYBOXRENDERFRAME_H_INCLUDED)
#define PLAYBOXRENDERFRAME_H_INCLUDED

namespace aqua
{
	class GraphicsCamera;
}

class RenderPanel : public wxWindow
{
	DECLARE_CLASS(RenderPanel)

	wxTimer* timer_;
	bool     created_;
	float    speed_;

	//!< User camera3d_
	aqua::shared_ptr< aqua::GraphicsCamera > camera3d_;

	void OnIdle(wxIdleEvent& idle);
	void OnRenderTimer(wxTimerEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseMotion(wxMouseEvent& squeak);
	void OnMouseWheel(wxMouseEvent& event);

	void toggleTimerRendering();
	void paintNow();
	void OnDestroy(wxWindowDestroyEvent& bang);
	void OnKeyDown(wxKeyEvent& event);

public:
	RenderPanel();
	RenderPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	bool Initialise();
	void Render(wxDC& dc);
	void Resize(int width, int height);
	void focusOnSceneExtents(const aqua::PbVector3& viewDirection = aqua::PbVector3(0.0f, 0.0f, -1.0f));

	DECLARE_EVENT_TABLE();
};

#endif


