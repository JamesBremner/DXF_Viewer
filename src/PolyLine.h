// PolyLine.h: interface for the CPolyLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYLINE_H__2845A691_54B0_40CE_9000_80D63B888975__INCLUDED_)
#define AFX_POLYLINE_H__2845A691_54B0_40CE_9000_80D63B888975__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPolyLine   : public cDXFGraphObject

{
public:
	UINT m_VertexCount,m_PolyLineFlag;
	double x[1024],y[1024];
	CString m_Layer;
	BYTE m_Select;
	BYTE m_Nest;

	CPolyLine();
	virtual ~CPolyLine();

	void Update( cBoundingRectangle& rect );


	bool Read( FILE * fp, int& code, char* value );

/**  get drawing parameters, scaled to display window, for the 'next' line

@param[in/out] draw  structure holding parameters, intialize index to zero on first call
@return true if valid line has been returned

*/
    bool getDraw( s_dxf_draw& draw );
};

#endif // !defined(AFX_POLYLINE_H__2845A691_54B0_40CE_9000_80D63B888975__INCLUDED_)
