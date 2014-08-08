// Arc.cpp: implementation of the CArc class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "dxf.h"
#include "Arc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArc::CArc()
{
	m_Layer = "0";
	m_Select = FALSE;
	m_Nest = FALSE;
}

CArc::~CArc()
{

}
	void CArc::Update( cBoundingRectangle& rect )
	{
	    rect.Update( x, y+r );
	    rect.Update( x, y-r );
	    rect.Update( x+r, y );
	    rect.Update( x-r, y );
	}
    bool CArc::getDraw( s_dxf_draw& draw )
    {
        draw.x1 = x - r;
        draw.y1 = y + r;
        draw.rect->ApplyScale( draw.x1, draw.y1);
        draw.r =  2 * r / draw.rect->myScale;
        draw.sa = sa;
        draw.ea = ea;
          if( ea < sa ) {
            // required to draw in clockwise direction
            // work arround for wxWidgets bug http://trac.wxwidgets.org/ticket/4437
          draw.ea += 360;
       }
        return true;
    }

bool CArc::Read( FILE * fp, int& code, char* lpValue )
{
	if( strcmp(lpValue,"ARC") != 0 ) {
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
			break;
		case 40:
			r = atof(lpValue);
			break;
		case 50:
			sa = atof(lpValue);
			break;
		case 51:
			ea = atof(lpValue);
			break;
		}
	}
	return true;
}



