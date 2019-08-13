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
    UINT m_VertexCount;
    double x[MAXPOINTS],y[MAXPOINTS];
    std::string m_Layer;
    BYTE m_Select;
    BYTE m_Nest;

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

#ifdef wxwbuild
    void Draw( wxDC& dc, CDxf * dxf );
#endif // wxwbuild

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

