#include "Riostream.h"
#include "TUrl.h"
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TGrid.h"

void CopyFromRemote(const char* txtfile="grid.esd.txt")
{
  char line[1024];
  ifstream in(gSystem->ExpandPathName(txtfile));
  
  while ( in.getline(line,1024,'\n') )
  {
    TUrl url(line);
    
    if ( TString(url.GetProtocol()) == "alien" ) 
    {
      if (!gGrid) 
      {
        TGrid::Connect("alien://");
        if (!gGrid) 
        {
          cout << "Cannot get alien connection" << endl;
          return;          
        }
      }
      
    }
    TString file(url.GetFile());

    cout << "File :" << file.Data() << endl;
    
    TString dir(gSystem->DirName(file));
    
    gSystem->mkdir(Form("/Users/audurier/Documents/Analysis/LHC_15g_pp/TrackingEfficiency/MonteCarlo/%s",dir.Data()),kTRUE);

    if ( gSystem->AccessPathName(file.Data())==kFALSE)
    {
      cout << "Skipping copy of " << file.Data() << " as it already exists" << endl;
    }
    else
    {
      if(TFile::Cp(line,Form("/Users/audurier/Documents/Analysis/LHC_15g_pp/TrackingEfficiency/MonteCarlo/%s",file.Data())))
      {
        if ( TString(line).Contains("root_archive.zip") )
        {
          gSystem->Exec(Form("unzip %s -d %s",file.Data(),gSystem->DirName(file.Data())));
          gSystem->Exec(Form("rm %s",file.Data()));
        }
      }
      else gSystem->Exec(Form("rm -rf /Users/audurier/Documents/Analysis/LHC_15g_pp/TrackingEfficiency/MonteCarlo/%s",dir.Data()));

    }
  }
}