
#include "StdAfx.h"

#include <Utils/Clock.h>
#include <Utils/Logger.h>
#include <Utils/stringfunctions.h>
#include <aqua/VertexBufferData.h>
#include <aqua/VertexIndexBufferData.h>
#include <aqua/Material.h>
#include <GraphicsSystem/GraphicsSystem.h>
#include <GraphicsSystem/GraphicsCamera.h>
#include <Shader/ShaderProgram.h>
#include <Shader/ShaderCache.h>
#include <Managers/MaterialManager.h>
#include <Managers/ShaderManager.h>
#include <Utils/stringfunctions.h>

#include "Config.h"
#include "MetaDataUtils.h"
#include "LayerManager.h"
#include "IGraphicObject.h"
#include "GraphicObject.h"
#include "LayerManager.h"
#include "MetaSerializer.h"
#include "SceneManager.h"
#include "ModelViewProjectionParameterUpdater.h"
#include "Shaders.h"
#include "renderpanel.h"

#if	 ((!defined(LINUX)) && (!defined(WIN32)))
#error "Forget it - Win32 only at the moment. Linux a possiblity"
#endif

// sbome camera parameters
const aqua::PbReal CAMERA_SPEED_MOVEMENT = 1.0F;
const aqua::PbReal CAMERA_SPEED_ROTATION = 2.0F;
const aqua::PbReal CAMERA_SPEED_ZOOM = 1.0F;

// Required for the timer
const long ID_RENDERTIMER = wxNewId();

#ifdef LINUX
#error "Doesn't support Linux yet"
#endif

#ifdef WIN32
#define GAME_NAME  ("Tool")
#define WIN32_MEAN_AND_LEAN

#ifndef PB_USE_GLSL
#error "Really only works with GLSL atm"
#endif

#ifdef PB_USE_DIRECTX
#error "Viewer does not support direct x yet"
#include <RenderTarget/RenderTargetDirectX9Win32.h>
#else
#include <RenderTarget/RenderTargetOpenGlWin32.h>
#endif

// -------- PLATFORM SPECIFIC GRAPHICS INIT -------------------------------------------------------


void startGraphics(HINSTANCE hInstance, WNDPROC WndProc, HDC hDC = 0, HGLRC hRC = 0, HWND hWnd = 0)
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	const unsigned int numberOfProcessors = systemInfo.dwNumberOfProcessors;
	if (numberOfProcessors > 1) {
		HANDLE hProcess = GetCurrentProcess();
		DWORD dwProcessAffinityMask, dwSystemAffinityMask;

		GetProcessAffinityMask( hProcess, &dwProcessAffinityMask, &dwSystemAffinityMask );

		SetProcessAffinityMask( hProcess, 2L );// use CPU 1 only
	}

	const aqua::Locator::Service<aqua::GraphicsSystem>& graphicsSystem(aqua::Locator::checkOut<aqua::GraphicsSystem>());
	graphicsSystem->initialise();

	// Get the default render context.
	aqua::shared_ptr<aqua::RenderTarget> currentContext(graphicsSystem->getCurrentRenderTarget());

	// cast to get Windows specific functions
	aqua::shared_ptr<aqua::RenderTargetOpenGlWin32> currentWin32Context(boost::dynamic_pointer_cast<aqua::RenderTargetOpenGlWin32, aqua::RenderTarget>(currentContext));

	if (hWnd == 0) // User hasn't passed a hWnd, so we'll create our own window
		hWnd = currentWin32Context->createWindow(hInstance, WndProc, aqua::RenderTargetOpenGlWin32::CAPTION_BAR | aqua::RenderTargetOpenGlWin32::MINIMIZE_BUTTON);
	currentWin32Context->setWindowDetails(hDC, hRC, hWnd);
	currentWin32Context->setWindowed(true, "Aqua Viewer");
	currentWin32Context->displayOn();
	aqua::Locator::checkIn(graphicsSystem);
}
#endif


