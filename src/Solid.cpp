// Circle.cpp: implementation of the CSolid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dxf.h"
#include "Solid.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dxfv
{

CSolid::CSolid()
    : cDXFGraphObject("SOLID",cDXFGraphObject::eType::circle)
{
    m_Layer = "0";
    m_Select = FALSE;
    m_Nest = FALSE;
}

CSolid::CSolid( cCodeValue& cv )
    : CSolid()
{
    myfValid =( cv.myValue == myCode );
}

CSolid::~CSolid()
{

}


void CSolid::Update( cBoundingRectangle& rect )
{
    std::cout << "CSolid::Update " << x <<" "<< y <<" "<< x2 <<" "<< y2 <<"\n";
    rect.Update( x, y );
    rect.Update( x2, y2 );
}
bool CSolid::Append( cvit_t& cvit )
{
    while( true )
    {
        cvit++;
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
        case 12:
            x2 = atof(cvit->myValue.c_str());
            break;
        case 22:
            y2 = atof(cvit->myValue.c_str());
            break;
        case 62:
            myColor = AutocadColor2RGB( atoi( cvit->myValue.c_str() ) );
            break;
        }
    }
    return true;
}

bool CSolid::getDraw( cDrawPrimitiveData& draw )
{
    if( draw.index )
        return false;
    draw.index++;
    draw.color = myColor;
    draw.x1 = x;
    draw.y1 = y;
    draw.x2  = x2;
    draw.y2  = y2;

    draw.rect->ApplyScale( draw.x1, draw.y1 );
    draw.rect->ApplyScale( draw.x2, draw.y2 );

    // convert right, bottom to width, height
    draw.x2 -= draw.x1;
    draw.y2 -= draw.y1;

    return true;
}
void CSolid::Adjust( double ax, double ay )
{
    x += ax;
    y += ay;
}

int CSolid::AutocadColor2RGB( int ai )
{
    // http://gohtx.com/acadcolors.php

    std::cout << "AutocadColor2RGB " << ai << "\n";
    switch ( ai )
    {
    case 0:
        return 0;
    case 9:
        return 0x808080;
    default:
        return 0;
    }
}
}
