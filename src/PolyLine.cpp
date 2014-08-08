// PolyLine.cpp: implementation of the CPolyLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dxf.h"
#include "PolyLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyLine::CPolyLine()
{
	m_VertexCount = 0;
	m_PolyLineFlag = 0;
	m_Layer = "0";
	m_Select = FALSE;
	m_Nest = FALSE;
}

CPolyLine::~CPolyLine()
{

}

	void CPolyLine::Update( cBoundingRectangle& rect )
	{
	    for( int k = 0; k < (int)m_VertexCount; k++ )
        {
            rect.Update( x[k], y[k] );
        }
	}

bool CPolyLine::Read( FILE * fp, int& code, char* lpValue )
{
	if( strcmp(lpValue,"LWPOLYLINE") != 0 ) {
		// not a line
		return false;
	}
	int point_index = 0;
	while( fp != NULL ) {
		ReadTwoLines(fp, code, lpValue );
		switch ( code ) {
		case 0:
			// a new object
			if( point_index != (int)m_VertexCount )
				return false;
			return true;
		case 8:
			m_Layer = lpValue;
			break;
		case 90:
			m_VertexCount = atoi(lpValue);
			break;
		case 70:
			m_PolyLineFlag = atoi(lpValue);
			break;
		case 10:
			x[point_index] = atof(lpValue);
			break;
		case 20:
			y[point_index++] = atof(lpValue);
			break;
		}
	}
	return true;
}

bool CPolyLine::getDraw( s_dxf_draw& draw )
{
    if( 0 > draw.index || draw.index >= (int)m_VertexCount-1  )
        return false;
    draw.x1 = x[draw.index];
    draw.y1 = y[draw.index];
    draw.index++;
    draw.x2 = x[draw.index];
    draw.y2 = y[draw.index];
    draw.rect->ApplyScale( draw.x1, draw.y1 );
    draw.rect->ApplyScale( draw.x2, draw.y2 );
    return true;
}


