// Line.h: interface for the CLine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace dxfv
{

class cCodeValue;

class CLine : public cDXFGraphObject

{
public:
    double x1,y1,x2,y2;


    CLine( cCodeValue& cv );
    virtual ~CLine();

    void Options(CDxf *dxf);

    void Update( cBoundingRectangle& rect )
    {
        rect.Update( x1, y1 );
        rect.Update( x2, y2 );
    }


    bool Append( std::vector<cCodeValue>::iterator& cvit );


    /**  get drawing parameters, scaled to display window

    @param[in/out] draw  structure holding parameters
    @return true if valid line has been returned

    draw.x1, draw.y1  first endpoint of line
    draw.x2, draw.y2  second endpoint of line

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void Adjust( double x, double y );

};
}
