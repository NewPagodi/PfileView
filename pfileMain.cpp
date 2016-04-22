/***************************************************************
 * Name:      pfileMain.cpp
 * Purpose:   Code for Application Frame
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

#include "pfileMain.h"
#include <vector>

class npDialogAdapter : public wxPGEditorDialogAdapter
{
    public:
        npDialogAdapter(const wxString& s):wxPGEditorDialogAdapter(),m_string(s){}

    private:
        virtual bool DoShowDialog( wxPropertyGrid* WXUNUSED(propGrid),
                                   wxPGProperty* WXUNUSED(property) ) wxOVERRIDE
        {
            MyDialog1 md(NULL);
            md.m_textCtrl2->SetValue(m_string);
            md.m_button3->SetFocus();
            md.ShowModal();

            return false;
        }

        const wxString& m_string;
};


class npDialogProperty : public wxStringProperty
{
    public:

        npDialogProperty( const wxString& label,
                              const wxString& name = wxPG_LABEL,
                              const wxString& value = wxEmptyString ,
                              const wxString& dialogtext = wxEmptyString)
            : wxStringProperty(label, name, value),m_dialogtext(dialogtext)
        {

        }

        // Set editor to have button
        virtual const wxPGEditor* DoGetEditorClass() const wxOVERRIDE
        {
            return wxPGEditor_TextCtrlAndButton;
        }

        // Set what happens on button click
        virtual wxPGEditorDialogAdapter* GetEditorDialog() const wxOVERRIDE
        {
            return new npDialogAdapter(m_dialogtext);
        }

    protected:
        wxString m_dialogtext;
};

pfileFrame::pfileFrame(wxFrame *frame): GUIFrame(frame)
{
    DataGrid->DragAcceptFiles(true);
    DataGrid->Bind( wxEVT_DROP_FILES, &pfileFrame::OnDnD, this );
}

unsigned char ReadChar(wxFile* file,unsigned char* c)
{
    file->Read(c, 1);
    return *c;
}

unsigned char ReadChar(wxFile* file)
{
    unsigned char c;
    return ReadChar(file,&c);
}

float ReadFloat(wxFile* file,float* fl)
{
    //This definately doesn't work:
    //file->Read(fl, 4);

    //This seems to be a little better:
    void* p = malloc(4);
    file->Read(p, 4);
    *fl=*(reinterpret_cast<float*>(p));
    free(p);

    return *fl;
}

float ReadFloat(wxFile* file)
{
    float f;
    return ReadFloat(file,&f);
}

unsigned short int ReadShort(wxFile* file,unsigned short int* usip)
{
    void* p = malloc(2);
    file->Read(p, 2);
    *usip=*(reinterpret_cast<unsigned short int*>(p));
    free(p);

    return *usip;
}

unsigned short int ReadShort(wxFile* file)
{
    unsigned short int usi;
    return ReadShort(file,&usi);
}

int ReadInt(wxFile* f,int* i)
{
    f->Read(i,4);
    return *i;
}

int ReadInt(wxFile* f)
{
    int i;
    return ReadInt(f,&i);
}

//void pfileFrame::outfloat(float f)
//{
//        unsigned char arrayOfByte[4];
//
//        for (int i = 0; i < 4; i++)
//        {
//            //arrayOfByte[3 - i] = (ii >> (i * 8));
//            float* fp;
//            int* ip;
//            fp=&f;
//            ip = reinterpret_cast<int*>(fp);
//            arrayOfByte[i] = (*ip >> (i * 8));
//        }
//
//
//        //(*m_textCtrl1) << ii <<"\n";
//
//        (*m_textCtrl1) << "\t";
//        for (int i = 0; i < 4; i++)
//        {
//            (*m_textCtrl1) << "\t"<< wxString::Format("%#02x", arrayOfByte[i]);
//        }
//        (*m_textCtrl1) << "\n";
//}

wxPGProperty* pfileFrame::AddCategory(const wxString& s,wxFile* file)
{
    wxString temp = wxString::Format( "%s @ %#02x", s, file->Tell() );

    return DataGrid->Append( new wxPropertyCategory( temp, wxPG_LABEL ) );
}

wxPGProperty* pfileFrame::AddSubCat(wxPGProperty* p,const wxString& s, int i)
{
    return DataGrid->AppendIn( p, new wxPropertyCategory(wxString::Format("%s %d",s,i),wxPG_LABEL) );
}

void pfileFrame::AddInt(wxPGProperty* p,const wxString& s,int i)
{
    DataGrid->AppendIn( p, new wxIntProperty(s,wxPG_LABEL,i) );
}

void pfileFrame::AddFloat(wxPGProperty* p,const wxString& s,float f)
{
    DataGrid->AppendIn( p, new wxFloatProperty(s,wxPG_LABEL,f) );
}

void pfileFrame::AddColor( wxPGProperty* p, const wxString& s, int i, unsigned char r,unsigned char g, unsigned char b, unsigned char a)
{
    wxPGProperty* misc = DataGrid->AppendIn( p, new wxColourProperty(wxString::Format("%s %d",s,i),wxPG_LABEL,wxColour( r, g, b, a)) );
    DataGrid->SetPropertyAttribute(misc, "HasAlpha", true);
}

void pfileFrame::OnDnD( wxDropFilesEvent & event )
{
    if(event.GetNumberOfFiles()!=1)
    {
        DataGrid->Clear();
        SetStatusText("Only 1 file can be dropped.",1);
        return;
    }

    m_filePicker1->SetPath(event.GetFiles()[0]);
    ProcessFile(event.GetFiles()[0]);
}

void pfileFrame::OnFileChanged( wxFileDirPickerEvent& event )
{
    DataGrid->Clear();
    wxString filename = m_filePicker1->GetFileName().GetFullPath();

    ProcessFile(filename);
}

void pfileFrame::ProcessFile(const wxString& filename)
{
    DataGrid->Clear();
    if(!wxFileName::FileExists(filename))
    {
        SetStatusText("Cant find file",1);
        return;
    }

    wxULongLong filesize = wxFileName::GetSize(filename);
    if(filesize<64)
    {
        SetStatusText("Invalud p file",1);
        return;
    }

    wxFile myfile;
    if( !myfile.Open(filename,wxFile::read) )
    {
        SetStatusText("Unable to open file",1);
        myfile.Close();
        return;
    }

    int Version            = ReadInt(&myfile);
    int off04              = ReadInt(&myfile);
    int VertexType         = ReadInt(&myfile);
    int NumVertices        = ReadInt(&myfile);
    int NumNormals         = ReadInt(&myfile);
    int NumUnknown1        = ReadInt(&myfile);
    int NumTexCs           = ReadInt(&myfile);
    int NumVertexColors    = ReadInt(&myfile);
    int NumEdges           = ReadInt(&myfile);
    int NumPolys           = ReadInt(&myfile);
    int NumUnknown2        = ReadInt(&myfile);
    int NumUnknown3        = ReadInt(&myfile);
    int NumHundreds        = ReadInt(&myfile);
    int NumGroups          = ReadInt(&myfile);
    int NumBoundingBoxes   = ReadInt(&myfile);
    int NormIndexTableFlag = ReadInt(&myfile);

    unsigned int needed_size
    =64
    +64
    +12*NumVertices
    +12*NumNormals
    +12*NumUnknown1
    +8*NumTexCs
    +4*NumVertexColors
    +4*NumPolys
    +4*NumEdges
    +24*NumPolys
    +24*NumUnknown2
    +3*NumUnknown3
    +100*NumHundreds
    +56*NumGroups
    +4 //There seems to be an extra 4 bytes between the groups table and the rest
    +24*NumBoundingBoxes
    +4*NumVertices;

    if(filesize<needed_size)
    {
        SetStatusText("Invalid p file",1);
        myfile.Close();
        return;
    }

    //If we've made it this far, start setting up the grid
    unsigned char c,blue,green,red,alpha;
    wxPGProperty* pgp,*misc;
    wxString s;

    pgp = DataGrid->Append( new wxPropertyCategory( "Header", wxPG_LABEL ) );
    AddInt(pgp,"Version",Version);
    AddInt(pgp,"off04",off04);
    AddInt(pgp,"VertexType",VertexType);
    AddInt(pgp,"NumVertices",NumVertices);
    AddInt(pgp,"NumNormals",NumNormals);
    AddInt(pgp,"NumUnknown1",NumUnknown1);
    AddInt(pgp,"NumTexCs",NumTexCs);
    AddInt(pgp,"NumVertexColors",NumVertexColors);
    AddInt(pgp,"NumEdges",NumEdges);
    AddInt(pgp,"NumPolys",NumPolys);
    AddInt(pgp,"NumUnknown2",NumUnknown2);
    AddInt(pgp,"NumUnknown3",NumUnknown3);
    AddInt(pgp,"NumHundreds",NumHundreds);
    AddInt(pgp,"NumGroups",NumGroups);
    AddInt(pgp,"NumBoundingBoxes",NumBoundingBoxes);
    AddInt(pgp,"NormIndexTableFlag",NormIndexTableFlag);

    pgp = AddCategory( "Runtime Data", &myfile );
    s << "decimal\thex\n";
    for(int i=0;i<64;i++)
    {
        ReadChar(&myfile,&c);
        s<< (int) c << "\t"<< wxString::Format("%#02x", c);
        if(i!=63)
        {
            s << "\n";
        }
    }

    DataGrid->Append( new npDialogProperty("Bytes",wxPG_LABEL,"64 bytes...",s) );
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Vertices", &myfile );
    for(int i=0;i<NumVertices;i++)
    {
        misc = AddSubCat( pgp, "vertex", i);
        AddFloat( misc, "x", ReadFloat(&myfile) );
        AddFloat( misc, "y", ReadFloat(&myfile) );
        AddFloat( misc, "z", ReadFloat(&myfile) );
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Vertex Normals", &myfile );
    for(int i=0;i<NumNormals;i++)
    {
        misc = AddSubCat( pgp, "normal", i);
        AddFloat( misc, "x", ReadFloat(&myfile) );
        AddFloat( misc, "y", ReadFloat(&myfile) );
        AddFloat( misc, "z", ReadFloat(&myfile) );
    }
    DataGrid->Collapse(pgp);

    if(NumUnknown1!=0)
    {
        pgp = AddCategory( "Unknown1", &myfile );
        for( int i=0; i<NumUnknown1; i++ )
        {
            misc = AddSubCat( pgp, "unknown", i);
            AddFloat( misc, "x", ReadFloat(&myfile) );
            AddFloat( misc, "y", ReadFloat(&myfile) );
            AddFloat( misc, "z", ReadFloat(&myfile) );
        }
        DataGrid->Collapse(pgp);
    }

    pgp = AddCategory( "Texture coordinates", &myfile );
    for( int i=0; i<NumTexCs; i++ )
    {
        misc = AddSubCat( pgp, "Texture coordinate", i);
        AddInt( misc, "0", ReadChar(&myfile) );
        AddInt( misc, "1", ReadChar(&myfile) );
        AddInt( misc, "2", ReadChar(&myfile) );
        AddInt( misc, "3", ReadChar(&myfile) );
        AddInt( misc, "4", ReadChar(&myfile) );
        AddInt( misc, "5", ReadChar(&myfile) );
        AddInt( misc, "6", ReadChar(&myfile) );
        AddInt( misc, "7", ReadChar(&myfile) );
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Vertex colors", &myfile );
    for(int i=0;i<NumVertexColors;i++)
    {
        ReadChar(&myfile,&blue);
        ReadChar(&myfile,&green);
        ReadChar(&myfile,&red);
        ReadChar(&myfile,&alpha);
        AddColor( pgp, "Vertex color", i, red, green, blue, alpha);
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Polygon colors ", &myfile );
    for(int i=0;i<NumPolys;i++)
    {
        ReadChar(&myfile,&blue);
        ReadChar(&myfile,&green);
        ReadChar(&myfile,&red);
        ReadChar(&myfile,&alpha);
        AddColor( pgp, "Polygon color", i, red, green, blue, alpha);
    }
    DataGrid->Collapse(pgp);


    pgp = AddCategory( "Edges", &myfile );
    for(int i=0;i<NumEdges;i++)
    {
        AddInt( pgp, wxString::Format("edge %d",i), ReadInt(&myfile) );
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Polygons", &myfile );
    for(int i=0;i<NumPolys;i++)
    {
        misc = AddSubCat(pgp,"polygon",i);
        AddInt(misc,"00 00",ReadShort(&myfile));
        AddInt(misc,"VertexIndex1",ReadShort(&myfile));
        AddInt(misc,"VertexIndex2",ReadShort(&myfile));
        AddInt(misc,"VertexIndex3",ReadShort(&myfile));
        AddInt(misc,"NormalIndex1",ReadShort(&myfile));
        AddInt(misc,"NormalIndex2",ReadShort(&myfile));
        AddInt(misc,"NormalIndex3",ReadShort(&myfile));
        AddInt(misc,"EdgeIndex1",ReadShort(&myfile));
        AddInt(misc,"EdgeIndex2",ReadShort(&myfile));
        AddInt(misc,"EdgeIndex3",ReadShort(&myfile));
        AddInt(misc,"u1",ReadShort(&myfile));
        AddInt(misc,"u2",ReadShort(&myfile));
    }
    DataGrid->Collapse(pgp);

    if(NumUnknown2!=0)
    {
        pgp = AddCategory( "Unknown2", &myfile );
        for(int i=0;i<NumUnknown2;i++)
        {
            misc = AddSubCat(pgp,"unknown",i);
            AddInt(misc,"00 00",ReadShort(&myfile));
            AddInt(misc,"VertexIndex1",ReadShort(&myfile));
            AddInt(misc,"VertexIndex2",ReadShort(&myfile));
            AddInt(misc,"VertexIndex3",ReadShort(&myfile));
            AddInt(misc,"NormalIndex1",ReadShort(&myfile));
            AddInt(misc,"NormalIndex2",ReadShort(&myfile));
            AddInt(misc,"NormalIndex3",ReadShort(&myfile));
            AddInt(misc,"EdgeIndex1",ReadShort(&myfile));
            AddInt(misc,"EdgeIndex2",ReadShort(&myfile));
            AddInt(misc,"EdgeIndex3",ReadShort(&myfile));
            AddInt(misc,"u1",ReadShort(&myfile));
            AddInt(misc,"u2",ReadShort(&myfile));
        }
        DataGrid->Collapse(pgp);
    }

    if(NumUnknown3!=0)
    {
        pgp = AddCategory( "Unknown3", &myfile );
        for(int i=0;i<NumUnknown3;i++)
        {
            misc = AddSubCat(pgp,"unknown",i);
            AddInt(misc,"0",ReadChar(&myfile));
            AddInt(misc,"1",ReadChar(&myfile));
            AddInt(misc,"2",ReadChar(&myfile));
        }
        DataGrid->Collapse(pgp);
    }

    pgp = AddCategory( "Hundreds", &myfile );
    for(int i=0;i<NumHundreds;i++)
    {
        s.Clear();
        s << "decimal\thex\n";
        for(int i=0;i<100;i++)
        {
            ReadChar( &myfile, &c );
            s<< (int)c << "\t"<< wxString::Format("%#02x", c) <<"\n";
        }
        DataGrid->Append( new npDialogProperty("Bytes",wxPG_LABEL,wxString::Format("%d bytes...",100*NumHundreds),s) );
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Groups", &myfile );
    for(int i=0;i<NumGroups;i++)
    {
        misc = AddSubCat(pgp,"group",i);
        AddInt( misc, "PrimitiveType", ReadInt(&myfile) );
        AddInt( misc, "PolygonStartIndex", ReadInt(&myfile) );
        AddInt( misc, "NumPolygons", ReadInt(&myfile) );
        AddInt( misc, "VerticesStartIndex", ReadInt(&myfile) );
        AddInt( misc, "NumVertices", ReadInt(&myfile) );
        AddInt( misc, "EdgeStartIndex", ReadInt(&myfile) );
        AddInt( misc, "NumEdges", ReadInt(&myfile) );
        AddInt( misc, "u1", ReadInt(&myfile) );
        AddInt( misc, "u2", ReadInt(&myfile) );
        AddInt( misc, "u3", ReadInt(&myfile) );
        AddInt( misc, "u4", ReadInt(&myfile) );
        AddInt( misc, "TexCoordStartIndex", ReadInt(&myfile) );
        AddInt( misc, "AreTexturesUsed", ReadInt(&myfile) );
        AddInt( misc, "TextureNumber", ReadInt(&myfile) );
    }
    DataGrid->Collapse(pgp);

    //There seems to be an inexplicable 4 bytes between the groups tables
    //and the remaining 2 tables:
    ReadInt(&myfile);

    pgp = AddCategory( "Bounding boxes", &myfile );
    for(int i=0;i<NumBoundingBoxes;i++)
    {
        misc = AddSubCat(pgp,"box",i);
        AddFloat(misc,"max x",ReadFloat(&myfile));
        AddFloat(misc,"max y",ReadFloat(&myfile));
        AddFloat(misc,"max z",ReadFloat(&myfile));
        AddFloat(misc,"min x",ReadFloat(&myfile));
        AddFloat(misc,"min y",ReadFloat(&myfile));
        AddFloat(misc,"min z",ReadFloat(&myfile));
    }
    DataGrid->Collapse(pgp);

    pgp = AddCategory( "Normal index table", &myfile );
    for(int i=0;i<NumVertices ;i++)
    {
        AddInt(pgp,wxString::Format("vertex %d",i),ReadInt(&myfile));
    }
    DataGrid->Collapse(pgp);


    //Since we don't know the size of Hundreds, before continuing, we
    //need to do some calculations:

//    s.Clear();
//    wxFileOffset t = myfile.Tell();
//    std::vector<unsigned char> the_rest;
//
//    while(!myfile.Eof())
//    {
//        ReadChar(&myfile,&red);
//        the_rest.push_back(red);
//    }
//    int bytes_in_hundreds = (int)(the_rest.size() - 56*NumGroups - 24*NumBoundingBoxes -4*NumVertices );
//
//    s << "decimal\thex\n";
//    for(int i=0;i<bytes_in_hundreds;i++)
//    {
//        s<< (int) the_rest[i] << "\t"<< wxString::Format("%#02x", the_rest[i]) <<"\n";
//    }
//    the_rest.clear();
//
//    wxFileOffset sought = myfile.Seek(t+bytes_in_hundreds);
//    if(sought==wxInvalidOffset)
//    {
//        //We should never get here, but just in case:
//        SetStatusText("Unable to read rest of file");
//        myfile.Close();
//        return;
//    }
//
//    pgp = AddCategory( "Hundreds", &myfile );
//    DataGrid->Append( new npDialogProperty("Bytes",wxPG_LABEL,wxString::Format("%d bytes...",bytes_in_hundreds),s) );
//    DataGrid->Collapse(pgp);
//
//    pgp = AddCategory( "Groups", &myfile );
//    for(int i=0;i<NumGroups;i++)
//    {
//        misc = AddSubCat(pgp,"group",i);
//        AddInt( misc, "PrimitiveType", ReadInt(&myfile) );
//        AddInt( misc, "PolygonStartIndex", ReadInt(&myfile) );
//        AddInt( misc, "NumPolygons", ReadInt(&myfile) );
//        AddInt( misc, "VerticesStartIndex", ReadInt(&myfile) );
//        AddInt( misc, "NumVertices", ReadInt(&myfile) );
//        AddInt( misc, "EdgeStartIndex", ReadInt(&myfile) );
//        AddInt( misc, "NumEdges", ReadInt(&myfile) );
//        AddInt( misc, "u1", ReadInt(&myfile) );
//        AddInt( misc, "u2", ReadInt(&myfile) );
//        AddInt( misc, "u3", ReadInt(&myfile) );
//        AddInt( misc, "u4", ReadInt(&myfile) );
//        AddInt( misc, "TexCoordStartIndex", ReadInt(&myfile) );
//        AddInt( misc, "AreTexturesUsed", ReadInt(&myfile) );
//        AddInt( misc, "TextureNumber", ReadInt(&myfile) );
//    }
//    DataGrid->Collapse(pgp);
//
//    pgp = AddCategory( "Bounding boxes", &myfile );
//    for(int i=0;i<NumBoundingBoxes;i++)
//    {
//        misc = AddSubCat(pgp,"box",i);
//        AddFloat(misc,"max x",ReadFloat(&myfile));
//        AddFloat(misc,"max y",ReadFloat(&myfile));
//        AddFloat(misc,"max z",ReadFloat(&myfile));
//        AddFloat(misc,"min x",ReadFloat(&myfile));
//        AddFloat(misc,"min y",ReadFloat(&myfile));
//        AddFloat(misc,"min z",ReadFloat(&myfile));
//    }
//    DataGrid->Collapse(pgp);
//
//    pgp = AddCategory( "Normal index table", &myfile );
//    for(int i=0;i<NumVertices ;i++)
//    {
//        AddInt(pgp,wxString::Format("vertex %d",i),ReadInt(&myfile));
//    }
//    DataGrid->Collapse(pgp);

    myfile.Close();
    SetStatusText("File Opened",1);
}
