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
#include <wx/filepicker.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
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

#define SIZER_BORDER 10
#define SIZERFLAGS() wxSizerFlags().Border(wxALL, SIZER_BORDER)
#define SIZERFLAGS_(x) wxSizerFlags(x).Border(wxALL, SIZER_BORDER)
#define HORIZON_SIZERFLAGS() \
wxSizerFlags().Border(wxLEFT | wxRIGHT, SIZER_BORDER)
#define HORIZON_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxLEFT | wxRIGHT, SIZER_BORDER)
#define TOP_SIZERFLAGS() wxSizerFlags().Border(wxTOP, SIZER_BORDER)
#define TOP_SIZERFLAGS_(x) wxSizerFlags(x).Border(wxTOP, SIZER_BORDER)
#define BOTTOM_SIZERFLAGS() wxSizerFlags().Border(wxBOTTOM, SIZER_BORDER)
#define BOTTOM_SIZERFLAGS_(x) wxSizerFlags(x).Border(wxBOTTOM, SIZER_BORDER)
#define LEFT_SIZERFLAGS wxSizerFlags().Border(wxLEFT, SIZER_BORDER)
#define LEFT_SIZERFLAGS_(x) wxSizerFlags(x).Border(wxLEFT, SIZER_BORDER)
#define RIGHT_SIZERFLAGS wxSizerFlags().Border(wxRIGHT, SIZER_BORDER)
#define RIGHT_SIZERFLAGS_(x) wxSizerFlags(x).Border(wxRIGHT, SIZER_BORDER)
#define TOP_LEFT_SIZERFLAGS() \
wxSizerFlags().Border(wxTOP | wxLEFT, SIZER_BORDER)
#define TOP_LEFT_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxTOP | wxLEFT, SIZER_BORDER)
#define TOP_RIGHT_SIZERFLAGS() \
wxSizerFlags().Border(wxTOP | wxRIGHT, SIZER_BORDER)
#define TOP_RIGHT_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxTOP | wxRIGHT, SIZER_BORDER)
#define TOP_HORIZON_SIZERFLAGS() \
wxSizerFlags().Border(wxTOP | wxLEFT | wxRIGHT, SIZER_BORDER)
#define TOP_HORIZON_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxTOP | wxLEFT | wxRIGHT, SIZER_BORDER)
#define BOTTOM_LEFT_SIZERFLAGS() \
wxSizerFlags().Border(wxBOTTOM | wxLEFT, SIZER_BORDER)
#define BOTTOM_LEFT_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxBOTTOM | wxLEFT, SIZER_BORDER)
#define BOTTOM_RIGHT_SIZERFLAGS() \
wxSizerFlags().Border(wxBOTTOM | wxRIGHT, SIZER_BORDER)
#define BOTTOM_RIGHT_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxBOTTOM | wxRIGHT, SIZER_BORDER)
#define BOTTOM_HORIZON_SIZERFLAGS() \
wxSizerFlags().Border(wxBOTTOM | wxLEFT | wxRIGHT, SIZER_BORDER)
#define BOTTOM_HORIZON_SIZERFLAGS_(x) \
wxSizerFlags(x).Border(wxBOTTOM | wxLEFT | wxRIGHT, SIZER_BORDER)

#endif // STDWX_H
