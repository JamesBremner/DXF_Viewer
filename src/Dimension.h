#pragma once

namespace dxfv
{

class CDimension : public cDXFGraphObject
{
public:
    bool Read( FILE * fp, int& code, char* value );
    bool getDraw( cDrawPrimitiveData& draw );
    void Update( cBoundingRectangle& rect )
    {
//	    rect.Update( x1, y1 );
    }
private:
    std::string myText;
    double x1;
    double y1;

};
}