// -- RENDER PANEL ----------------------------------------

	
RenderPanel::RenderPanel()
	: created_(false) {	
}


RenderPanel::RenderPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: created_(false),
	  speed_(0.3F),
	  wxWindow(parent, id, pos, size, style, wxT("RenderPanel"))
{

}


IMPLEMENT_CLASS(RenderPanel, wxWindow)

BEGIN_EVENT_TABLE(RenderPanel, wxWindow)
	EVT_SIZE(RenderPanel::OnSize)
	EVT_PAINT(RenderPanel::OnPaint)
	EVT_ERASE_BACKGROUND(RenderPanel::OnEraseBackground)
	EVT_TIMER(ID_RENDERTIMER, RenderPanel::OnRenderTimer)
	EVT_MOTION(RenderPanel::OnMouseMotion)
	EVT_MOUSEWHEEL(RenderPanel::OnMouseWheel)
	EVT_KEY_DOWN(RenderPanel::OnKeyDown)
END_EVENT_TABLE()


aqua::shared_ptr<aqua::VertexBufferData>	  g_vbuffer;
aqua::shared_ptr<aqua::VertexIndexBufferData> g_vibuffer;
aqua::shared_ptr<aqua::Material>		  g_material;
aqua::shared_ptr<aqua::PbMatrix4>		  g_matrix;

// Test tetrahedron ----------------------------------------
//!
//! Default hideous pink material for missing textures, etc
//! @return pointer to blue::material
//!
aqua::shared_ptr<aqua::Material> hideousPink()
{
	aqua::shared_ptr<aqua::Material> result(new aqua::Material);
	PbColor			 localDiffuse;
	// default to hideous "pink"
	localDiffuse.setR(1.0);
	localDiffuse.setB(0.5);
	localDiffuse.setG(0.5);
	localDiffuse.setA(0.5);
	result->diffuse = localDiffuse;
	result->shader	= aqua::HashString("simple_shader");
	return result;
}

/**  Create a tetrahedron for test rendering purposes. */
void  makeTetrahedron() {
	
	static float v[4][3] = {
		-1, -1, -1,
		1,	1, -1,
		1, -1,	1,
		-1,	 1,	 1,
	};

	static int f[4][3] = {
		1, 2, 3,
		1, 0, 2,
		3, 2, 0,
		0, 1, 3,
	};


	int nverts = 4;
	int nfaces = 4;

	g_material = hideousPink();
	
	aqua::Array<PrimitiveData>			primitives;
	primitives.resize(nverts);
	aqua::Array<NormalData>				normals;
	aqua::Array<TextureCoordinateData>	textureCoordinate1;
	aqua::Array<TextureCoordinateData>	textureCoordinate2;
	aqua::Array<VertexColorData>		colors;	   
	PrimitiveData*						primItr(primitives.begin());
	for (int i = 0; i < nverts; i++) {
		primItr->x = v[i][0];
		primItr->y = v[i][1];
		primItr->z = v[i][2];
		++primItr;
	}
	aqua::Array<aqua::IndexData>	  indexData;
	indexData.resize(nfaces*3);	
	for(int i = 0; i < nfaces; ++i) {
		indexData[i*3+0] = aqua::IndexData(f[i][0]);
		indexData[i*3+1] = aqua::IndexData(f[i][1]);
		indexData[i*3+2] = aqua::IndexData(f[i][2]);
	}
	g_vbuffer  = aqua::shared_ptr<aqua::VertexBufferData>(new aqua::VertexBufferData(primitives, normals, textureCoordinate1, textureCoordinate2, colors));
	g_vibuffer = aqua::shared_ptr<aqua::VertexIndexBufferData>(new aqua::VertexIndexBufferData(indexData));

}


/**
 * Initialise the drawing surface
 * @return true if successful
 */
