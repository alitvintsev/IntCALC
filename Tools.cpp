//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Tools.h"
#include "graph_cpp.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
        : TForm(Owner)
{
   ToolBtn=new TToolButton *[Form1->NumberOfElements];
ToolBar1->AutoSize=false;

Graphics::TBitmap *Bmp = new Graphics::TBitmap();
//bmp->LoadFromStream
//ImageList1->Add(
AnsiString Fname="d:\\program files\\borland\\cbuilder5\\projects\\mygraph\\pics\\icos\\tr3-2.bmp";
FS=new TFileStream*[Form1->NumberOfElements];
ImageList1->Width=23;
ImageList1->Height=23;
for (int i=0;i<Form1->NumberOfElements;i++)
  {
   Fname=Form1->TemplObj[Form1->NumberOfElements-i-1].IcoPictureFileName;
//   Fname="d:\\program files\\borland\\cbuilder5\\projects\\mygraph\\pics\\icos\\tr3-2.bmp";
   FS[i]=new TFileStream(Fname,fmOpenRead);

   Bmp->LoadFromStream(FS[i]);
   ImageList1->AddMasked(Bmp,1);
  }
for (int i=0;i<Form1->NumberOfElements;i++)
  {
   delete FS[i];
  }
delete []FS;
FS=NULL;
//ImageList1->AddMasked(Bmp,1);

for (int i=0;i<Form1->NumberOfElements;i++)
  {
   ToolBtn[i]=new TToolButton(ToolBar1);
   ToolBtn[i]->Parent=ToolBar1;

   ToolBtn[i]->Width=30;
   ToolBtn[i]->Height=30;
//   ToolBtn[i]->Left=i*(ToolBtn[i]->Width);

  // ToolBtn[i]->Left=a;
   ToolBar1->Width=(1+ToolBtn[i]->Width)*(i+1);
   ToolBtn[i]->OnClick=Form1->Obj1Execute;
   ToolBtn[i]->ShowHint=true;
   ToolBtn[i]->Hint=Form1->TemplObj[Form1->NumberOfElements-i-1].ElementName;
   ToolBtn[i]->ImageIndex=i;
   //   ToolBtn[i]->Hint=i;
  }
this->Height=CoolBar1->Top+CoolBar1->Height+41;
this->Left=0;
this->Top=0;
this->Width=Screen->Width;



}
//---------------------------------------------------------------------------


void __fastcall TForm3::FormClose(TObject *Sender, TCloseAction &Action)
{
Action=caNone;
Form1->MenuExit();
}
//---------------------------------------------------------------------------





void __fastcall TForm3::ComboBox1KeyPress(TObject *Sender, char &Key)
{
AnsiString Mashtab="";
if(Key=='\r')
{
for (int j=0;j<ComboBox1->Text.Length();j++)
 {
  if(ComboBox1->Text[j+1]=='.')
   ComboBox1->Text[j+1]=',';
  if(ComboBox1->Text[j+1]!='%')
   Mashtab+=ComboBox1->Text[j+1];
 }
Form1->Hide();
Form1->ScaleBy(100,Form1->Mk*100);
Form1->Mk=StrToFloat(Mashtab)/100.;
Form1->ScaleBy(int(Form1->Mk*100.),100);
Form1->Show();
}
}
//---------------------------------------------------------------------------

void __fastcall TForm3::ToolButton1Click(TObject *Sender)
{
Form1->MenuNew();        
}
//---------------------------------------------------------------------------

void __fastcall TForm3::ToolButton2Click(TObject *Sender)
{
Form1->MenuOpen();        
}
//---------------------------------------------------------------------------

void __fastcall TForm3::ToolButton3Click(TObject *Sender)
{
Form1->MenuSave();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::ToolButton5Click(TObject *Sender)
{
Form1->IntervalRaschet();
}
//---------------------------------------------------------------------------
