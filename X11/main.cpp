#include <unistd.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "Renderer.h"

// Name of the application
const char* ApplicationName       = "Simple Triangle";

// Width and height of the window
const unsigned int WindowWidth     = 1024;
const unsigned int WindowHeight    = 768;

/*!*********************************************************************************************************************
\param[in]			functionLastCalled          Function which triggered the error
\return		True if no EGL error was detected
\brief	Tests for an EGL error and prints it.
***********************************************************************************************************************/
bool testEGLError(const char* functionLastCalled)
{
	//	eglGetError returns the last error that occurred using EGL, not necessarily the status of the last called function. The user has to
	//	check after every single EGL call or at least once every frame. Usually this would be for debugging only, but for this example
	//	it is enabled always.
	EGLint lastError = eglGetError();
	if (lastError != EGL_SUCCESS)
	{
		std::cerr<<functionLastCalled <<" failed "<<lastError<<std::endl;
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			nativeDisplay               Handle to the display
\param[in]			error                       The error event to handle
\return		Result code to send to the X window system
\brief	Processes event messages for the main window
***********************************************************************************************************************/
int handleX11Errors(Display* nativeDisplay, XErrorEvent* error)
{
	// Get the X Error
	char errorStringBuffer[256];
	XGetErrorText(nativeDisplay, error->error_code, errorStringBuffer, 256);

	// Print the error
	std::cerr<<errorStringBuffer<<std::endl;

	// Exit the application
	exit(-1);
	return 0;
}

/*!*********************************************************************************************************************
\param[in]			functionLastCalled          Function which triggered the error
\return		True if no GL error was detected
\brief	Tests for an GL error and prints it in a message box.
***********************************************************************************************************************/
bool testGLError(const char* functionLastCalled)
{
	//	glGetError returns the last error that occurred using OpenGL ES, not necessarily the status of the last called function. The user
	//	has to check after every single OpenGL ES call or at least once every frame. Usually this would be for debugging only, but for this
	//	example it is enabled always
	GLenum lastError = glGetError();
	if (lastError != GL_NO_ERROR)
	{
		std::cerr<<functionLastCalled<<" failed "<<lastError<<std::endl;
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param[out]		nativeDisplay				Native display to create
\return		Whether the function succeeded or not.
\brief	Creates a native isplay for the application to render into.
***********************************************************************************************************************/
bool createNativeDisplay(Display** nativeDisplay)
{
	// Check for a valid display
	if (!nativeDisplay)	{	return false;	}

	// Open the display
	*nativeDisplay = XOpenDisplay(0);
	if (!*nativeDisplay)
	{
		std::cerr<<"Error: Unable to open X display"<<std::endl;
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			nativeDisplay				Native display used by the application
\param[out]		nativeWindow			    Native window type to create
\return		Whether the function succeeded or not.
\brief	Creates a native window for the application to render into.
***********************************************************************************************************************/
bool createNativeWindow(Display* nativeDisplay, Window* nativeWindow)
{
	// Get the default screen for the display
	int defaultScreen = XDefaultScreen(nativeDisplay);

	// Get the default depth of the display
	int defaultDepth = DefaultDepth(nativeDisplay, defaultScreen);

	// Select a visual info

	XVisualInfo* visualInfo = new XVisualInfo();

	if (!XMatchVisualInfo(nativeDisplay, defaultScreen, defaultDepth, TrueColor, visualInfo))
	{
		std::cerr<<"Error: Unable to acquire visual"<<std::endl;
		delete visualInfo;
		return false;
	}

	// Get the root window for the display and default screen
	Window rootWindow = RootWindow(nativeDisplay, defaultScreen);

	// Create a color map from the display, root window and visual info
	Colormap colorMap = XCreateColormap(nativeDisplay, rootWindow, visualInfo->visual, AllocNone);

	// Now setup the final window by specifying some attributes
	XSetWindowAttributes windowAttributes;

	// Set the color map that was just created
	windowAttributes.colormap = colorMap;

	// Set events that will be handled by the app, add to these for other events.
	windowAttributes.event_mask = StructureNotifyMask | ExposureMask | ButtonPressMask;

	// Create the window
	*nativeWindow = XCreateWindow(nativeDisplay,              // The display used to create the window
	                              rootWindow,                   // The parent (root) window - the desktop
	                              0,                            // The horizontal (x) origin of the window
	                              0,                            // The vertical (y) origin of the window
	                              WindowWidth,                 // The width of the window
	                              WindowHeight,                // The height of the window
	                              0,                            // Border size - set it to zero
	                              visualInfo->depth,            // Depth from the visual info
	                              InputOutput,                  // Window type - this specifies InputOutput.
	                              visualInfo->visual,           // Visual to use
	                              CWEventMask | CWColormap,     // Mask specifying these have been defined in the window attributes
	                              &windowAttributes);           // Pointer to the window attribute structure

	// Make the window viewable by mapping it to the display
	XMapWindow(nativeDisplay, *nativeWindow);

	// Set the window title
	XStoreName(nativeDisplay, *nativeWindow, ApplicationName);

	// Setup the window manager protocols to handle window deletion events
	Atom windowManagerDelete = XInternAtom(nativeDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(nativeDisplay, *nativeWindow, &windowManagerDelete , 1);
	delete visualInfo;
	return true;
}

/*!*********************************************************************************************************************
\param[in]		nativeDisplay               The native display used by the application
\param[out]		eglDisplay				    EGLDisplay created from nativeDisplay
\return		Whether the function succeeded or not.
\brief	Creates an EGLDisplay from a native native display, and initializes it.
***********************************************************************************************************************/
bool createEGLDisplay(Display* nativeDisplay, EGLDisplay& eglDisplay)
{
	//	Get an EGL display.
	//	EGL uses the concept of a "display" which in most environments corresponds to a single physical screen. After creating a native
	//	display for a given windowing system, EGL can use this handle to get a corresponding EGLDisplay handle to it for use in rendering.
	//	Should this fail, EGL is usually able to provide access to a default display.
	eglDisplay = eglGetDisplay((EGLNativeDisplayType)nativeDisplay);
	// If a display couldn't be obtained, return an error.
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		std::cerr<<"Failed to get an EGLDisplay"<<std::endl;
		return false;
	}

	//	Initialize EGL.
	//	EGL has to be initialized with the display obtained in the previous step. All EGL functions other than eglGetDisplay
	//	and eglGetError need an initialized EGLDisplay.
	//	If an application is not interested in the EGL version number it can just pass NULL for the second and third parameters, but they
	//	are queried here for illustration purposes.
	EGLint eglMajorVersion, eglMinorVersion;
	if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion))
	{
		std::cerr<<"Failed to initialize the EGLDisplay"<<std::endl;
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			eglDisplay                  The EGLDisplay used by the application
\param[out]		eglConfig                   The EGLConfig chosen by the function
\return		Whether the function succeeded or not.
\brief	Chooses an appropriate EGLConfig and return it.
***********************************************************************************************************************/
bool chooseEGLConfig(EGLDisplay eglDisplay, EGLConfig& eglConfig)
{
	//	Specify the required configuration attributes.
	//	An EGL "configuration" describes the capabilities an application requires and the type of surfaces that can be used for drawing.
	//	Each implementation exposes a number of different configurations, and an application needs to describe to EGL what capabilities it
	//	requires so that an appropriate one can be chosen. The first step in doing this is to create an attribute list, which is an array
	//	of key/value pairs which describe particular capabilities requested. In this application nothing special is required so we can query
	//	the minimum of needing it to render to a window, and being OpenGL ES 2.0 capable.
	const EGLint configurationAttributes[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	//	Find a suitable EGLConfig
	//	eglChooseConfig is provided by EGL to provide an easy way to select an appropriate configuration. It takes in the capabilities
	//	specified in the attribute list, and returns a list of available configurations that match or exceed the capabilities requested.
	//	Details of all the possible attributes and how they are selected for by this function are available in the EGL reference pages here:
	//	http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglChooseConfig.html
	//	It is also possible to simply get the entire list of configurations and use a custom algorithm to choose a suitable one, as many
	//	advanced applications choose to do. For this application however, taking the first EGLConfig that the function returns suits
	//	its needs perfectly, so we limit it to returning a single EGLConfig.
	EGLint configsReturned;
	if (!eglChooseConfig(eglDisplay, configurationAttributes, &eglConfig, 1, &configsReturned) || (configsReturned != 1))
	{
		std::cerr<<"Failed to choose a suitable config."<<std::endl;
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			nativeWindow                A native window that's been created
\param[in]			eglDisplay                  The EGLDisplay used by the application
\param[in]			eglConfig                   An EGLConfig chosen by the application
\param[out]		eglSurface					The EGLSurface created from the native window.
\return		Whether the function succeeds or not.
\brief	Creates an EGLSurface from a native window
***********************************************************************************************************************/
bool createEGLSurface(Window nativeWindow, EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface& eglSurface)
{
	//	Create an EGLSurface for rendering.
	//	Using a native window created earlier and a suitable eglConfig, a surface is created that can be used to render OpenGL ES calls to.
	//	There are three main surface types in EGL, which can all be used in the same way once created but work slightly differently:
	//	 - Window Surfaces  - These are created from a native window and are drawn to the screen.
	//	 - Pixmap Surfaces  - These are created from a native windowing system as well, but are offscreen and are not displayed to the user.
	//	 - PBuffer Surfaces - These are created directly within EGL, and like Pixmap Surfaces are offscreen and thus not displayed.
	//	The offscreen surfaces are useful for non-rendering contexts and in certain other scenarios, but for most applications the main
	//	surface used will be a window surface as performed below.
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)nativeWindow, NULL);
	if (!testEGLError("eglCreateWindowSurface")){	return false;	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			eglDisplay                  The EGLDisplay used by the application
\param[in]			eglConfig                   An EGLConfig chosen by the application
\param[in]			eglSurface					The EGLSurface created from the native window.
\param[out]		eglContext                  The EGLContext created by this function
\param[in]			nativeWindow                A native window, used to display error messages
\return		Whether the function succeeds or not.
\brief	Sets up the EGLContext, creating it and then installing it to the current thread.
***********************************************************************************************************************/
bool setupEGLContext(EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface eglSurface, EGLContext& eglContext)
{
	//	Make OpenGL ES the current API.
	// EGL needs a way to know that any subsequent EGL calls are going to be affecting OpenGL ES,
	// rather than any other API (such as OpenVG).
	eglBindAPI(EGL_OPENGL_ES_API);
	if (!testEGLError("eglBindAPI")){	return false;	}

	//	Create a context.
	//	EGL has to create what is known as a context for OpenGL ES. The concept of a context is OpenGL ES's way of encapsulating any
	//	resources and state. What appear to be "global" functions in OpenGL actually only operate on the current context. A context
	//	is required for any operations in OpenGL ES.
	//	Similar to an EGLConfig, a context takes in a list of attributes specifying some of its capabilities. However in most cases this
	//	is limited to just requiring the version of the OpenGL ES context required - In this case, OpenGL ES 2.0.
	EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	// Create the context with the context attributes supplied
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttributes);
	if (!testEGLError("eglCreateContext")){	return false;	}

	//	Bind the context to the current thread.
	//	Due to the way OpenGL uses global functions, contexts need to be made current so that any function call can operate on the correct
	//	context. Specifically, make current will bind the context to the thread it's called from, and unbind it from any others. To use
	//	multiple contexts at the same time, users should use multiple threads and synchronise between them.
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!testEGLError("eglMakeCurrent")){	return false;	}
	return true;
}

/*!*********************************************************************************************************************
\param[in]			eglDisplay                   The EGLDisplay used by the application
\brief	Releases all resources allocated by EGL
***********************************************************************************************************************/
void releaseEGLState(EGLDisplay eglDisplay)
{
	if (eglDisplay != NULL)
	{
		// To release the resources in the context, first the context has to be released from its binding with the current thread.
		eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		// Terminate the display, and any resources associated with it (including the EGLContext)
		eglTerminate(eglDisplay);
	}
}

/*!*********************************************************************************************************************
\param[in]			nativeDisplay               The native display to release
\param[in]			nativeWindow                The native window to destroy
\brief	Releases all resources allocated by the windowing system
***********************************************************************************************************************/
void releaseNativeResources(Display* nativeDisplay, Window nativeWindow)
{
	// Destroy the window
	if (nativeWindow){	XDestroyWindow(nativeDisplay, nativeWindow);	}

	// Release the display.
	if (nativeDisplay){	XCloseDisplay(nativeDisplay);	}
}

bool renderScene(Renderer *renderer, EGLDisplay eglDisplay, EGLSurface eglSurface, Display* nativeDisplay)
{
	//renderer.DrawFrame();
	//	Present the display data to the screen.
	//	When rendering to a Window surface, OpenGL ES is double buffered. This means that OpenGL ES renders directly to one frame buffer,
	//	known as the back buffer, whilst the display reads from another - the front buffer. eglSwapBuffers signals to the windowing system
	//	that OpenGL ES 2.0 has finished rendering a scene, and that the display should now draw to the screen from the new data. At the same
	//	time, the front buffer is made available for OpenGL ES 2.0 to start rendering to. In effect, this call swaps the front and back
	//	buffers.
	renderer->DrawFrame();

	if (!eglSwapBuffers(eglDisplay, eglSurface))
	{
		testEGLError("eglSwapBuffers");
		return false;
	}

	// Check for messages from the windowing system.
	int numberOfMessages = XPending(nativeDisplay);
	for (int i = 0; i < numberOfMessages; i++)
	{
		XEvent event;
		XNextEvent(nativeDisplay, &event);

		switch (event.type)
		{
		// Exit on window close
		case ClientMessage:
		// Exit on mouse click
		case ButtonPress:
		case DestroyNotify:
			return false;
		default:
			break;
		}
	}
	return true;

}

int main(int /*argc*/, char** /*argv*/)
{

	// X11 variables
	Display* nativeDisplay = NULL;
	Window nativeWindow = 0;

	// EGL variables
	EGLDisplay			eglDisplay = NULL;
	EGLConfig			eglConfig = NULL;
	EGLSurface			eglSurface = NULL;
	EGLContext			eglContext = NULL;
	Renderer *renderer = NULL;
	// Get access to a native display
	if (!createNativeDisplay(&nativeDisplay)){ goto cleanup;	}

	// Setup the windowing system, create a window
	if (!createNativeWindow(nativeDisplay, &nativeWindow)){	goto cleanup;	}

	// Create and Initialize an EGLDisplay from the native display
	if (!createEGLDisplay(nativeDisplay, eglDisplay)){ goto cleanup;	}

	// Choose an EGLConfig for the application, used when setting up the rendering surface and EGLContext
	if (!chooseEGLConfig(eglDisplay, eglConfig)){ goto cleanup;	}

	// Create an EGLSurface for rendering from the native window
	if (!createEGLSurface(nativeWindow, eglDisplay, eglConfig, eglSurface))
	{
		goto cleanup;
	}

	// Setup the EGL Context from the other EGL constructs created so far, so that the application is ready to submit OpenGL ES commands
	if (!setupEGLContext(eglDisplay, eglConfig, eglSurface, eglContext))
	{
		goto cleanup;
	}

	renderer = new Renderer();

	renderer->InitializeGl();
	renderer->SetViewport(WindowWidth,WindowHeight);

	while (renderScene(renderer, eglDisplay, eglSurface, nativeDisplay))
	{
	}

	renderer->ReleaseGl();

cleanup:
	if (renderer != NULL)
	{
		delete renderer;
	}
	// Release the EGL State
	releaseEGLState(eglDisplay);

	// Release the windowing system resources
	releaseNativeResources(nativeDisplay, nativeWindow);

	// Destroy the eglWindow
	return 0;
}