bool RenderPanel::Initialise()
{
	bool result = false;
	static PIXELFORMATDESCRIPTOR pfd =	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,								// Request An RGBA Format
		static_cast<BYTE>(32),				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		1,											// Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		32,											// 32Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	HWND hWnd = (HWND) GetHandle();
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HDC hDC = GetDC(hWnd);
	WNDPROC WndProc = (WNDPROC)(LONG_PTR)::GetWindowLong(hWnd, GWL_WNDPROC);
	GLuint pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat) {
		SetPixelFormat(hDC, pixelFormat, &pfd);
		HGLRC hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC,hRC);
		startGraphics(hInstance, WndProc, hDC, hRC, hWnd);
		const aqua::Locator::Service<aqua::GraphicsSystem> &graphicsSystem(aqua::Locator::checkOut<aqua::GraphicsSystem>());
		aqua::shared_ptr<aqua::RenderTarget> renderTarget(graphicsSystem->getCurrentRenderTarget());
		const float screenWidth	 = aqua::Float32(renderTarget->getWidth());
		const float screenHeight = aqua::Float32(renderTarget->getHeight());
		const float aspectRatio	 = screenWidth / screenHeight;
		camera3d_ = aqua::shared_ptr<aqua::GraphicsCamera>(new aqua::GraphicsCamera(45.0f, aspectRatio, 0.1f, 5000.0f));
		aqua::PbVector3 camPosition(0.0, 0.0, -10.0f);
		camera3d_->setPosition(camPosition);
		aqua::PbVector3 camFront(0.0f, 0.0f, 1.0f);
		aqua::PbVector3 camUp(0.0f, 1.0f, 0.0f);
		camera3d_->setOrientation(camFront, camUp);
		initialise_shaders();		

// // LIGHTS LightManager& lightManager =
//		graphicsSystem.getLightManager(); lightManager.enable();
//		PbLight& light1 = lightManager.addDiffuseLight();
//		light1.setPosition(PbVector3(-10.0f, 5.0f, 2.0f));
//		light1.setColor(PbColor(1.0f, 1.0f, 1.0f, 0.2f));
//		light1.enable(); PbLight& light2 =
//		lightManager.addAmbientLight(); light2.setColor(PbColor(0.25f,
//		0.25f, 0.25f, 0.2f)); light2.enable(); PbLight& light3 =
//		lightManager.addSpecularLight();
//		light3.setPosition(PbVector3(-10.0f, 5.0f, 2.0f));
//		light3.setColor(PbColor(.1f, 0.1f, 0.1f, 0.2f));
//		light3.enable(); CONFIRM INIIT result = true;
		created_ = true;
		// set up render during idle
		SetExtraStyle(wxWS_EX_PROCESS_IDLE);
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(RenderPanel::OnIdle));
//		graphicsSystem.enableLighting();
//		graphicsSystem.updateLights();
//#ifdef FIXED_TIMESTEP
		timer_ = new wxTimer();
		timer_->SetOwner(this);
		timer_->Start(2);
		Connect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(RenderPanel::OnRenderTimer));
		Connect(wxID_ANY, wxEVT_DESTROY, wxWindowDestroyEventHandler(RenderPanel::OnDestroy));
																							
//#endif
		aqua::Locator::checkIn(graphicsSystem);
	}

// #define USE_TEST_TETRAHEDRON
#ifdef USE_TEST_TETRAHEDRON
	makeTetrahedron();
#endif

	g_matrix = aqua::shared_ptr<aqua::PbMatrix4>(new aqua::PbMatrix4(aqua::PbMatrix4::IDENTITY));	
	return result;
}



/** Handle a window redraw. */
void RenderPanel::paintNow()
{
	wxClientDC dc(this);
	// any wx-style gdi annotoations go in here
	Render(dc);
}

/**
 * Actually render the scene using the game renderer
 * @param dc Device context we draw on
 */
