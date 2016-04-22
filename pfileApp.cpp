/***************************************************************
 * Name:      pfileApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2016-04-21
 * Copyright:  ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "pfileApp.h"
#include "pfileMain.h"

IMPLEMENT_APP(pfileApp);

bool pfileApp::OnInit()
{
    pfileFrame* frame = new pfileFrame(0L);
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Show();
    
    return true;
}
