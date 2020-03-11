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
    int8_t start_quadrant = (int)sa % 360 / 90 + 1;// the quadrant of sa
    if(sa < 0)
    {
        start_quadrant += 3;
    }
    int8_t end_quadrant = (int)ea % 360 / 90 + 1;// the quadrant of ea
    if(ea < 0)
    {
        end_quadrant += 3;
    }

    // A little transform so we could calculate how many axes an arc have passed through correctly.
    // If the start_quadrant is 3 and the end_quadrant is 1, add 4 to end_quadrant.
    // So we know how many axes the arc pass through simply by substracting start_quadrant from end_quadrant( 5 - 3 = 2 )
    // If sa and ea are in the same quadrant, but start angle is larger than end angle.
    // It implies the arc pass through 4 axes, add 4 to end_quadrant as well.
    if(end_quadrant < start_quadrant || (end_quadrant == start_quadrant && (int)sa % 360 >(int)ea % 360 ))
    {
        end_quadrant += 4;
    }
    // Loop to determine which axis we should add to our consideration.
    // The first example above will go though 'axis' for 3 and 4 that means we take -y & +x into calculation
    for(int i = 0; i < end_quadrant - start_quadrant; i ++)
    {
        int axis = start_quadrant + i;
        if(axis > 4)
        {
            axis %= 4;
        }
        switch(axis)
        {
            case 1:
                rect.Update(x, y + r);// +y axis
                break;
            case 2:
                rect.Update(x - r, y);// -x axis
                break;
            case 3:
                rect.Update(x, y - r);// -y axis
                break;
            case 4:
                rect.Update(x + r, y);// +x axis
                break;
        }
    }
    rect.Update( x + r * cos(sa * M_PI / 180), y + r * sin(sa * M_PI / 180) );// start point of an arc
    rect.Update( x + r * cos(ea * M_PI / 180), y + r * sin(ea * M_PI / 180) );// end point of an arc
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

void CArc::Adjust( double ax, double ay )
{
    x += ax;
    y += ay;
}

}