void RenderPanel::Render(wxDC& dc)
{
	PBUNREFERENCED_PARAMETER(dc);
	if (created_) {
		const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
		if (sceneManager->isRenderable()) {
			const aqua::Locator::Service<aqua::GraphicsSystem> &graphicsSystem(aqua::Locator::checkOut<aqua::GraphicsSystem>()); 
			graphicsSystem->setCamera(camera3d_);
			graphicsSystem->startFrame();
#ifdef USE_TEST_TETRAHEDRON		
			graphicsSystem->drawStaticItem(g_material, g_vbuffer, g_vibuffer, g_matrix);
#endif
			sceneManager->renderScene();			
			graphicsSystem->endFrame();
			aqua::Locator::checkIn(graphicsSystem);
		}
		aqua::Locator::checkIn(sceneManager);
	}
	return;
}

/** Position the camera to show the whole scene. */
void RenderPanel::focusOnSceneExtents(const aqua::PbVector3& viewDirection)
{
	aqua::PbVector3 minExtent;
	aqua::PbVector3 maxExtent;
	
	const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());

	sceneManager->getSceneExtents(minExtent, maxExtent);	
	aqua::PbVector3 center;
	center.x() = minExtent.x() + (maxExtent.x() -  minExtent.x()) * 0.5f;
	center.y() = minExtent.y() + (maxExtent.y() -  minExtent.y()) * 0.5f;
	center.z() = minExtent.z() + (maxExtent.z() -  minExtent.z()) * 0.5f;
	float maxExtentRange = maxExtent.x() - minExtent.x();
	float yExtentRange   = maxExtent.y() - minExtent.y();
	float zExtentRange   = maxExtent.z() - minExtent.z();
	if (maxExtentRange < yExtentRange)
		maxExtentRange = yExtentRange;
	if (maxExtentRange < zExtentRange)
		maxExtentRange = zExtentRange;
	aqua::PbVector3 normalisedViewDirection(viewDirection);
	normalisedViewDirection.normalize();
	aqua::PbVector3 viewUp(0.0f, 1.0f, 0.0f);
	camera3d_->setPosition(center + (-normalisedViewDirection) * maxExtentRange);
	camera3d_->setOrientation(normalisedViewDirection, viewUp);
	aqua::Locator::checkIn(sceneManager);
	return;
}

/**
 * Hanlde window resize request
 * @param width Width of window
 * @param height Hieght of window
 */
void RenderPanel::Resize(int width, int height)
{
	wxLogDebug(wxT("Resize %d %d "), width, height);
	if (!GetParent()->IsShown())
		return;
	const aqua::Locator::Service<aqua::GraphicsSystem>& graphicsSystem(aqua::Locator::checkOut<aqua::GraphicsSystem>());
	// Get the default render context
	aqua::shared_ptr<aqua::RenderTarget> currentContext(graphicsSystem->getCurrentRenderTarget());
	aqua::Locator::checkIn(graphicsSystem);
	currentContext->setDisplayDimensions(width, height);
	if (camera3d_)
		camera3d_->setDisplayAspectRatio( (float) height / (float) width );
}

// -- EVENT HANDLERS

void RenderPanel::OnSize(wxSizeEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	// Setting new size;
	int width;
	int height;
	GetSize(&width, &height);
	Resize(width, height);
	event.Skip();
}

void RenderPanel::OnPaint(wxPaintEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	wxPaintDC dc(this);
	Render(dc);
}

void RenderPanel::OnEraseBackground(wxEraseEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
	if ((!created_) || (!(sceneManager->isRenderable()))) {
			event.Skip();
	}
	aqua::Locator::checkIn(sceneManager);
}

void RenderPanel::OnRenderTimer(wxTimerEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	if (created_)
		wxWakeUpIdle();
}

void RenderPanel::OnIdle(wxIdleEvent& idle)
{
	if (created_)
	{
		paintNow();
#ifndef FIXED_TIMESTEP		
		idle.RequestMore();
#endif		
	}
}

