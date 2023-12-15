// Solid.cpp: implementation of the CSolid class.
//
//////////////////////////////////////////////////////////////////////

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
    std::cout << "CSolid::Update " << x <<" "<< y <<" "<< x2 <<" "<< y2 <<"\n";
    rect.Update( x, y );
    rect.Update( x2, y2 );
}

bool CSolid::Append( cvit_t& cvit )
{
    double xa[4], ya[4];  // x array, y array, which are used for eParser::solid_4point

    // loop over code-value pairs until 0 code encountered,
    // indicating new object or section
    while( true )
    {
        // advance to next code-value pair
        cvit++;

        switch ( myParser )
        {

        case eParser::solid_2point:

            switch( cvit->Code() )
            {
            case 0:
                // a new object

                // convert to triangular mesh
                convert_2point();

                // all done
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
            break;

        case eParser::solid_4point:

            switch( cvit->Code() )
            {
            case 0:
                // a new object

                // convert 4 points to 2 triangular and add them to the mesh
                append_triangle(&xa[0],&ya[0]);
                append_triangle(&xa[1],&ya[1]);
                // calculate the bounding rectangle
                x = xa[0], y = ya[0], x2 = xa[0], y2 = ya[0];
                for(int i=1; i<4; i++) // index start from 1
                {
                    if(xa[i]<x) x=xa[i];   // left
                    if(ya[i]>y) y=ya[i];   // top
                    if(xa[i]>x2) x2=xa[i]; // right
                    if(ya[i]<y2) y2=xa[i]; // bottom
                }

                cvit--;
                return false;
            case 8:
                m_Layer = cvit->myValue;
                break;
            // 10 20
            case 10:
                xa[0] = atof(cvit->myValue.c_str());
                break;
            case 20:
                ya[0] = atof(cvit->myValue.c_str());
                break;
            // 11 21
            case 11:
                xa[1] = atof(cvit->myValue.c_str());
                break;
            case 21:
                ya[1] = atof(cvit->myValue.c_str());
                break;
            // 12 22
            case 12:
                xa[2] = atof(cvit->myValue.c_str());
                break;
            case 22:
                ya[2] = atof(cvit->myValue.c_str());
                break;
            // 13 23
            case 13:
                xa[3] = atof(cvit->myValue.c_str());
                break;
            case 23:
                ya[3] = atof(cvit->myValue.c_str());
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
    throw std::runtime_error("DXF error, unterminated SOLID");
}

void CSolid::convert_2point()
{
    std::vector< std::vector< std::vector< double > > > mesh
    {
        {
            {
                x, y
            },
            {
                x,y2
            },
            {
                x2,y2
            }
        },
        {
            {
                x,y
            },
            {
                x2,y
            },
            {
                x2,y2
            }
        }
    };
    myTriangMesh = mesh;

}


void CSolid::append_triangle(double x[], double y[])
{
    std::vector< std::vector< double > >  triangle =
    {
        {
            x[0], y[0]
        },
        {
            x[1], y[1]
        },
        {
            x[2], y[2]
        }
    };
    myTriangMesh.push_back(triangle);
}

bool CSolid::getDraw( cDrawPrimitiveData& draw )
{
    if( draw.index >= 2 )
        return false;
    draw.index++;
    draw.color = myColor;
    draw.x1 = myTriangMesh[ draw.index-1 ][0][0];
    draw.y1 = myTriangMesh[ draw.index-1 ][0][1];
    draw.x2 = myTriangMesh[ draw.index-1 ][1][0];
    draw.y2 = myTriangMesh[ draw.index-1 ][1][1];
    draw.x3 = myTriangMesh[ draw.index-1 ][2][0];
    draw.y3 = myTriangMesh[ draw.index-1 ][2][1];
    draw.rect->ApplyScale( draw.x1, draw.y1 );
    draw.rect->ApplyScale( draw.x2, draw.y2 );
    draw.rect->ApplyScale( draw.x3, draw.y3 );

//    switch ( myParser )
//    {
//    case eParser::solid_2point:
//        draw.color = myColor;
//        draw.x1 = x;
//        draw.y1 = y;
//        draw.x2  = x2;
//        draw.y2  = y2;
//
//        draw.rect->ApplyScale( draw.x1, draw.y1 );
//        draw.rect->ApplyScale( draw.x2, draw.y2 );
//
//        // convert right, bottom to width, height
//        draw.x2 -= draw.x1;
//        draw.y2 -= draw.y1;
//        break;
//    default:
//        throw std::runtime_error("SOLID parser not implemented");
//    }

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
