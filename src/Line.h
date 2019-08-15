// Line.h: interface for the CLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_H__57D9AECB_3DF9_42D3_BC4B_789EA9B64D68__INCLUDED_)
#define AFX_LINE_H__57D9AECB_3DF9_42D3_BC4B_789EA9B64D68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace dxfv
{

class cCodeValue;

class CLine : public cDXFGraphObject

{
public:
    double x1,y1,x2,y2;
    std::string m_Layer;
    unsigned char m_Select;
    unsigned char m_Nest;

    CLine();
    CLine( cCodeValue& cv );
    virtual ~CLine();


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
#endif // !defined(AFX_LINE_H__57D9AECB_3DF9_42D3_BC4B_789EA9B64D68__INCLUDED_)
