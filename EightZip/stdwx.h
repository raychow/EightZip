// stdwx.h

#ifndef STDWX_H
#define STDWX_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef PURE
#define PURE = 0
#endif

#include <wx/aui/aui.h>
#include <wx/dir.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/stdpaths.h>

#ifdef __WINDOWS__
// Load from a resource
#define wxICON16(X) wxIcon(wxT(#X), wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXPM__)
// Load from a resource
#define wxICON16(X) wxIcon(wxT(#X), wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXDFB__)
// Initialize from an included XPM
#define wxICON16(X) wxIcon(X##_xpm, wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXGTK__)
// Initialize from an included XPM
#define wxICON16(X) wxIcon(X##_xpm, wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXMAC__)
// Initialize from an included XPM
#define wxICON16(X) wxIcon(X##_xpm, wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXMOTIF__)
// Initialize from an included XPM
#define wxICON16(X) wxIcon(X##_xpm, wxICON_DEFAULT_TYPE, 16, 16)
#elif defined(__WXX11__)
// Initialize from an included XPM
#define wxICON16(X) wxIcon(X##_xpm, wxICON_DEFAULT_TYPE, 16, 16)
#else
// This will usually mean something on any platform
#define wxICON16(X) wxIcon(wxT(#X), wxICON_DEFAULT_TYPE, 16, 16)
#endif // platform

#define EIGHTZIP_PROJECT_NAME wxT("EightZip")
#define EIGHTZIP_NAME _("Eight Zip")

#endif // STDWX_H
