// Circle.h: interface for the CCircle class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace dxfv
{
class CCircle  : public cDXFGraphObject
{
public:
    double x,y,r;

    CCircle();
    CCircle( cCodeValue& cv );
    virtual ~CCircle();
    bool Append( std::vector<cCodeValue>::iterator& cvit );
    bool Read( FILE * fp, int& code, char* value );

    void Update( cBoundingRectangle& rect );

    /**  get drawing parameters, scaled to display window

    @param[in/out] draw  structure holding parameters
    @return true if valid line has been returned

    draw.x1, draw.y1  Center of circle
    draw.r            Radius of cirle

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void Adjust( double x, double y );
};
}

