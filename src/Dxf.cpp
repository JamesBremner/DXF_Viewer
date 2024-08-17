// Dxf.cpp: implementation of the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#include "Dxf.h"

namespace dxfv
{


cDXFGraphObject* cDXFGraphObject::Factory( cCodeValue& cv )
{
    if( cv.Code() )
        throw std::runtime_error( "cDXFGraphObject::Factory bad code");
    //std::cout << cv.myValue << "\n";
    cDXFGraphObject* po = (cDXFGraphObject*)new CLine( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new cLWPolyLine( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new cPolyLine( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CSpline( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CCircle( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CArc( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CText( cv );
    if( po->myfValid )
        return po;
    delete po;
    po = (cDXFGraphObject*)new CSolid( cv );
    if( po->myfValid )
        return po;
    delete po;
    return nullptr;
}

void cDXFGraphObject::AutocadColor2RGB( int ai )
{
    // http://gohtx.com/acadcolors.php

    std::cout << "AutocadColor2RGB " << ai << "\n";
    switch ( ai )
    {
    case 0:
        myColor = 0;
        break;
    case 1:
        myColor = 0x0000FF;
        break;
    case 2:
        myColor = 0x00FFFF;
        break;
    case 3:
        myColor = 0x00FF00;
        break;
    case 4:
        myColor = 0xFFFF00;
        break;
    case 5:
        myColor = 0xFF0000;
        break;
    case 9:
        myColor = 0x808080;
        break;
    default:
        // TODO: implement some other colors #30
        myColor = 0x808080;
    }
}

void cBoundingRectangle::CalcScale( int w, int h )
{
    myWindowHeight = h;

    double dx = x2 - x1;
    double dy = y2 - y1;

    double sx = dx / w;
    double sy = dy / h;

    sx *= myZoom;
    sy *= myZoom;

    if(sx > sy)
    {
        myScale = sx;
        x_offset = int(-x1 / myScale);
        y_offset = int((h / 2) + (dy / (2 * myScale)) - (y2 / myScale));
    }
    else
    {
        myScale = sy;
        x_offset = int((w / 2) - (dx / (2 * myScale)) - (x1 / myScale));
        y_offset = int(-y1 / myScale);
    }

//    std::cout << "CalcScale " << myScale
//        <<" "<< x1<<" "<<x2
//        <<" "<<w<<" "<<h<< "\n";
}

void cBoundingRectangle::ApplyScale( double& x, double& y )
{
    //std::cout << "ApplyScale " << myScale <<" "<< x <<" " << y << " -> ";

    x = (x / myScale ) + x_offset;
    y = myWindowHeight - (( y / myScale ) + y_offset );

    x += xpan;
    y += ypan;

    //std::cout << x <<" " << y << "\n";
//
//    double xt = x;
//    double yt = y;
//    RemoveScale( xt, yt );
//    std::cout <<"test " << xt <<" " << yt << "\n";
}
void cBoundingRectangle::RemoveScale( double& x, double& y )
{
    // std::cout << "RemoveScale " << myScale <<" "<< x <<" " << y << " -> ";
    x -= xpan;
    y -= ypan;
    x = ( x - x_offset ) * myScale;
    y = myWindowHeight - y;
    y = ( y - y_offset ) * myScale;
//     std::cout << x <<" " << y << "\n";
//     double xt = x;
//     double yt = y;
//     ApplyScale( xt, yt );
//     std::cout << "test " << xt <<" "<< yt << "\n";

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxf::CDxf()
    : m_InitialDraw( false )
    , myLoadStatus( none )
    , m_Nesting( false )
    , myfwxwidgets( false ) // do not use wxwidgets for contol point splines
    , myfSplineControlPointsPreferred( false )  // if true and choice available, splines prefer control points
    , mySolidParser( eParser::solid_2point )
{
}

CDxf::~CDxf()
{

}
/**  Read DXF file

@param[in] str path and filename

*/
void CDxf::LoadFile(const std::string& filepath)
{
    Init();

    myCodeValue.clear();
    std::ifstream f( filepath );
    if( ! f.is_open() )
    {
        myLoadStatus = none;
        return;
    }

    bool entities = false;
    cCodeValue cv;
    while( cv.Read( f ) )
    {
        // ignore everything until the ENTITIES section is reached
        if( ! entities )
        {
            if( cv.myValue == "ENTITIES")
                entities = true;
            continue;
        }
        //std::cout << cv.myCode <<" " << cv.myValue << "\n";
        myCodeValue.push_back( cv );
    }
    if( ! entities )
        throw std::runtime_error("DXF file has no ENTITIES section");

    for(
        cvit_t cvit = myCodeValue.begin();
        cvit != myCodeValue.end();
        cvit++ )
    {
        if( cvit->Code() != 0 )
            continue;

        //std::cout << cvit->myCode << ", "<< cvit->myValue << "\n";

        cDXFGraphObject * po = cDXFGraphObject::Factory( *cvit );
        if( po )
        {
            po->Options( this );
            po->Append( cvit );
            myGraphObject.push_back( po );
        }

    }
    UpdateBoundingRectangle();
    myLoadStatus = OK;
}


void CDxf::UpdateBoundingRectangle()
{
    myBoundingRectangle.init = false;
    for( auto po : myGraphObject )
        po->Update( myBoundingRectangle );

    // if( myBoundingRectangle.y1 < 0 )
    // {
    //     // entities with negative y co-ords present
    //     // zoom code doesn't handle -ve y
    //     // so adjust co-ords so that all y's are +ve

    //     for( auto po : myGraphObject )
    //     {
    //         po->Adjust( 0, -myBoundingRectangle.y1 );
    //     }

    //     // recalculate bounding rectangle
    //     myBoundingRectangle.init = false;
    //     for( auto po : myGraphObject )
    //     {
    //         po->Update( myBoundingRectangle );
    //     }
    // }
}

void CDxf::Init()
{
    myLoadStatus = none;
    m_InitialDraw = FALSE;
    myGraphObject.clear();
    m_Nesting = FALSE;
}

void CDxf::SolidParser( eParser ep )
{
    switch( ep )
    {
    case eParser::solid_2point:
    case eParser::solid_3point:
    case eParser::solid_4point:
        mySolidParser = ep;
        break;
    default:
        mySolidParser = eParser::solid_2point;
        break;
    }
}

void CDxf::Draw( int width, int height )
{
    // scale to window
    myBoundingRectangle.CalcScale(
        width,
        height );

    for( auto po : myGraphObject )
        po->Draw( this );
}

cDrawPrimitiveData::cDrawPrimitiveData( CDxf * dxf )
{
    index = 0;
    index_curve = 0;
    rect = &(dxf->myBoundingRectangle);
}


}