void RenderPanel::OnKeyDown(wxKeyEvent& event)
{
	aqua::PbVector3 motion(0.0f, 0.0f, 0.0f);
	if (event.GetKeyCode() == 'A') {
		motion += camera3d_->getRight() * -speed_;
	}
	if (event.GetKeyCode() == 'D') {
		motion += camera3d_->getRight() * speed_;		
	}
	if (event.GetKeyCode() == 'W') {
		motion = camera3d_->getFront() * speed_;
	}
	if (event.GetKeyCode() == 'S') {
		motion = camera3d_->getFront() * -speed_;		
	}
	if (event.GetKeyCode() == 'E') {
		motion = camera3d_->getTop() * speed_;
	}
	if (event.GetKeyCode() == 'Q') {
		motion = camera3d_->getTop() * -speed_;		
	}
	if (event.GetKeyCode() ==  WXK_NUMPAD2) {
		// view from behind
		aqua::PbVector3 viewDirection(0.0f, 0.0f, -1.0f);
		focusOnSceneExtents(viewDirection);		
	}
	if (event.GetKeyCode() ==  WXK_NUMPAD8) {
		// view from front
		aqua::PbVector3 viewDirection(0.0f, 0.0f, 1.0f);		
		focusOnSceneExtents(viewDirection);		
	}
	if (event.GetKeyCode() == WXK_NUMPAD6) {
		// view from right
		aqua::PbVector3 viewDirection(1.0f, 0.0f, 0.0f);		
		focusOnSceneExtents(viewDirection);		
	}
	if (event.GetKeyCode() == WXK_NUMPAD4) {		
		// view from left
		aqua::PbVector3 viewDirection(-1.0f, 0.0f, 0.0f);				
		focusOnSceneExtents(viewDirection);		
	}
	
	aqua::PbVector3 newPosition(camera3d_->getPosition());
	newPosition += motion;
	camera3d_->setPosition(newPosition);
}

void RenderPanel::OnMouseMotion(wxMouseEvent& squeak)
{
	static wxCoord lastX = 0;
	static wxCoord lastY = 0;
	static bool	   inDrag = false;
	if (squeak.Dragging()) {
		if (!inDrag) {
			lastX = squeak.GetX();
			lastY = squeak.GetY();
			inDrag = true;
		} else {
			aqua::Int32 deltaX = squeak.GetX() - lastX;
			aqua::Int32 deltaY = squeak.GetY() - lastY;
			camera3d_->rotateY((3.14f / 360.0f) * speed_ * deltaX);
			camera3d_->rotateX((3.14f / 360.0f) * -speed_ * deltaY);
			lastX = squeak.GetX();
			lastY = squeak.GetY();
		}
	} else {
		inDrag = false;		
	}
}

void RenderPanel::OnMouseWheel(wxMouseEvent& event)
{
	const aqua::PbVector3 currentCameraPosition(camera3d_->getPosition());
	aqua::PbVector3 newCameraPosition(currentCameraPosition);
	if (!event.AltDown()) {
		float delta_speed = 1.0f;
		if (event.ControlDown())	{
			delta_speed = 0.1f;
		}
		if (event.ShiftDown())	{
			delta_speed = 2.0f;
		}
		float moveZ = ((float)event.GetWheelRotation() / 120.0f) * delta_speed * CAMERA_SPEED_ZOOM;
		//Get the front vector of the current Graphics Camera
		aqua::PbVector3 cameraFront(camera3d_->getFront());
		cameraFront.normalize();
		newCameraPosition += (cameraFront * moveZ);
		aqua::PbVector3 newCameraFront(cameraFront);
		camera3d_->setPosition(newCameraPosition);
	} else {
		const aqua::PbReal wheelZoomAmount(((float)event.GetWheelRotation() / 120.0F) / 10.0F);
		// get field of view -----------------------------------------------------------------------------
		aqua::PbReal newFov(camera3d_->getFieldOfView());
		newFov -= CAMERA_SPEED_ZOOM * wheelZoomAmount;
		camera3d_->setFieldOfView(newFov);
	}
}

void RenderPanel::OnDestroy(wxWindowDestroyEvent& bang)
{	
	PBUNREFERENCED_PARAMETER(bang);
	created_ = false;
	timer_->Stop();
	Disconnect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(RenderPanel::OnRenderTimer));
}

