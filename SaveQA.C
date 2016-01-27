//
//  SetPath.C
//
//
//  Created by Javier Martin-Blanco on 29/03/13.
//
//

#include <stdio.h>
#include <TArrayI.h>
#include <TString.h>
#include <TObjString.h>
#include <Riostream.h>
#include <fstream>
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"

Bool_t isQATask = kTRUE;
TString MultDirData = "LowMultiplicity";
TString MultDirMC = "LowMultiplicity";

//----------------------------------------------------------------------------
void SaveQA(TString inputDataFile, TString inputMCFile)
{
  // Open rootfile lists
  ifstream inDataFile(inputDataFile.Data()); // open input file
  if (!inDataFile.is_open())
  {
    printf("cannot open file %s\n",inputDataFile.Data());
    return;
  }
  ifstream inMCFile(inputMCFile.Data()); // open input file
  if (!inMCFile.is_open())
  {
    printf("cannot open file %s\n",inputMCFile.Data());
    return;
  }

  if (gSystem->AccessPathName("displays")) gSystem->Exec("mkdir displays");

  TString currDataName; // Current file path in Data
  TString currMCName; // Current file path in Data

  TCanvas cTmp;
  gStyle->SetOptStat(0);
  cTmp.Divide(2,1,0,0);

  while (!inDataFile.eof()) //loop over elements in input file
  {
    currDataName.ReadLine(inDataFile,kTRUE); // Read line in input Data file
    if(currDataName.IsNull()) continue;

    currMCName.ReadLine(inMCFile,kTRUE); // Read line in input MC file
    if(currMCName.IsNull()) continue;

    //Get runnumber for Data
    TString runNumber = currDataName;
    Int_t c = runNumber.Last('/');
    runNumber.Remove(c);
    c = runNumber.Last('/');
    runNumber.Remove(0,c+1);

    printf("run number data : %s\n",runNumber.Data() );

    //Get MC run number
    TString runNumberCheck = currMCName;
    c = runNumberCheck.Last('/');
    runNumberCheck.Remove(c);
    c = runNumberCheck.Last('/');
    runNumberCheck.Remove(0,c+1);

    printf("run number MC : %s\n",runNumberCheck.Data() );

    if (!runNumber.Contains(runNumberCheck.Data())) {
      cout << "ERROR: mismatch found between the 2 files... Aborting." << endl;
      return;
    }

    cout<<"\rprocessing run "<<runNumber.Data()<<" ...\r"<<endl;

    // Create folder to store results
    if (gSystem->AccessPathName(Form("displays/%s",runNumber.Data())))
      gSystem->Exec(Form("mkdir displays/%s", runNumber.Data()));

    //Open files
    TFile* currDataFile = new TFile(currDataName.Data(),"read");
    TFile* currMCFile = new TFile(currMCName.Data(),"read");

    TObjArray *objsData = 0x0, *objsMC = 0x0;
    if (isQATask)
    {
      objsData = static_cast<TObjArray*>(currDataFile->Get("MUON_QA/expert"));
      objsMC = static_cast<TObjArray*>(currMCFile->Get("MUON_QA/expert"));
      if (!objsData || !objsMC)
      {
      	cout << "lists not found for run " << runNumber.Data() << endl;
      	break;
      }
    }

    // Loop over the chambers
    for ( Int_t iCh = 1 ; iCh < 11 ; iCh++ )
    {

      // //Create a temporary dir
      // if (gSystem->AccessPathName(Form("displays/%s/ESDclusterMapChamber%d",runNumber.Data(),iCh)))
      // gSystem->Exec(Form("mkdir displays/%s/ESDclusterMapChamber%d", runNumber.Data(),iCh));


      //Get Histo
      TH1* clusterMapData = isQATask ?
      static_cast<TH1*>(objsData->FindObject(Form("hClusterHitMapInCh%d",iCh))) :
      static_cast<TH1*>(currDataFile->Get(Form("MUON/ESDs/%s/Expert/%s_hESDClusterHitMap%d",
					       MultDirData.Data(),MultDirData.Data(),iCh)));
      TH1* clusterMapMC = isQATask ?
      static_cast<TH1*>(objsMC->FindObject(Form("hClusterHitMapInCh%d",iCh))) :
      static_cast<TH1*>(currMCFile->Get(Form("MUON/ESDs/%s/Expert/%s_hESDClusterHitMap%d",
					     MultDirMC.Data(),MultDirMC.Data(),iCh)));
      if (!clusterMapData || !clusterMapMC)
      {
      	cout << "histograms not found for run " << runNumber.Data() << endl;
      	break;
      }

      // Save canvas with the two chambers hitmap
      cTmp.Clear();
      gStyle->SetOptStat(0);
      cTmp.Divide(2,2/*,0.,0.*/);

     

      cTmp.cd(1);
      gPad->SetPad(0., 0.5, 0.5, 1); 
      clusterMapData->SetTitle(Form("Cluster position distribution for data"));
      clusterMapData->DrawCopy("");

      cTmp.cd(2);
      gPad->SetPad(0.5, 0.5, 1, 1);
      clusterMapMC->SetTitle(Form("Cluster position distribution for MC"));
      clusterMapMC->DrawCopy("");

      cTmp.cd(4);
      gPad->SetPad(0., 0., 1., 0.5); 

       // Scale histo
      // clusterMapData->Scale(1./clusterMapData->GetEntries());
      clusterMapMC->Scale(clusterMapData->GetEntries()/clusterMapMC->GetEntries());
      
      //Fill diff. Histo
      TH2F*h2= new TH2F("h1",Form("Differences for chamber %d",iCh),clusterMapData->GetNbinsX(),0.,clusterMapData->GetNbinsX(),clusterMapData->GetNbinsY(),0.,clusterMapData->GetNbinsY());

      //Loop over bin
      for (Int_t i = 0; i < clusterMapData->GetNbinsX(); ++i)
      {
        for (Int_t j = 0; j < clusterMapData->GetNbinsY(); ++j)
        {
          Double_t diff =0.;

          Double_t DataBin = clusterMapData->GetBinContent(i,j);
          Double_t MCBin = clusterMapMC->GetBinContent(i,j);

          if( (DataBin+MCBin) > 0 ) diff = TMath::Abs( (DataBin-MCBin)/ (DataBin+MCBin) );
          if (diff > 0.6 ) h2->SetBinContent(i,j,TMath::Abs(1-(DataBin-MCBin))*diff/2);
        }
      }
      // Int_t palette[5];
      
      // palette[0] = 20;
      // palette[1] = 23;
      // palette[2] = 31;
      // palette[3] = 34;
      // palette[4] = 13;
      // gStyle->SetPalette(5,palette);
      // gStyle->SetPalette(91);
      h2->DrawCopy("COLZ");
      cTmp.Print(Form("displays/%s/ESDclusterMapChamber%d.png",runNumber.Data(),iCh), "png");

      // //Saving first image
      // cTmp.Clear();
      // gStyle->SetOptStat(0);
      // cTmp.cd(1);
      // clusterMapData->DrawNormalized("COLZ",1./clusterMapData->GetEntries());
      // cTmp.Print(Form("displays/%s/ESDclusterMapChamber%d/clusterMapData.png",runNumber.Data(),iCh), "png");

      // //Saving Second image
      // cTmp.Clear();
      // gStyle->SetOptStat(0);
      // cTmp.cd(1);
      // clusterMapMC->DrawNormalized("COLZ",1./clusterMapMC->GetEntries());
      // cTmp.Print(Form("displays/%s/ESDclusterMapChamber%d/clusterMapMC.png",runNumber.Data(),iCh), "png");

      // //Compare the two hitmap
      // gSystem->Exec(Form("compare -alpha Background -highlight-color Brown -fuzz 10%% displays/%s/ESDclusterMapChamber%d/clusterMapData.png displays/%s/ESDclusterMapChamber%d/clusterMapMC.png displays/%s/ESDclusterMapChamber%dDifference.png",runNumber.Data(),iCh,runNumber.Data(),iCh,runNumber.Data(),iCh));

      // // Delete temporary folder
      // gSystem->Exec(Form("rm -dr displays/%s/ESDclusterMapChamber%d",runNumber.Data(),iCh));

      // //Merging images
      // gSystem->Exec(Form("montage displays/%s/ESDclusterMapChamber%d.png displays/%s/ESDclusterMapChamber%dDifference.png \
      //                    -geometry +1+1+1  -background none  displays/%s/ESDclusterMapChamber%dFinal.png",runNumber.Data(),iCh,runNumber.Data(),iCh,runNumber.Data(),iCh));

      delete h2;

    }

    currDataFile->Close();
    currMCFile->Close();
  }

  inDataFile.close();
  inMCFile.close();

  cout << "done                     " << endl;

}

