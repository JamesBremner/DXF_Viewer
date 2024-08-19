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

    CSolid();
    CSolid( cCodeValue& cv );
    virtual ~CSolid();

    void Options( CDxf * dxf );

    /** Append DXF code-value pair to object specification

        @param[in] cvit iterator pointing to code value pair

        The interpretation of the codes, depends on which parser has been selected
        The 2-point parser uses:
        <pre>
        code    interpretation
        0       end of SOLID specification
        10      x location of top left of 2D rectangle
        12      x location of bottom right of 2D rectangle
        20      y location of top left of 2D rectangle
        22      y location of bottom right of 2D rectangle
        </pre>
        The 4-point parser uses:
        <pre>
        code    interpretation
        0       end of SOLID specification
        10      x location of first vertex of 2D quadrilateral
        11      x location of second vertex of 2D quadrilateral
        12      x location of third vertex of 2D quadrilateral
        13      x location of fourth vertex of 2D quadrilateral
        20      y location of first vertex of 2D quadrilateral
        21      y location of second vertex of 2D quadrilateral
        22      y location of third vertex 2D quadrilateral
        23      y location of fourth vertex of 2D quadrilateral
        </pre>
        The quadrilateral is made up of two triangles. The first triangle is
        defined by the first three vertices, the second triangle is defined
        by the last three vertices.

        All other codes are ignored.
        A runtime_error exception is thrown if code 0 is not encountered

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

private:

    // create two triangle from the two point, which makes one rectangle
    void convert_2point();

    // append a triangle(3 points) to the internal triangle mesh
    void append_triangle(double x[], double y[]);
};
}

