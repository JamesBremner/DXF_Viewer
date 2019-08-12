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
    double x[1024],y[1024];
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
    bool getDraw( s_dxf_draw& draw );

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