//----------------------------------------------------------------------------
void CountTracks(TString inputFile)
{
  /// return the number of tracker track per run in the reconstruction QA

  ifstream inFile(inputFile.Data()); // open input file
  if (!inFile.is_open())
  {
    printf("cannot open file %s\n",inputFile.Data());
    return;
  }

  TString currName;
  Int_t nRuns = 0;
  TArrayI runs(10000);
  Int_t nTracksTot = 0;
  TArrayI nTracks(10000);
  TList badFiles;
  badFiles.SetOwner(kTRUE);

  while (!inFile.eof()) //loop over elements in input file
  {
    currName.ReadLine(inFile,kTRUE); // Read line in input Data file
    if(currName.IsNull()) continue;

    TString runNumber = currName;
    Int_t c = runNumber.Last('/');
    runNumber.Remove(c);
    c = runNumber.Last('/');
    runNumber.Remove(0,c+1);

    cout<<"\rprocessing run "<<runNumber.Data()<<" ...\r"<<flush;

    TFile* currFile = new TFile(currName.Data(),"read");
    if (!currFile || !currFile->IsOpen()) continue;

    TH1* hnClusters = 0x0;
    if (isQATask) {
      TObjArray *objs = static_cast<TObjArray*>(currFile->Get("MUON_QA/general1"));
      if (objs) hnClusters = static_cast<TH1*>(objs->FindObject("hNClustersPerTrack"));
    } else
      hnClusters = static_cast<TH1*>(currFile->Get(Form("MUON/ESDs/%s/%s_hESDnClustersPerTrack",
							MultDirData.Data(),MultDirData.Data())));

    if (!hnClusters) {
      currFile->Close();
      badFiles.AddLast(new TObjString(currName.Data()));
      continue;
    }

    runs[nRuns] = runNumber.Atoi();
    nTracks[nRuns] = (Int_t)hnClusters->GetEntries();
    nTracksTot += nTracks[nRuns];
    nRuns++;

    currFile->Close();
  }

  cout << "done                     " << endl;
  inFile.close();

  Float_t percentTot = 0.;
  for (Int_t i = 0; i < nRuns; i++) {
    Float_t percent = 100.*((Float_t)nTracks[i])/((Float_t)nTracksTot);
    printf("%d %9d   %05.2f%%\n", runs[i], nTracks[i], percent);
    percentTot += percent;
  }
  printf("\ntotal number of tracks in the %d good runs = %d (%06.2f%%)\n", nRuns, nTracksTot, percentTot);

  if (badFiles.GetEntries() > 0) {
    printf("\nlist of bad files:\n");
    badFiles.Print();
  }

}

