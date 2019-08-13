#include "stdafx.h"
#include "DXF.h"
#include "Text.h"


namespace dxfv
{

CText::CText()
    : cDXFGraphObject("MTEXT",cDXFGraphObject::eType::text)
{

}
CText::CText( cCodeValue& cv )
    : CText()
{
    myfValid =( cv.myValue == myCode );
}

bool CText::Append(  cvit_t& cvit )
{
    while( true )
    {
        cvit++;
        switch( cvit->Code() )
        {
        case 0:
            // a new object
            cvit--;
            return false;
        case 1:
            // the text to display
            myText = cvit->myValue;
//            if( myText.find("{\\f") != -1 ) {
//                int p = myText.find(";");
//                if( p == -1 )
//                    return false;
//                myText = myText.substr(p+1);
//            }
            clean_mtext( myText );
            break;
        case 10:
            x1 = atof(cvit->myValue.c_str());
            break;
        case 20:
            y1 = atof(cvit->myValue.c_str());
            break;
        }

    }
    return true;
}


bool CText::getDraw( s_dxf_draw& draw )
{
    if( draw.index )
        return false;
    draw.index++;
    draw.text = myText;
    draw.x1 = x1;
    draw.y1 = y1;
    draw.rect->ApplyScale(draw.x1, draw.y1);
    return true;
}

void CText::clean_mtext( std::string& text )
{
    int esc = 0;
    while( 1 )
    {
        esc = text.find("\\",esc);
        if( esc == -1 )
            break;
        if( text[esc+1] == 'f' )
        {
            int p = text.find(";",esc+1);
            if( p == -1 )
            {
                text = "";
                return;
            }
            text = text.substr(0,esc) + text.substr(p+1);
        }
        else if ( text[esc+1] == 'L' )
        {
            text = text.substr(0,esc) + text.substr(esc+2);
        }
        else if ( text[esc+1] == 'P' )
        {
            text = text.substr(0,esc) + "\n" + text.substr(esc+2);
        }
        else if ( text[esc+1] == 'A' )
        {
            text = text.substr(0,esc) + text.substr(esc+2);
        }
        else if ( text[esc+1] == 'p' )
        {
            int p = text.find(";",esc+1);
            if( p == -1 )
            {
                text = "";
                return;
            }
            text = text.substr(0,esc) + text.substr(p+1);
        }
        else if ( text[esc+1] == 'H' )
        {
            int p = text.find(";",esc+1);
            if( p == -1 )
            {
                text = "";
                return;
            }
            text = text.substr(0,esc) + text.substr(p+1);
        }
        else if ( text[esc+1] == 'S' )
        {
            int p = text.find(";",esc+1);
            if( p == -1 )
            {
                text = "";
                return;
            }
            text = text.substr(0,esc) + text.substr(p+1);
        }
        else
        {
            esc++;
        }
    }
    if( text[0] == '{' )
        text = text.substr(1);
    if( text[ text.length()-1] == '}' )
        text = text.substr(0,text.length()-1);
}
}
