// PolyCtl.cpp : Implementation of CPolyCtl
#include "stdafx.h"
#include "PolyCtl.h"

void CPolyCtl::DrawRightAngleGlyphs(HDC hdc)
{

	for (int i = 0; i < m_nTriangles * 3; i+=3)
	{
		double xslope =(float)(m_arrTriangles[i].x - m_arrTriangles[i + 2].x) / (float)(m_arrTriangles[i].y - m_arrTriangles[i + 2].y);
		int xsize = (int)round(xslope * 15.0);

		double yslope = (float)(m_arrTriangles[i].y - m_arrTriangles[i + 2].y) / (float)(m_arrTriangles[i].x - m_arrTriangles[i + 2].x);
		int ysize = (int)round(yslope * 15.0);

		// Determine how to best display the glyph. 

		
	}


}

static POINT DistanceFromPoint(int x1, int y1, int x2, int y2, int n)
{
	POINT a;

	int d = sqrt((x2 - x1) ^ 2 + (y2 - y1) ^ 2);
	int r = n / d;

	a.x = r * x2 + (1 - r) * x1;
	a.y = r * y2 + (1 - r) * y1; 

	return a;
}


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

	Ellipse(hdc, rc.left, rc.top, rc.right/2, rc.bottom);
	Ellipse(hdc, rc.left + (rc.right / 2), rc.top, rc.right, rc.bottom);

	// Create and select the brush that will be used to fill the polygon
	hBrush = CreateSolidBrush(colFore);
	SelectObject(hdc, hBrush);

	CalcPoints(rc);
	Polygon(hdc, &m_arrPolyPoint[0], m_nPolySides);

	// Select back the old pen and brush and delete the brush we created
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);

	PolyPolygon(hdc, &m_arrTriangles[0], &m_nTrianglePoints[0], m_nTriangles);
	// Create the right angle glyphs. 
	DrawRightAngleGlyphs(hdc);

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
	m_nTriangles = 0;
	int idxTrianglePts = 0;

	for (; i < m_nPolySides; i++)
	{
	
		m_arrPolyPoint[i].x = (long)(dblRadiusx * cos(dblAngle) + ptPolyCenter.x + 0.5);
		m_arrPolyPoint[i].y = (long)(dblRadiusy * sin(dblAngle) + ptPolyCenter.y + 0.5);

		// Center point is the same for each triangle.
		m_arrTriangles[idxTrianglePts].x = ptTriangleCenter.x;
		m_arrTriangles[idxTrianglePts].y = ptTriangleCenter.y;

		// Second point in triangle
		m_arrTriangles[idxTrianglePts + 1].x = (long)(dblRadiusx * cos(dblAngle) + ptTriangleCenter.x + 0.5);
		m_arrTriangles[idxTrianglePts + 1].y = (long)(dblRadiusy * sin(dblAngle) + ptTriangleCenter.y + 0.5);

		// When executing in the Visual C++ Debugger the output isn't directed to DebugView.
		ATLTRACE(_T("X: %d Y: %d Angle in radians: %f\n"), m_arrPolyPoint[i].x, m_arrPolyPoint[i].y, dblAngle);

		dblAngle += dblDiff;

		// Third point in triangle.
		POINT temp;
		temp.x = (long)(dblRadiusx*cos(dblAngle) + ptTriangleCenter.x + 0.5);
		temp.y = (long)(dblRadiusy*sin(dblAngle) + ptTriangleCenter.y + 0.5);

		m_arrTriangles[idxTrianglePts + 2].x = m_arrTriangles[idxTrianglePts + 1].x + (temp.x - m_arrTriangles[idxTrianglePts + 1].x) / 2;
		m_arrTriangles[idxTrianglePts + 2].y = m_arrTriangles[idxTrianglePts + 1].y + (temp.y - m_arrTriangles[idxTrianglePts + 1].y) / 2;

		m_nTriangles++;
		m_nTrianglePoints[i] = 3;

		ATLTRACE(_T("Triangle (%d,%d) (%d,%d) (%d,%d)\n"), 
			m_arrTriangles[idxTrianglePts].x, m_arrTriangles[idxTrianglePts].y,
			m_arrTriangles[idxTrianglePts + 1].x, m_arrTriangles[idxTrianglePts + 1].y,
			m_arrTriangles[idxTrianglePts + 2].x, m_arrTriangles[idxTrianglePts + 2].y);


		idxTrianglePts = idxTrianglePts + m_nTrianglePoints[i];
	}
}
