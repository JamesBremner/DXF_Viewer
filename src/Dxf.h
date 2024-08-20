// Dxf.h: interface for the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#define MAXPOINTS 1024

#ifdef wxwbuild
class wxDC;
#endif // wxwbuild
#ifdef nanabuild
namespace nana
{
    namespace paint
    {
        class graphics;
    }
}
#endif // nanabuild

namespace dxfv
{

    class CDxf;



    /// DXF code value pair
    class cCodeValue
    {
    public:
        std::string myCode;
        std::string myValue;

        bool Read(std::ifstream &f)
        {
            if (!std::getline(f, myCode))
                return false;
            if (!std::getline(f, myValue))
                return false;
            // remove leading whitespace from the group code
            size_t p = myCode.find_first_not_of(" ");
            if (p > 0)
                myCode.erase(0, p - 1);
            return true;
        }
        int Code()
        {
            return atoi(myCode.c_str());
        }
    };
    typedef std::vector<cCodeValue>::iterator cvit_t;

    /**  Manage zooming and scaling
     */
    class cBoundingRectangle
    {
    public:
        double x1, y1, x2, y2;
        bool init;
        double myScale;
        double myZoom;
        int xpan, ypan;
        double x_offset, y_offset;
        int myWindowHeight;

        cBoundingRectangle()
            : init(false), myZoom(1), xpan(0), ypan(0)
        {
        }

        /**  Fit all ontent so they show in the display window */
        void Fit()
        {
            myZoom = 1;
            xpan = 0;
            ypan = 0;
        }
        /**  Register some new content

        @param[in] x location of content, file units
        @param[in] y location of content, file units

        We do not care what the file units are,
        just assume that they are consistent

        The bounding rectangle will be enlarged to include this new point,
        if neccessary

        */
        void Update(double x, double y)
        {
            // std::cout << "brupdate " << y << "\n";
            if (!init)
            {
                init = true;
                x1 = x;
                x2 = x;
                y1 = y;
                y2 = y;
            }
            else
            {
                if (x < x1)
                    x1 = x;
                if (x > x2)
                    x2 = x;
                if (y < y1)
                    y1 = y;
                if (y > y2)
                    y2 = y;
            }
            //	std::cout << "BRUpdate  "<< x <<" "<< y <<" "<< x1 <<" "<< x2
            //        <<" "<< y1 <<" "<< y2 <<"\n";
        }
        /**  Increase zoom ( make contents display larger ) */
        void ZoomIn()
        {
            myZoom *= 1.05;
        }
        /**  Decrease zoom ( make contents display smaller ) */
        void ZoomOut()
        {
            myZoom *= 0.95;
        }
        /** Pan ( move contents across display

        @param[in]  ox, oy  old location, pixels
        @param[in   nx, ny  new location, pixels

        Contents will be moved by same pixels as old -> new

        */
        void Pan(int ox, int oy, int nx, int ny)
        {
            xpan += nx - ox;
            ypan += ny - oy;
        }
        /**  Calculate scale factors

         First calculate scale that will keep the bounding rectangle inside window
         Then multiply by zoom factor.

        The same scale can be applied to every point,
        so call this just once and the scale will be applied
        by ApplyScale until this is called with new values.

        @param[in] w  window width, pixels
        @param[in] h window height, pixels

        */
        void CalcScale(int w, int h);

        /**  Apply current scale, xoom and pan

        @param[in/out] x value
        @param[in/out] y value

        */
        void ApplyScale(double &x, double &y);
        void RemoveScale(double &x, double &y);
    };

    /**  Structure holding parameters for the operation needed to draw all or part of graphical object

    Some objects can be drawn in one operation - a line, a circle, an arc
    Other objects need multiple operations - a polyline and a spline

    The exact meaning of the elements of this structure depends on the graphical object

    The values are in pixels, scaled to fit into the display window
    */
    class cDrawPrimitiveData
    {
    public:
        double x1;
        double y1;
        double x2;
        double y2;
        double x3;
        double y3;
        double r;
        double sa;
        double ea;
        int thick;
        int index;
        int index_curve;
        std::string text;
        int color;
        cBoundingRectangle *rect; // bounding rectange scaled to window

        cDrawPrimitiveData(CDxf *dxf);
    };

    /**  Base class for all dxf graphical objects
     */
    class cDXFGraphObject
    {
    public:
        /// graph object types using this base class
        enum class eType
        {
            line,
            lwpolyline,
            polyline,
            arc,
            text,
            circle,
            spline,
            solid,
        } myType;

