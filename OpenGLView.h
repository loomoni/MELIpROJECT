
// OpenGLView.h : interface of the COpenGLView class
//
// You should have already added these lines.
#include "gl\gl.h"
#include "gl\glu.h"


#pragma once


class COpenGLView : public CView
{
protected: // create from serialization only
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)

// Attributes
public:
	COpenGLDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~COpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// You will add the following stuff!!!

	virtual BOOL SetupPixelFormat(void);
	virtual BOOL SetupViewport(int cx, int cy);
	virtual BOOL SetupViewingFrustum(GLdouble aspect_ratio);
	virtual BOOL SetupViewingTransform(void);
	virtual BOOL PreRenderScene(void) { return TRUE; }
	virtual void RenderStockScene(void);
	virtual BOOL RenderScene(void);

private:

	BOOL InitializeOpenGL();
	void SetError(int e);

	HGLRC     m_hRC;
	CDC*      m_pDC;

	static const char* const _ErrorStrings[];
	const char* m_ErrorString;
	GLfloat   sceneRotate;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in OpenGLView.cpp
inline COpenGLDoc* COpenGLView::GetDocument() const
   { return reinterpret_cast<COpenGLDoc*>(m_pDocument); }
#endif

