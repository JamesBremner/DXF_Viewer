#pragma once

namespace dxfv
{

class CText : public cDXFGraphObject
{
public:
    CText();
    CText( cCodeValue& cv );
    	bool Append( std::vector<cCodeValue>::iterator& cvit );
    bool Read( FILE * fp, int& code, char* value );
    bool getDraw( s_dxf_draw& draw );
    void Update( cBoundingRectangle& rect )
    {
        rect.Update( x1, y1 );
    }
private:
    std::string myText;
    double x1;
    double y1;

    void clean_mtext( std::string& text );
};
}
