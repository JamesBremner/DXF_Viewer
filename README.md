# DXF_Viewer
A simple DXF File viewer

Displaying sample file `dxf/Gear Sample-iss4.DXF`

![screenshot](https://github.com/JamesBremner/DXF_Viewer/blob/master/doc/Capture.png)

## DXF Graphical entities supported

- LINE
- LWPOLYLINE
- POLYLINE
- VERTEX ( as part of POLYLINE only )
- ARC
- TEXT
- CIRCLE
- SPLINE with fit points
- SPLINE with control points OR fit points
- SOLID

not supported:
- SPLINE with fit AND control points

## View features

- Fit to window ( menu click )
- Pan ( drag with left mouse button pressed )
- Zoom in and out around window center ( mouse wheel )
