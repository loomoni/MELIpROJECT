
// OpenGLView.cpp : implementation of the COpenGLView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "OpenGL.h"
#endif

#include "OpenGLDoc.h"
#include "OpenGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// You will add stuff here!!!!
const char* const COpenGLView::_ErrorStrings[] = {
	{ "No Error" },                     // 0
	{ "Unable to get a DC" },           // 1
	{ "ChoosePixelFormat failed" },     // 2
	{ "SelectPixelFormat failed" },     // 3
	{ "wglCreateContext failed" },      // 4
	{ "wglMakeCurrent failed" },        // 5
	{ "wglDeleteContext failed" },      // 6
	{ "SwapBuffers failed" },           // 7
};


// COpenGLView construction/destruction

COpenGLView::COpenGLView()
// You will add the following line !!!
	: m_hRC(0), m_pDC(0), m_ErrorString(_ErrorStrings[0]) // Call constructors

{
	// TODO: add construction code here

}

COpenGLView::~COpenGLView()
{
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	// You will add stuff here !!!
	// An OpenGL window must be created with the following flags and must not
	// include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;


	return CView::PreCreateWindow(cs);
}

// COpenGLView drawing

void COpenGLView::OnDraw(CDC* pDC)
{
	COpenGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	// You are to add a lot stuff here !!!!!!

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	PreRenderScene();

	::glPushMatrix();
	RenderStockScene();
	::glPopMatrix();

	::glPushMatrix();
	RenderScene();
	::glPopMatrix();

	::glFinish();

	if (FALSE == ::SwapBuffers(m_pDC->GetSafeHdc()))
	{
		SetError(7);
	}

}


// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLDoc* COpenGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLDoc)));
	return (COpenGLDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenGLView message handlers


int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	// You will add stuff here!!!
	InitializeOpenGL();


	return 0;
}


void COpenGLView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	// You will add some stuff here!!!!
	if (FALSE == ::wglDeleteContext(m_hRC))
	{
		SetError(6);
	}

	if (m_pDC)
	{
		delete m_pDC;
	}

}


BOOL COpenGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	// You are to modify the following line!!!!
	//sceneRotate += 5;


	return CView::OnEraseBkgnd(pDC);
}


void COpenGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	// You are to add a lot of stuff here !!!!
	GLdouble aspect_ratio; // width/height ratio

	if (0 >= cx || 0 >= cy)
	{
		return;
	}


	SetupViewport(cx, cy);

	// select the projection matrix and clear it
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();

	// compute the aspect ratio
	// this will keep all dimension scales equal
	aspect_ratio = (GLdouble)cx / (GLdouble)cy;

	// select the viewing volumn
	SetupViewingFrustum(aspect_ratio);

	// switch back to the modelview matrix
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

	// now perform any viewing transformations
	SetupViewingTransform();

}

/////////////////////////////////////////////////////////////////////////////
// GL helper functions
// You are to add all new functions here !!!!!!!!!!
// They include:
//    void COpenGLView::SetError( int e )
//    BOOL COpenGLView::InitializeOpenGL()
//    BOOL COpenGLView::SetupPixelFormat()
//    BOOL COpenGLView::SetupViewport( int cx, int cy )
//    BOOL COpenGLView::SetupViewingFrustum( GLdouble aspect_ratio )
//    BOOL COpenGLView::SetupViewingTransform()
//    BOOL COpenGLView::RenderScene()
//    void COpenGLView::RenderStockScene()

void COpenGLView::SetError(int e)
{
	// if there was no previous error,
	// then save this one
	if (_ErrorStrings[0] == m_ErrorString)
	{
		m_ErrorString = _ErrorStrings[e];
	}
}

BOOL COpenGLView::InitializeOpenGL()
{
	// Can we put this in the constructor?
	m_pDC = new CClientDC(this);

	if (NULL == m_pDC) // failure to get DC
	{
		SetError(1);
		return FALSE;
	}

	if (!SetupPixelFormat())
	{
		return FALSE;
	}

	//n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	//::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	//  CreateRGBPalette();

	if (0 == (m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc())))
	{
		SetError(4);
		return FALSE;
	}

	if (FALSE == ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
	{
		SetError(5);
		return FALSE;
	}

	// specify black as clear color
	::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// specify the back of the buffer as clear depth
	::glClearDepth(1.0f);
	// enable depth testing
	::glEnable(GL_DEPTH_TEST);

	sceneRotate = 20.0;

	return TRUE;
}

