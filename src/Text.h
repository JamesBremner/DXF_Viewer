#pragma once

namespace dxfv
{

class CText : public cDXFGraphObject
{
public:
    CText();
    CText( cCodeValue& cv );
    bool Append( cvit_t& cvit );
    bool getDraw( cDrawPrimitiveData& draw );
    void Update( cBoundingRectangle& rect )
    {
        rect.Update( x1, y1 );
    }
#ifdef wxwbuild
    void Draw( wxDC& dc, CDxf * dxf );
#endif // wxwbuild

private:
    std::string myText;
    double x1;
    double y1;

    void clean_mtext( std::string& text );
};
}