        /// true if valid construction
        bool myfValid;

        int myColor;

        int myThick;

        std::string myLayer;

        /// default contructor
        cDXFGraphObject()
        :  myColor(0x808080), myThick( 1 ), myLayer("0")
        {
        }

        virtual ~cDXFGraphObject()
        {
        }

        /** Construct specified type
            @param[in] cv DXF file code value pair specifying the type
            @return pointer to valid  constructed graph type, or nullptr
        */
        static cDXFGraphObject *Factory(cCodeValue &cv);

        /** Set options fron CDxf
            @param[in] CDxf to set options from

            NOP in base class,
            override in entity type that needs to set options
        */
        virtual void Options(CDxf *dxf) {}

        /**
         * @brief Parse dxf group code value pairs for an entity
         * 
         * @param cvit reference to iterator pointing to first code/value pair to be parsed ( code 0 ! )
         * @return true this should never happen
         * @return false encountered a new entity
         * 
         * In the base class this method is undefined.
         * Overide with parser for each entity type.
         * 
         * This should work its way through the code value pairs in a .dxf file
         * until a new entity is encountered ( a new code 0 ), parsing the values.
         * When new entity found return false with cvit pointing to new entity 0 code value pair
         * 
         */
        virtual bool Append(cvit_t &cvit) = 0;

        virtual void Update(cBoundingRectangle &rect) = 0;

        /** Get data for next call to graphic primitive
            @param[in/out] draw data for previous/next graphic primitive call
            @return false if no more needed, true otherwise
        */
        virtual bool getDraw(cDrawPrimitiveData &draw) = 0;

        /** Use graphic primitives to draw the entity
            @param[in] dxf pointer to the DXF entity container
        */
        virtual void Draw(CDxf *dxf) = 0;

        virtual void Adjust(double x, double y) = 0;

    protected:
        std::string myCode;


        /* @brief Convert autocad color code to RGB values
        /// @param ai color code // http://gohtx.com/acadcolors.php
        /// Stores RGB color in myColor attribute

        How to implement color for an arbitrary element:

    1.  In the element's Append() method, parse the color code by adding

    ```C++
                case 62:
                    AutocadColor2RGB(atof(cvit->myValue.c_str()));
                    break;
    ```

    2. In the element's getDraw() method, copy the color by adding

    ```C++
    draw.color = myColor;
    ```

    3. In the element's Draw method, pass the color to the GUI framework by adding

    ```C++
    dxf->shapes()->color( draw.color );
    ```

    4. Build and run

    */

        void AutocadColor2RGB(int ai);
    };

}

#include "line.h"
#include "arc.h"
#include "Circle.h"
#include "PolyLine.h"
#include "Spline.h"
#include "Solid.h"
#include "text.h"
#include "Dimension.h"

namespace dxfv
{

    /** A container for the graphical objects in a .DXF file
     */
    class CDxf
    {
    public:

        cBoundingRectangle myBoundingRectangle;

        CDxf();
        virtual ~CDxf();

        void Init();

        void LoadFile(const std::string &filepath);

        bool wxwidgets() const
        {
            return myfwxwidgets;
        }
        bool SplineControlPointsPreferred() const
        {
            return myfSplineControlPointsPreferred;
        }

        // accessors for graphical device context
        // which depend on the GUI framework used by the build
#ifdef wxwbuild
        void DC(wxDC *dc)
        {
            myDC = dc;
        }
        wxDC *DC()
        {
            return myDC;
        }
#endif // wxwbuild
#ifdef nanabuild
        void graph(nana::paint::graphics *g)
        {
            myGraph = g;
        }
        nana::paint::graphics *graph()
        {
            return myGraph;
        }

#endif // nanabuild


        /// Draw every entity
        void Draw(int width, int height);

    private:

        bool myfwxwidgets;                    ///< true if using wxwidgets method for control point splines
        bool myfSplineControlPointsPreferred; ///< true if, given choice, splines use control rather than fit points
        std::vector<cCodeValue> myCodeValue;

        /// Vector storing pointers to each parsed entity
        std::vector<cDXFGraphObject *> myGraphObject;

#ifdef wxwbuild
        wxDC *myDC;
#endif // wxwbuild

#ifdef nanabuild
        nana::paint::graphics *myGraph;
#endif // nanabuild

        void UpdateBoundingRectangle();
    };
}
