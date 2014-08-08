// Line.cpp: implementation of the CLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DXF.h"
#include "Line.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLine::CLine()
{
	m_Layer = "0";
	m_Select = FALSE;
	m_Nest = FALSE;
}

CLine::~CLine()
{

}
bool CLine::Read( FILE * fp, int& code, char* lpValue )
{
	if( strcmp(lpValue,"LINE") != 0 ) {
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
			x1 = atof(lpValue);
			break;
		case 20:
			y1 = atof(lpValue);
			break;
		case 11:
			x2 = atof(lpValue);
			break;
		case 21:
			y2 = atof(lpValue);
			break;
		}
	}
	return true;

}

    bool CLine::getDraw( s_dxf_draw& draw )
    {
        draw.x1 = x1;
        draw.y1 = y1;
        draw.x2 = x2;
        draw.y2 = y2;
        draw.rect->ApplyScale(draw.x1, draw.y1);
        draw.rect->ApplyScale(draw.x2, draw.y2);
        return true;
    }