BOOL COpenGLView::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |          // support OpenGL
		PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
										// 32,                          // 32-bit z-buffer
		16,    // NOTE: better performance with 16-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int pixelformat;

	if (0 == (pixelformat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)))
	{
		SetError(2);
		return FALSE;
	}

	if (FALSE == ::SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd))
	{
		SetError(3);
		return FALSE;
	}

	return TRUE;
}

BOOL COpenGLView::SetupViewport(int cx, int cy)
{
	// select the full client area
	::glViewport(0, 0, cx, cy);

	return TRUE;
}

BOOL COpenGLView::SetupViewingFrustum(GLdouble aspect_ratio)
{
	// select a default viewing volumn
	::gluPerspective(40.0f, aspect_ratio, .1f, 20.0f);
	return TRUE;
}


BOOL COpenGLView::SetupViewingTransform()
{
	// select a default viewing transformation
	// of a 20 degree rotation about the X axis
	// then a -5 unit transformation along Z
	::glTranslatef(0.0f, 0.0f, -5.0f);
	::glRotatef(sceneRotate, 1.0f, 0.0f, 0.0f);
	return TRUE;
}


BOOL COpenGLView::RenderScene()
{
	// draw a red wire sphere inside a
	// light blue cone (cylinder with one raduis set to 0)

	//Quadrics are part of glu library that make parameterized shapes
	/*GLUquadricObj *qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_LINE); /* wireframe */

										 // rotate the wire sphere so it's vertically
										 // oriented
	/*::glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	::glColor3f(1.0f, 0.0f, 0.0f);
	::gluSphere(qobj, 0.5, 20, 10);

	::glColor3f(0.5f, 0.5f, 1.0f);
	::glTranslatef(0, 0, -1.0); /* Try to align cylinder "up" on Z axis */
	//::gluCylinder(qobj, 0.0, 1.0f, 1.5f, 20, 5);
	//glcolor3f()

	/*glBegin(GL_POLYGON);
	glVertex2f(0.0f, 2.0f); //note 2D form
	glVertex2f(1.0f, 2.0f);
	glVertex2f(0.0f, -2.0f);
	glVertex2f(-1.0f, 0.0f);
	glEnd();*/

	/*glBegin(GL_QUADS);

	glVertex2f(0.0f, 0.0f); //note 2D form
	glVertex2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.0f, 1.0f);

	glEnd();*/
	
	/*double hd = 3.0, hv = 2.0, displ = 1.0;
	double P1[2] = { 0,0 };
	double P2[2] = { hd, 0 };
	double P3[2] = { 0, hv };
	double P4[2] = { hd + displ, hv };

	 P1[2], P2[2], P3[2], P4[2];
	P1[0] = 0.0; P1[1] = 0.0;

	glBegin(GL_POLYGON);
	glVertex2dv(P1); //note 2D form
	glVertex2dv(P2);
	glVertex2dv(P3);
	glVertex2dv(P4);
	glEnd();
	*/
	/*glBegin(GL_LINE_LOOP);  //make it a connected close line segment
	glVertex2f(1.0f, 2.0f); //note 2D form
	glVertex2f(2.0f, 2.0f);
	glVertex2f(1.0f, -2.0f);
	glVertex2f(1.0f, 0.0f);
	glEnd();8*/

	//drawing of the low down line
	glBegin(GL_LINES);  //make it a connected close line segment
	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(2.5, -1.5);
	glEnd();

	//drawing of fowarding slash line
	glBegin(GL_LINES);  //make it a connected close line segment
	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(-2.5, 0.2);
	glEnd();

	glBegin(GL_LINES); 
	glVertex2f(-2.5, 0.2);//make it a connected close line segment
	glVertex2f(-2.0, 0.2); //note 2D form
	glEnd();
	
	//the inner drop part
	glBegin(GL_LINES);
	glVertex2f(-2.0, 0.2);
	glVertex2f(-1.9, 0.1);//make it a connected close line segment
	glEnd();           //note 2D form
	
	//make the dipper place straight back to the ship
	glBegin(GL_LINES);
	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.9, 0.1);
	glEnd();

	//make it the line to come up
	glBegin(GL_LINES);
	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.3, 0.2);
	glEnd();

	//make a long line back straight it up
	glBegin(GL_LINES);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-1.3, 0.2);
	glEnd();

	//make the line to down dip to join with back straight line down
	glBegin(GL_LINES);
	//glColor3f(0.0, 1.0, 0.0);
	//glColor3f(0.0, 1.0, 2.0);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-0.2, 0.0);
	glEnd();

	//make the straight line down in the ship
	glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(-0.2, 0.0);
	glEnd();

	//make slash back straight up
	glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(2.3, 0.3);
	glEnd();

	//straight it back for the bending to be ready to go
	glBegin(GL_LINES);
