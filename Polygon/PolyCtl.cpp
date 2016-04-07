// PolyCtl.cpp : Implementation of CPolyCtl
#include "stdafx.h"
#include "PolyCtl.h"


// CPolyCtl, OnDraw
HRESULT CPolyCtl::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	HDC hdc = di.hdcDraw;

	ATLTRACE(_T("OnDraw left:%d, top:%d, right:%d bottom:%d\n"), rc.left, rc.top, rc.right, rc.bottom);

	COLORREF    colFore;
	HBRUSH      hOldBrush, hBrush;
	HPEN        hOldPen, hPen;

	// Translate m_colFore into a COLORREF type
	OleTranslateColor(m_clrFillColor, NULL, &colFore);

	// Create and select the colors to draw the circle
	hPen = (HPEN)GetStockObject(BLACK_PEN);
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);

	// Create and select the brush that will be used to fill the polygon
	hBrush = CreateSolidBrush(colFore);
	SelectObject(hdc, hBrush);

	CalcPoints(rc);
	Polygon(hdc, &m_arrPolyPoint[0], m_nPolySides);

	// Select back the old pen and brush and delete the brush we created
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);


	DeleteObject(hBrush);



	return S_OK;
}


STDMETHODIMP CPolyCtl::get_Sides(SHORT* pVal)
{
	*pVal = m_nPolySides;

	return S_OK;
}


STDMETHODIMP CPolyCtl::put_Sides(SHORT newVal)
{
	if (2 < newVal && newVal < 101)
	{
		m_nPolySides = newVal;

		FireViewChange();

		return S_OK;
	}
	else
	{
		return Error(_T("Shape must have between 3 and 100 sides"));
	}
}


void CPolyCtl::CalcPoints(const RECT& rc)
{
	const double pi = 3.14159265358979;
	POINT   ptPolyCenter;
	POINT   ptTriangleCenter;

	// The radius is the hypotenuse 
	double  dblRadiusx = (rc.right - rc.left) / 4;
	double  dblRadiusy = (rc.bottom - rc.top) / 2;

	// angle is in radians, or radius lengths. The Y coordinates
	// are inverted on windows so top of circle is 3/2 PI instead
	// of PI/2.
	double  dblAngle = 3 * pi / 2;          // Start at the top
	double  dblDiff = 2 * pi / m_nPolySides;   // Angle each side will make

	ptPolyCenter.x = (rc.left + rc.right) / 4;
	ptPolyCenter.y = (rc.top + rc.bottom) / 2;

	ptTriangleCenter.x = (rc.left + rc.right) / 4 * 3;
	ptTriangleCenter.y = (rc.top + rc.bottom) / 2;

	ATLTRACE(_T("Polygon Center(x:%d, y : %d)\n"),ptPolyCenter.x,ptPolyCenter.y);
	ATLTRACE(_T("Triangle Center(x:%d, y : %d)\n"), ptTriangleCenter.x, ptTriangleCenter.y);

	// Calculate the points for each side
	int i = 0;
	for (; i < m_nPolySides; i++)
	{
	
		m_arrPolyPoint[i].x = (long)(dblRadiusx * cos(dblAngle) + ptPolyCenter.x + 0.5);
		m_arrPolyPoint[i].y = (long)(dblRadiusy * sin(dblAngle) + ptPolyCenter.y + 0.5);

		// Center point is the same for each triangle.
		m_arrTriangles[i].x = ptTriangleCenter.x;
		m_arrTriangles[i].y = ptTriangleCenter.y;

		// Second point in triangle
		m_arrTriangles[i + 1].x = (long)(dblRadiusx * cos(dblAngle) + ptTriangleCenter.x + 0.5);
		m_arrTriangles[i + 1].y = (long)(dblRadiusy * sin(dblAngle) + ptTriangleCenter.y + 0.5);

		// When executing in the Visual C++ Debugger the output isn't directed to DebugView.
		ATLTRACE(_T("X: %d Y: %d Angle in radians: %f\n"), m_arrPolyPoint[i].x, m_arrPolyPoint[i].y, dblAngle);

		dblAngle += dblDiff;

		// Third point in triangle.
		 = (long)(dblRadiusx * cos(dblAngle) + ptTriangleCenter.x + 0.5);
		m_arrTriangles[i + 2].y = (long)(dblRadiusy * sin(dblAngle) + ptTriangleCenter.y + 0.5);

		ATLTRACE(_T("Triangle Coordinates (%d,%d) (%d,%d) (%d,%d)\n"), 
			m_arrTriangles[i].x, m_arrTriangles[i].y,
			m_arrTriangles[i + 1].x, m_arrTriangles[i + 1].y,
			m_arrTriangles[i + 2].x, m_arrTriangles[i + 2].y);

		
	}
}
