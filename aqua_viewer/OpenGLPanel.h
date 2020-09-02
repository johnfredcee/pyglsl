#ifndef __WXOPENGLPANEL_H__
#define __WXOPENGLPANEL_H__

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/glcanvas.h>

#if  (wxUSE_GLCANVAS == 0)
#error "Must be build with wx OpenGL support enabled."
#endif


class OpenGLPanel :	public wxGLCanvas {
	DECLARE_CLASS(OpenGLPanel)
public:
	/** A new OpenGLPanel must receive a parent frame to which to attach
		itself to */
	OpenGLPanel(wxWindow *parent);
	~OpenGLPanel();

	void createRenderWindow();

	void toggleTimerRendering();
	void resetCamera();
	void wireFrame(bool on);

protected:
	DECLARE_EVENT_TABLE()

private:

	/** WX Callbacks */
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	//void OnEraseBackground(wxEraseEvent& event);
	void OnRenderTimer(wxTimerEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void Render();

	/** Rendering timer */
	wxTimer mTimer;
	wxPoint mPrevPos;
	float mZoomScale;
	bool mWireframe;
	bool mCreated;

};


#endif // __WXOGRE_H__
