// PolyLine.h: interface for the cLWPolyLine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

namespace dxfv
{
/// Light Weight polyline

class cLWPolyLine   : public cDXFGraphObject

{
public:
    unsigned int m_VertexCount;
    double x[MAXPOINTS],y[MAXPOINTS];
    std::string m_Layer;
    unsigned char m_Select;
    unsigned char m_Nest;

    cLWPolyLine();
    cLWPolyLine( cCodeValue& cv );

    virtual ~cLWPolyLine();

    void Update( cBoundingRectangle& rect );

    bool Append( cvit_t& cvit );

    /**  get drawing parameters, scaled to display window, for the 'next' line

    @param[in/out] draw  structure holding parameters, intialize index to zero on first call
    @return true if valid line has been returned

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void Adjust( double ax, double ay );

protected:
    bool myfClosed;
};

/// Polyline

class cPolyLine : public cLWPolyLine
{
public:
    cPolyLine();
    cPolyLine( cCodeValue& cv );

    virtual ~cPolyLine();

    bool Append( cvit_t& cvit );
};
}

