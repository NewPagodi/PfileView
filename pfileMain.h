/***************************************************************
 * Name:      pfileMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2016-04-21
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef PFILEMAIN_H
#define PFILEMAIN_H

#include "GUIFrame.h"

class pfileFrame: public GUIFrame
{
    public:
        pfileFrame(wxFrame *frame);
    private:
        void OnFileChanged( wxFileDirPickerEvent& event );
        void OnDnD( wxDropFilesEvent & event );

        void ProcessFile(const wxString&);

        wxPGProperty* AddCategory(const wxString&,wxFile*);
        wxPGProperty*  AddSubCat(wxPGProperty*,const wxString&,int);
        void AddInt(wxPGProperty*,const wxString&,int);
        void AddFloat(wxPGProperty*,const wxString&,float);
        void AddColor(wxPGProperty*,const wxString&,int,unsigned char,unsigned char,unsigned char,unsigned char);

};

#endif // PFILEMAIN_H