//	glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.3, 0.3);
	glEnd();

	//make a line to connect the end back
	glBegin(GL_LINES);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.5, -1.5);
	glEnd();
// end of ship frame;

	// Building the back tower
	glBegin(GL_LINES);
	glVertex2f(2.5, 0.3);
	glVertex2f(2.5, 0.5);
	glEnd();

	//draw a line of upper first box
	glBegin(GL_LINES);
	glVertex2f(1.8, 0.5);
	glVertex2f(2.5, 0.5);
	glEnd();

	//left line of the first draft
	glBegin(GL_LINES);
	glVertex2f(1.8, 0.5);
	glVertex2f(1.8, 0.0);
	glEnd();
	//end of first tower

	//start of the uper tower
	//right line of the second layer of the tawer
	glBegin(GL_LINES);
	glVertex2f(2.3, 0.5);
	glVertex2f(2.3, 0.8);
	glEnd();
	
	//uper line of the second tower
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(2.3, 0.8);
	glVertex2f(1.9, 0.8);
	glEnd();

	//draw a complete line in the second layer
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(1.9, 0.8);
	glVertex2f(1.9, 0.5);
	glEnd();

	//the start of the third layer
	glBegin(GL_LINE_LOOP);
	
	glVertex2f(2.2, 0.8);
	glVertex2f(2.2, 1.0);
	glVertex2f(2.0, 1.0);
	glVertex2f(2.0, 0.8);
	glEnd();

	//building of the long at the back, starting
	glBegin(GL_LINES);
	glVertex2f(2.1, 1.0);
	glVertex2f(2.1, 1.6);
	glEnd();

	//build bendera at the top
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(2.1, 1.6);
	glVertex2f(2.3, 1.6);
	glVertex2f(2.3, 1.5);
	glVertex2f(2.1, 1.5);
	glEnd();


	
	glBegin(GL_POLYGON);

	glColor3f(1.0, 1.0, 0.0);

	//drawing of the low down line
	//glBegin(GL_LINES);  //make it a connected close line segment
	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(2.5, -1.5);
	//glEnd();

	//drawing of fowarding slash line
	//glBegin(GL_LINES);  //make it a connected close line segment
	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(-2.5, 0.2);
	//glEnd();

	//glBegin(GL_LINES);
	glVertex2f(-2.5, 0.2);//make it a connected close line segment
	glVertex2f(-2.0, 0.2); //note 2D form
	//glEnd();

	//the inner drop part
	//glBegin(GL_LINES);
	glVertex2f(-2.0, 0.2);
	glVertex2f(-1.9, 0.1);//make it a connected close line segment
	//glEnd();           //note 2D form

	//make the dipper place straight back to the ship
	//glBegin(GL_LINES);
	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.9, 0.1);
	//glEnd();
	//
	//make it the line to come up
	//glBegin(GL_LINES);
	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.3, 0.2);
	//glEnd();

	//make a long line back straight it up
	//glBegin(GL_LINES);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-1.3, 0.2);
	//glEnd();

	//make the line to down dip to join with back straight line down
	//(GL_LINES);
	//glColor3f(0.0, 1.0, 0.0);
	//glColor3f(0.0, 1.0, 2.0);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make the straight line down in the ship
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make slash back straight up
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//straight it back for the bending to be ready to go
	//glBegin(GL_LINES);
	//	glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//make a line to connect the end back
	//glBegin(GL_LINES);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.5, -1.5);
	//glEnd();
	glEnd();
	/*glColor3f(0.0, 0.0, 1.0);
	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(2.5, -1.5);

	glVertex2f(-2.0, -1.5); //note 2D form
	glVertex2f(-2.5, 0.2);

	glVertex2f(-2.5, 0.2);//make it a connected close line segment
	glVertex2f(-2.0, 0.2);

	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.9, 0.1);

	glVertex2f(-1.5, 0.1);
	glVertex2f(-1.9, 0.1);

    glVertex2f(-1.5, 0.1);
	glVertex2f(-1.3, 0.2);

	//make the line to down dip to join with back straight line down
	//glBegin(GL_LINES);
	//glColor3f(0.0, 1.0, 0.0);
	//glColor3f(0.0, 1.0, 2.0);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make the straight line down in the ship
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make slash back straight up
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//straight it back for the bending to be ready to go
	//glBegin(GL_LINES);
	//	glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//make a line to connect the end back
	//glBegin(GL_LINES);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.5, -1.5);

	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0, 1.0, 0.0);

	//make the line to down dip to join with back straight line down
	//glBegin(GL_LINES);
	//glColor3f(0.0, 1.0, 0.0);
	//glColor3f(0.0, 1.0, 2.0);
	glVertex2f(-0.5, 0.2);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make the straight line down in the ship
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(-0.2, 0.0);
	//glEnd();

	//make slash back straight up
	//glBegin(GL_LINES);
	//glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.0, 0.0);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//straight it back for the bending to be ready to go
	//glBegin(GL_LINES);
	//	glColor3f(0.0, 0.0, 2.0);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.3, 0.3);
	//glEnd();

	//make a line to connect the end back
	//glBegin(GL_LINES);
	glVertex2f(2.7, 0.3);
	glVertex2f(2.5, -1.5);
	//glEnd();
	glEnd();
/*glBegin(GL_QUADS);
//glColor3f(0.0, 1.0, 0.0);
glVertex2f(-1.0, -1.0);
glVertex2f(1.0, -1.0);
glVertex2f(1.0, 1.0);
glVertex2f(-1.0, 1.0);
*/


