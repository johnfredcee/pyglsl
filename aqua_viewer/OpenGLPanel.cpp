#include "StdAfx.h"

#include "OpenGLPanel.h"

#ifdef __WXGTK__
#include <gdk/gdkx.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h> 
#include <wx/gtk/win_gtk.h>
#include <GL/glx.h>
#endif

// Required for the timer
const long ID_RENDERTIMER = wxNewId();
const long ID_GL_CANVAS   = wxNewId();

IMPLEMENT_CLASS(OpenGLPanel, wxGLCanvas)

BEGIN_EVENT_TABLE(OpenGLPanel, wxGLCanvas)
	EVT_SIZE(OpenGLPanel::OnSize)
	EVT_PAINT(OpenGLPanel::OnPaint)
//EVT_ERASE_BACKGROUND(OpenGLPanel::OnEraseBackground)
	EVT_TIMER(ID_RENDERTIMER, OpenGLPanel::OnRenderTimer)
	EVT_LEFT_DOWN(OpenGLPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(OpenGLPanel::OnMouseDown)
	EVT_RIGHT_DOWN(OpenGLPanel::OnMouseDown)
	EVT_MOTION(OpenGLPanel::OnMouseMotion)
	EVT_MOUSEWHEEL(OpenGLPanel::OnMouseWheel)
END_EVENT_TABLE()

static	int attribs[3] = {
		WX_GL_RGBA,
		WX_GL_DOUBLEBUFFER,
		0
	};

#pragma warning( disable : 4355 )
OpenGLPanel::OpenGLPanel(wxWindow *parent) :
	wxGLCanvas(parent, ID_GL_CANVAS, attribs, wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas")),
	mTimer(this, ID_RENDERTIMER),
	mZoomScale(1.0f),
	mCreated(false)
{
}
#pragma warning( default : 4355 )

void OpenGLPanel::wireFrame(bool wireframe)
{
	(void) wireframe;
    // if (mWireFrame)
    //     mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
    // else
    //     mCamera->setPolygonMode(Ogre::PM_SOLID);
}


void OpenGLPanel::resetCamera()
{
    // mCamera->setAutoTracking(false);
	// mCameraNode->setPosition(Ogre::Vector3::ZERO);
	// mCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
	// mCamera->setPosition(Ogre::Vector3::ZERO);
}

void OpenGLPanel::createRenderWindow()
{
	mCreated = true;
	this->Show(); // GL context doesn't appear until window shown
}


void OpenGLPanel::toggleTimerRendering()
{
	// // Toggle Start/Stop
	// if (mTimer.IsRunning())
	// 	mTimer.Stop();
	// mTimer.Start(10);
}

OpenGLPanel::~OpenGLPanel()
{
	mTimer.Stop();
}

void OpenGLPanel::Render()
{
	wxGLContext context(this);
    SetCurrent(context);
    wxPaintDC(this);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, (GLint)GetSize().x, (GLint)GetSize().y);
 
    glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(-0.5f, 0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(0.5f, -0.5f);
	glColor3f(0.4f, 0.5f, 0.4f);
	glVertex2f(0.0f, -0.8f);
    glEnd();
 
    glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.1f, 0.1f);
	glVertex2f(-0.1f, 0.1f);
	glVertex2f(-0.1f, -0.1f);
	glVertex2f(0.1f, -0.1f);
    glEnd();
 
 
    glFlush();
    SwapBuffers();
}

void OpenGLPanel::OnSize(wxSizeEvent& event)
{
	(void) event;
	// Setting new size;
	int width;
	int height;
	GetSize(&width, &height);
    
    //if (mRenderWindow) {
    //    mRenderWindow->resize( width, height );
    //    // Letting Ogre know the window has been resized;
    //    mRenderWindow->windowMovedOrResized();
	// }

    //if (mCamera) {
	// Set the aspect ratio for the new size;
	//if (mCamera)
	//    mCamera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height));
    //}
	if (mCreated)
		Update();
}

void OpenGLPanel::OnPaint(wxPaintEvent& event)
{
	(void) event;
	Render();
}

// void OpenGLPanel::OnEraseBackground(wxEraseEvent& event)
// {
// 	if (mCreated)
// 		Render();
// }

void OpenGLPanel::OnRenderTimer(wxTimerEvent& event)
{
	(void) event;
	if (mCreated)
		Update();
}

void OpenGLPanel::OnMouseDown(wxMouseEvent& event)
{
	mPrevPos = event.GetPosition();
	event.Skip();
}


void OpenGLPanel::OnMouseMotion(wxMouseEvent& event)
{
	static float speed = 0.0f;
	static bool dragStart = true;
	static const wxString posInfo(wxT("Pos:X:%03d, Y:%03d Change X:%03d Y:%03d"));
	wxPoint pos = event.GetPosition();
	wxPoint change = pos - mPrevPos;

	if ((!dragStart) && ((!event.Dragging()) || (!event.LeftIsDown())))
	{
		wxString msg(wxT("Drag End"));
		dragStart = true;
	}

	if(event.Dragging())
	{
		if (event.LeftIsDown())
		{			
			wxString msg(wxT("Drag Start"));
		}
		else if(event.MiddleIsDown())
		{
			//int left, top, width, height;
			float speed = 1.0f;
			if (event.ShiftDown())
				speed = 0.1f;
			if (event.ControlDown())
				speed = 10.0f;
			//float moveX = ((float)-change.x / (float)width) * mZoomScale * speed;
			//float moveY = ((float)change.y / (float)height) * mZoomScale * speed;
		}
	}
	mPrevPos = pos;
}

void OpenGLPanel::OnMouseWheel(wxMouseEvent& event)
{
	float speed = 1.0f;
	if (event.ShiftDown())
		speed = 0.1f;
	if (event.ControlDown())
		speed = 10.0f;

	float moveZ = aqua::Float32((event.GetWheelRotation() / 120.0) / 100.0);
	
	moveZ = moveZ * mZoomScale * speed;

}


