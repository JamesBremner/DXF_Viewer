// Circle.cpp: implementation of the CCircle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dxf.h"
#include "Circle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCircle::CCircle()
{
	m_Layer = "0";
	m_Select = FALSE;
	m_Nest = FALSE;
}

CCircle::~CCircle()
{

}


	void CCircle::Update( cBoundingRectangle& rect )
	{
	    rect.Update( x, y+r );
	    rect.Update( x, y-r );
	    rect.Update( x+r, y );
	    rect.Update( x-r, y );
	}
bool CCircle::Read( FILE * fp, int& code, char* lpValue )
{
	if( strcmp(lpValue,"CIRCLE") != 0 ) {
		// not a line
		return false;
	}
	while( fp != NULL ) {
		ReadTwoLines(fp, code, lpValue );
		switch ( code ) {
		case 0:
			// a new object
			return true;
		case 8:
			m_Layer = lpValue;
			break;
		case 10:
			x = atof(lpValue);
			break;
		case 20:
			y = atof(lpValue);
		case 40:
			r = atof(lpValue);
			break;
		}
	}
	return true;
}


    bool CCircle::getDraw( s_dxf_draw& draw )
    {
        draw.x1 = x;
        draw.y1 = y;
        draw.r  = r;
        draw.rect->ApplyScale( draw.x1, draw.y1 );
        draw.r /= draw.rect->myScale;
        return true;
    }
