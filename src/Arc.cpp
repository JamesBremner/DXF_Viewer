// Arc.cpp: implementation of the CArc class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>

#include "stdafx.h"
#include "dxf.h"
#include "Arc.h"

namespace dxfv
{

CArc::CArc()
    : cDXFGraphObject("ARC",cDXFGraphObject::eType::arc)
{
    m_Layer = "0";
    m_Select = FALSE;
    m_Nest = FALSE;
}

CArc::CArc( cCodeValue& cv )
    : CArc()
{
    myfValid =( cv.myValue == myCode );
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
bool CArc::getDraw( cDrawPrimitiveData& draw )
{
    if( draw.index )
        return false;
    draw.index++;
    draw.x1 = x - r;
    draw.y1 = y + r;
    draw.rect->ApplyScale( draw.x1, draw.y1);
    draw.r =  2 * r / draw.rect->myScale;
    draw.sa = sa;
    draw.ea = ea;
    if( ea < sa )
    {
        // required to draw in clockwise direction
        // work arround for wxWidgets bug http://trac.wxwidgets.org/ticket/4437
        draw.ea += 360;
    }
    return true;
}

void CArc::WAPData(
    int& xl, int& yt, int& xr, int& yb,
    int& sx, int& sy, int& ex, int& ey,
    CDxf* dxf )
{
    double dxl = x-r;
    double dyt = y-r;
    double dxr = x+r;
    double dyb = y+r;
    double dsx = x + r * cos(sa * M_PI/180);
    double dsy = y + r * sin(sa * M_PI/180);
    double dex = x + r * cos(ea * M_PI/180);
    double dey = y + r * sin(ea * M_PI/180);
    dxf->myBoundingRectangle.ApplyScale( dxl, dyt );
    dxf->myBoundingRectangle.ApplyScale( dxr, dyb );
    dxf->myBoundingRectangle.ApplyScale( dsx, dsy );
    dxf->myBoundingRectangle.ApplyScale( dex, dey );
    xl = round(dxl);
    yt = round(dyt);
    xr = round(dxr);
    yb = round(dyb);
    sx = round(dsx);
    sy = round(dsy);
    ex = round(dex);
    ey = round(dey);
}

bool CArc::Append(  cvit_t& cvit )
{
    while( true )
    {
        cvit++;
        //std::cout << cvit->Code() <<" "<< cvit->myValue << "\n";
        switch( cvit->Code() )
        {
        case 0:
            // a new object
            cvit--;
            return false;
        case 8:
            m_Layer = cvit->myValue;
            break;
        case 10:
            x = atof(cvit->myValue.c_str());
            break;
        case 20:
            y = atof(cvit->myValue.c_str());
            break;
        case 40:
            r = atof(cvit->myValue.c_str());
            break;
        case 50:
            sa = atof(cvit->myValue.c_str());
            break;
        case 51:
            ea = atof(cvit->myValue.c_str());
            break;
        }
    }
    return true;
}

}


