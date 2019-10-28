// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#ifndef __GNUWIN32__

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#else
// using the gcc compiler
#include <cstdio>
#include <cstring>
#include <cstdlib>
#define CString std::string
//#define BYTE unsigned char
//#define UINT unsigned int
#define FALSE false
#define TRUE true

#endif

#include <string>
#include <vector>
using namespace std;