/*glBegin(GL_LINE_LOOP);
glColor3f(1.0, 0.0, 0.0);
glVertex2f(-1.0, -1.0);
glVertex2f(1.0, -1.0);
glVertex2f(1.0, 1.0);
glVertex2f(-1.0, 1.0);
glEnd();

glBegin(GL_LINE_LOOP);
glColor3f(0.0, 1.0, 0.0);
glVertex2f(-1.3, 1.0);
glVertex2f(1.3, 1.0);
glVertex2f(0.0, 1.5);
glEnd();

glBegin(GL_QUADS);
glColor3f(0.0, 0.0, 1.0);
glVertex2f(0.0, 0.5);
glVertex2f(0.5, 0.5);
glVertex2f(0.5, 0.7);
glVertex2f(0.0, 0.7);
glEnd();*/

/*glBegin(GL_POINTS);
glColor3f(0.0, 1.0, 0.0);
 glVertex2f(0.0, 2.0); //note 2D form	
glVertex2f(1.0, 2.0);
 glVertex2f(0.0, -2.0);
  glVertex2f(-1.0, 0.0);
glEnd();%*/

/*glBegin(GL_LINE_STRIP);
glColor3f(0.0, 1.0, 0.0);
glVertex2f(0.0, 0.0); //note 2D form	
glVertex2f(1.0, 0.0);
glVertex2f(0.5, 1.0);
glVertex2f(1.5, 0.0);
glVertex2f(1.5, 1.0);
//glVertex2f(-1.0, 0.0);
glEnd();*/




	return TRUE;

}

// Draw a square surface that looks like a
// black and white checkerboard
void COpenGLView::RenderStockScene()
{
	// define all vertices   X     Y     Z
	//GLfloat v0[3], v1[3], v2[3], v3[3], delta;
int color = 0;
	//glcolor3f()

	/*delta = 0.5f;

	// define the two colors
	GLfloat color1[3] = { 0.9f, 0.9f, 0.9f };
	GLfloat color2[3] = { 0.05f, 0.05f, 0.05f };

	v0[1] = v1[1] = v2[1] = v3[1] = 0.0f;

	::glBegin(GL_QUADS);

	for (int x = -5; x <= 5; x++)
	{
		for (int z = -5; z <= 5; z++)
		{
			::glColor3fv((color++) % 2 ? color1 : color2);

			v0[0] = 0.0f + delta*z;
			v0[2] = 0.0f + delta*x;

			v1[0] = v0[0] + delta;
			v1[2] = v0[2];

			v2[0] = v0[0] + delta;
			v2[2] = v0[2] + delta;

			v3[0] = v0[0];
			v3[2] = v0[2] + delta;

			::glVertex3fv(v0);
			::glVertex3fv(v1);
			::glVertex3fv(v2);
			::glVertex3fv(v3);
		}
	}
	::glEnd();*/
}
