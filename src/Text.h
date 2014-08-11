#pragma once

namespace dxfv {

class CText : public cDXFGraphObject
{
    public:
	bool Read( FILE * fp, int& code, char* value );
    bool getDraw( s_dxf_draw& draw );
	private:
    std::string myText;
    double x1;
    double y1;

    void clean_mtext( std::string& text );
};
}
