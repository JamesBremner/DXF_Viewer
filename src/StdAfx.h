// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6B3D9C89_6A4D_4FCA_88EB_B2C08E919750__INCLUDED_)
#define AFX_STDAFX_H__6B3D9C89_6A4D_4FCA_88EB_B2C08E919750__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
#define BYTE unsigned char
#define UINT unsigned int
#define FALSE false
#define TRUE true

#endif

#include <string>
#include <vector>
using namespace std;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6B3D9C89_6A4D_4FCA_88EB_B2C08E919750__INCLUDED_)
