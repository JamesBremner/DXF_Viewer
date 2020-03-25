/** Solid.h: interface for the CSolid class.

    SOLID objects are 2D rectangles
    specified by their top-right and bottm-left corners
*/

#pragma once

namespace dxfv
{
class CSolid  : public cDXFGraphObject
{
public:
    double x,y,x2,y2;       ///< locations of top-right and bottm-left corners

    std::vector< std::vector< std::vector< double > > > myTriangMesh;
    std::string m_Layer;
    unsigned char m_Select;
    unsigned char m_Nest;
    int myColor;

    CSolid();
    CSolid( cCodeValue& cv );
    virtual ~CSolid();

    void Options( CDxf * dxf );

    /** Append DXF code-value pair to object specification
        @param[in] cvit iterator pointing to code value pair

        <pre>
        code    interpretation
        10      x location of top left of 2D rectangle
        12      x location of bottom right of 2D rectangle
        20      y location of top left of 2D rectangle
        22      y location of bottom right of 2D rectangle
        </pre>
    */
    bool Append( cvit_t& cvit );

    void Update( cBoundingRectangle& rect );

    /**  get drawing parameters, scaled to display window

    @param[in/out] draw  structure holding parameters
    @return true if valid line has been returned

    */
    bool getDraw( cDrawPrimitiveData& draw );

    void Draw( CDxf * dxf );

    void Adjust( double x, double y );

    int AutocadColor2RGB( int ai );

private:

    // create two triangle from the two point, which makes one rectangle
    void convert_2point();

    // append a triangle(3 points) to the internal triangle mesh
    void append_triangle(double x[], double y[]);
};
}

