// Solid.cpp: implementation of the CSolid class.
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
    : cDXFGraphObject("SOLID",cDXFGraphObject::eType::solid)
{
    m_Layer = "0";
    m_Select = FALSE;
    m_Nest = FALSE;
    myColor = 0x00ffffff; // white color, default value
}

CSolid::CSolid( cCodeValue& cv )
    : CSolid()
{
    myfValid =( cv.myValue == myCode );
}

CSolid::~CSolid()
{

}

void CSolid::Options( CDxf * dxf )
{
    myParser = dxf->SolidParser();
}


void CSolid::Update( cBoundingRectangle& rect )
{
    std::cout << "CSolid::Update " << x[0] <<" "<< y[0] <<" "<< x[2] <<" "<< y[2] <<"\n";

    switch (myParser)
    {
    case eParser::solid_2point:
        rect.Update( x[0], y[0] );
        rect.Update( x[2], y[2] );
        break;
    case eParser::solid_4point:
        for(int i=0; i<4; i++)
        {
            rect.Update( x[i], y[i] );
        }
        break;
    }
}
bool CSolid::Append( cvit_t& cvit )
{
    while( true )
    {
        switch ( myParser )
        {

        case eParser::solid_2point:
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
                x[0] = atof(cvit->myValue.c_str());
                break;
            case 20:
                y[0] = atof(cvit->myValue.c_str());
                break;
            case 12:
                x[2] = atof(cvit->myValue.c_str());
                break;
            case 22:
                y[2] = atof(cvit->myValue.c_str());
                break;
            case 62:
                myColor = AutocadColor2RGB( atoi( cvit->myValue.c_str() ) );
                break;
            }
            break;

        case eParser::solid_4point:
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
            // 10 20
            case 10:
                x[0] = atof(cvit->myValue.c_str());
                break;
            case 20:
                y[0] = atof(cvit->myValue.c_str());
                break;
            // 11 21
            case 11:
                x[1] = atof(cvit->myValue.c_str());
                break;
            case 21:
                y[1] = atof(cvit->myValue.c_str());
                break;
            // 12 22
            case 12:
                x[2] = atof(cvit->myValue.c_str());
                break;
            case 22:
                y[2] = atof(cvit->myValue.c_str());
                break;
            // 13 23
            case 13:
                x[3] = atof(cvit->myValue.c_str());
                break;
            case 23:
                y[3] = atof(cvit->myValue.c_str());
                break;
            case 62:
                myColor = AutocadColor2RGB( atoi( cvit->myValue.c_str() ) );
                break;
            }
            break;

        default:
            throw std::runtime_error("SOLID parser not implemented");
        }
    }
    return true;
}

bool CSolid::getDraw( cDrawPrimitiveData& draw )
{
    int index = draw.index;

    switch ( myParser )
    {
    case eParser::solid_2point:
        if( draw.index > 0 ) // we only loop one element
            return false;
        draw.index++;

        draw.color = myColor;
        draw.x1 = x[0];
        draw.y1 = y[0];
        draw.x2  = x[2];
        draw.y2  = y[2];

        draw.rect->ApplyScale( draw.x1, draw.y1 );
        draw.rect->ApplyScale( draw.x2, draw.y2 );

        // convert right, bottom to width, height
        draw.x2 -= draw.x1;
        draw.y2 -= draw.y1;
        break;

    case eParser::solid_4point:
        if( draw.index > 3 )   // we have to loop 4 elements
            return false;

        draw.color = myColor;

        // change the last point's position
        if (index == 2)
            index = 3;
        else if(index==3)
            index = 2;
        draw.x1 = x[index];
        draw.y1 = y[index];

        draw.rect->ApplyScale( draw.x1, draw.y1 );

        draw.index++;          // increase the index
        break;

    default:
        throw std::runtime_error("SOLID parser not implemented");
    }

    return true;
}
void CSolid::Adjust( double ax, double ay )
{
    x[0] += ax;
    y[0] += ay;
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
