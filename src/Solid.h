// Circle.h: interface for the CCircle class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace dxfv
{
class CSolid  : public cDXFGraphObject
{
public:
    double x,y,x2,y2;
    std::string m_Layer;
    unsigned char m_Select;
    unsigned char m_Nest;
    int myColor;

    CSolid();
    CSolid( cCodeValue& cv );
    virtual ~CSolid();
    bool Append( std::vector<cCodeValue>::iterator& cvit );
    bool Read( FILE * fp, int& code, char* value );

    void Update( cBoundingRectangle& rect );

    /**  get drawing parameters, scaled to display window

    @param[in/out] draw  structure holding parameters
    @return true if valid line has been returned

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void Adjust( double x, double y );

    int AutocadColor2RGB( int ai );
};
}

