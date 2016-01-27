//
//  ComparisonEfficiency
//  
//
//  Created by Benjamin Audurier on 28/10/15.
//
//

#include <stdio.h>
#include <TH1.h>
#include <TH2.h>
#include <THnSparse.h>
#include <TAxis.h>
#include <TString.h>
#include <TObjString.h>
#include <Riostream.h>
#include <TFile.h>
#include <TList.h>
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>
#include <TArrayD.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TLegend.h>

#include "AliMpDEIterator.h"
#include "AliMUONCDB.h"
#include "AliCDBManager.h"
#include "AliMpDEManager.h"

#include <TCanvas.h>


#include <TFitResult.h>
#include <TH2.h>
#include <TProfile.h>
#include <TLegend.h>
#include <TList.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>

//This task is intended to compare two root file from macro Comparison1Mc

TGraphAsymmErrors* CreateRatioGraph(const char* name, const char* title, TGraphAsymmErrors& Graph1, TGraphAsymmErrors& Graph2) // Change this function as in RealEfficiency.C (value)
{
  if (Graph1.GetN() != Graph2.GetN() )
  {
    printf("Error, dividing graphs of different entries number : NGraph1 = %d NGraph2 = %d \n", Graph1.GetN(),Graph2.GetN());
    return 0x0;
  }
  
  // compute efficiency ratio file1/file2
  Int_t nBins = Graph1.GetN();
  TGraphAsymmErrors *ratio = new TGraphAsymmErrors(nBins);
  Double_t x,effD,effS,effDErrh,effDErrl,effSErrh,effSErrl,rat,ratErrh,ratErrl;
  
  // Set ratio's Xaxis lenght, name and title
  
  ratio->SetName(name);
  ratio->SetTitle(title);
  
  // Loop over bins
  for (Int_t i = 0; i < nBins; i++)
  {
    // Get points and errors from individual efficiencies
    Graph1.GetPoint(i,x,effD);
    Graph2.GetPoint(i,x,effS);
    effDErrh = Graph1.GetErrorYhigh(i);
    effDErrl = Graph1.GetErrorYlow(i);
    effSErrh = Graph2.GetErrorYhigh(i);
    effSErrl = Graph2.GetErrorYlow(i);
    
    // Compute the ratio and assym errors
    if (effD > 0. && effS > 0.)
    {
      rat = effD/effS;
      ratErrh = rat*TMath::Sqrt(effDErrh*effDErrh/effD*effD + effSErrl*effSErrl/effS*effS);
      ratErrl = rat*TMath::Sqrt(effDErrl*effDErrl/effD*effD + effSErrh*effSErrh/effS*effS);
    }
    if (effD == 0 && effS == 0)
    {
      rat = 1.;
      ratErrh = 0.;
      ratErrl = 0.;
    }
    if (effD == 0 && effS > 0.)
    {
      rat = 0.;
      ratErrh = 0.;
      ratErrl = 0.;
    }
    if (effD > 0. && effS == 0)
    {
      rat = 2.;
      ratErrh = 0.;
      ratErrl = 0.;
    }
    // Fill the ratio Graph
    ratio->SetPoint(i,x,rat);
    ratio->SetPointError(i,0.,0.,ratErrl,ratErrh);
  }

  ratio->GetXaxis()->SetTitle(Graph1.GetXaxis()->GetTitle());
  ratio->GetYaxis()->SetTitle("Ratio");

  if ( !strncmp(Graph1.GetXaxis()->GetTitle(),"run number",10) )
  {
    ratio->GetXaxis()->Set(nBins, -0.5, nBins-0.5);
  
    
    for (Int_t i = 1; i <= nBins; i++)
    {
      // set bin labels
      ratio->GetXaxis()->SetBinLabel(i, Graph1.GetXaxis()->GetBinLabel(i));
      
    }
  }

  if ( !strncmp(Graph1.GetXaxis()->GetTitle(),"Detection Element",10) )
  {
    ratio->GetXaxis()->Set(nBins+1, -0.5, nBins-0.5); //Set the length of the x axis for an appropiate display of DE
    ratio->GetXaxis()->SetNdivisions(nBins);    
  }
  return ratio;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
void BeautifyGraphs(TObjArray& array, const char* yAxisName) // Is differrent from the one in RealEfficiency.C since now the xaxis name is set in CreateRatioGraph
{
  TGraphAsymmErrors* g;
  
  for ( Int_t i = 0 ; i <= array.GetLast() ; ++ i )
  {
    g = static_cast<TGraphAsymmErrors*>(array.At(i));
  
    g->SetLineStyle(1);
    g->SetLineColor(1);
    g->SetMarkerStyle(20);
    g->SetMarkerSize(0.7);
    g->SetMarkerColor(2);
    g->GetXaxis()->CenterTitle(kTRUE);
    g->GetXaxis()->SetLabelFont(22);
    g->GetXaxis()->SetTitleFont(22);
    g->GetYaxis()->SetTitle(yAxisName);
    g->GetYaxis()->CenterTitle(kTRUE);
    g->GetYaxis()->SetLabelFont(22);
    g->GetYaxis()->SetTitleFont(22);
  }
}

// Draw
//---------------------------------------------------------------------------
TCanvas* DrawRatio(TString name, TString title, TGraphAsymmErrors* Graph1, TGraphAsymmErrors * Graph2, TGraphAsymmErrors* GraphRatio, TString Name1 = "result1", TString Name2 = "result2")
{ 
  Float_t fracOfHeight = 0.3;
  Float_t rightMargin = 0.03;
  TCanvas *c = new TCanvas(name.Data(), "ratio",1200,800);

  c->Divide(1,2,0,0);
  
  c->cd(1);
  
  gPad->SetPad(0., fracOfHeight, 0.99, 0.99);
  gPad->SetTopMargin(0.03);
  gPad->SetRightMargin(rightMargin);
  
  Double_t min1 = Graph1->GetYaxis()->GetXmin();
  Double_t min2 = Graph2->GetYaxis()->GetXmin();
  Double_t max1 = Graph1->GetYaxis()->GetXmax();
  Double_t max2 = Graph2->GetYaxis()->GetXmax();
  Double_t minA = TMath::Min(min1,min2);
  if (minA < 0.1) minA -= 0.1;
  Double_t maxA = TMath::Max(max1,max2);
  
  Graph1->SetMinimum(minA);
  Graph1->SetMaximum(maxA);
  //Graph1->SetMinimum(minA-0.1*minA);
  //Graph1->SetMaximum(maxA+0.1*maxA);
  Graph1->SetTitle(title.Data());
  Graph1->GetYaxis()->SetLabelSize(0.051);
  Graph1->GetYaxis()->SetTitleSize(0.051);
  Graph1->GetYaxis()->SetTitleOffset(0.8);
  Graph1->GetYaxis()->CenterTitle(kTRUE);
  Graph1->GetXaxis()->SetLabelOffset(0.1);
  Graph1->SetMarkerStyle(20);
  Graph1->SetMarkerSize(0.6);
  Graph1->SetMarkerColor(4);
  Graph1->SetLineColor(4);
  Graph1->Draw("AP");
  
  
  Graph2->SetMarkerStyle(20);
  Graph2->SetMarkerSize(0.6);
  Graph2->SetMarkerColor(2);
  Graph2->SetLineColor(2);
  Graph2->Draw("Psame");
   //------
//  Graph2.Draw("same");
  TLegend *legend = new TLegend (0.8, 0.8, 0.95, 0.95);
  legend->SetTextSize(0.06);
  legend->AddEntry(Graph2, Form(" %s",Name2.Data()), "ep");
  legend->AddEntry(Graph1, Form(" %s",Name1.Data()), "ep");
  legend->Draw("same");

//  GraphRatio->Draw("psamey+");
  
  c->cd(2);
  
  gPad->SetPad(0., 0., 0.99, fracOfHeight);
  gPad->SetRightMargin(rightMargin);
  gPad->SetBottomMargin(0.08/fracOfHeight);
  gPad->SetGridy();
  
  //------

  GraphRatio->SetLineStyle(1);
  GraphRatio->SetLineColor(1);
  GraphRatio->SetMarkerStyle(20);
  GraphRatio->SetMarkerSize(0.4);
  GraphRatio->SetMarkerColor(1);
  GraphRatio->GetXaxis()->SetLabelSize(0.11);
  GraphRatio->GetXaxis()->SetTitleSize(0.12);

  GraphRatio->GetXaxis()->CenterTitle(kTRUE);
  GraphRatio->GetXaxis()->SetLabelFont(22);
  GraphRatio->GetXaxis()->SetTitleFont(22);
  
  GraphRatio->GetYaxis()->SetLabelSize(0.07);
  GraphRatio->GetYaxis()->SetTitleSize(0.07);
  GraphRatio->GetYaxis()->CenterTitle(kTRUE);
  GraphRatio->GetYaxis()->SetTitleOffset(0.37);
  GraphRatio->GetYaxis()->SetLabelFont(22);
//  GraphRatio.SetMinimum(0.86);
//  GraphRatio.SetMaximum(1.14);
  GraphRatio->Draw("ap");
  //------
//  GraphRatio.Draw("same");

  TLegend *legend2 = new TLegend (0.70, 0.3, 0.95, 0.37);
  legend2->AddEntry(GraphRatio, Form(" %s / %s ",Name1.Data(), Name2.Data()), "ep");
  legend2->Draw("same");
  c->Update();
  //------
  
  return c;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
void ComparisonEfficiency(TString f1 = "", TString f2 ="")
{
  // Open input file1
  TFile *file1 = new TFile(f1.Data(), "read");
  if (!file1 || !file1->IsOpen()) {
    printf("cannot open file %s \n",f1.Data());
    return;
  }
  // Open input file2
  TFile *file2 = new TFile(f2.Data(), "read");
  if (!file2 || !file2->IsOpen()) {
    printf("cannot open file %s \n",f2.Data());
    return;
  }
  
  TString hname = "Global";
   
  TObjArray *listGlobalEff1 = static_cast<TObjArray*>(file1->FindObjectAny(Form("%sEffRatios",hname.Data())));
  if (!listGlobalEff1) {
    printf("list of Global efficiencies from 1 not found\n");
    return;
  }
  
  TObjArray *listGlobalEff2 = static_cast<TObjArray*>(file2->FindObjectAny(Form("%sEffRatios",hname.Data())));
  if (!listGlobalEff2) {
    printf("list of Global efficiencies from 2 not found\n");
    return;
  }
  
  // Get Global 1 and 2 graphs
  
//  TGraphAsymmErrors *effVScent1 = static_cast<TGraphAsymmErrors*>(file1->FindObjectAny("trackingEffVscentrality"));
//  if (!effVScent1) {
//    printf("Efficiency vs centrality from 1 not found\n");
//    return;
//  }
//  TGraphAsymmErrors *effVScent2 = static_cast<TGraphAsymmErrors*>(file2->FindObjectAny("trackingEffVscentrality"));
//  if (!effVScent2) {
//    printf("Efficiency vs centrality from 2 not found\n");
//    return;
//  }
  
  TGraphAsymmErrors *effVSrun1 = static_cast<TGraphAsymmErrors*>(listGlobalEff1->FindObject("RatioEffVsRun"));
  if (!effVSrun1) {
    printf("Efficiency vs run from 1 not found\n");
    return;
  }
  TGraphAsymmErrors *effVSrun2 = static_cast<TGraphAsymmErrors*>(listGlobalEff2->FindObject("RatioEffVsRun"));
  if (!effVSrun2) {
    printf("Efficiency vs run from 2 not found\n");
    return;
  }
  
  
  TGraphAsymmErrors *effVSy1 = static_cast<TGraphAsymmErrors*>(listGlobalEff1->FindObject("RatioEffVsY"));
  if (!effVSy1) {
    printf("Efficiency vs rapidity from 1 not found\n");
    return;
  }
  TGraphAsymmErrors *effVSy2 = static_cast<TGraphAsymmErrors*>(listGlobalEff2->FindObject("RatioEffVsY"));
  if (!effVSy2) {
    printf("Efficiency vs rapidity from 2 not found\n");
    return;
  }

  
  TGraphAsymmErrors *effVSpt1 = static_cast<TGraphAsymmErrors*>(listGlobalEff1->FindObject("RatioEffVsPt"));
  if (!effVSpt1) {
    printf("Efficiency vs pt from 1 not found\n");
    return;
  }
  TGraphAsymmErrors *effVSpt2 = static_cast<TGraphAsymmErrors*>(listGlobalEff2->FindObject("RatioEffVsPt"));
  if (!effVSpt2) {
    printf("Efficiency vs pt from 2 not found\n");
    return;
  }
  
  TGraphAsymmErrors *effVSphi1 = static_cast<TGraphAsymmErrors*>(listGlobalEff1->FindObject("RatioEffVsPhi"));
  if (!effVSphi1) {
    printf("Efficiency vs phi from 1 not found\n");
    return;
  }
  TGraphAsymmErrors *effVSphi2 = static_cast<TGraphAsymmErrors*>(listGlobalEff2->FindObject("RatioEffVsPhi"));
  if (!effVSphi2) {
    printf("Efficiency vs phi from 2 not found\n");
    return;
  }

  // Create an array list with the global ratios
  TObjArray globalRatios;
  
  // Create an array with the global plots of the individual efficencies and the ratios
  TObjArray globalRatiosAndEff;
  
//  globalRatios.Add(CreateRatioGraph("RatioEffVsCent","file1/file2 tracking efficiency versus centrality",effVScent1,effVScent2));
  
  //---- Eff vs run
  TGraphAsymmErrors* effVSrun1Copy = static_cast<TGraphAsymmErrors*>(effVSrun1->Clone()); // We make clones to do not modify them
  TGraphAsymmErrors* effVSrun2Copy = static_cast<TGraphAsymmErrors*>(effVSrun2->Clone()); 

  TGraphAsymmErrors *ratioRun = CreateRatioGraph("RatioEffVsRun","file1/file2 tracking efficiency versus run",*effVSrun1,*effVSrun2);
  globalRatios.Add(ratioRun);
  
  TGraphAsymmErrors* ratioRunCopy = static_cast<TGraphAsymmErrors*>(ratioRun->Clone());
  
  globalRatiosAndEff.Add(DrawRatio("RatioEffVSRunAndEff","Comparison file1&file2 tracking efficiency versus run", effVSrun1Copy,effVSrun2Copy,ratioRunCopy));
  //-----


  //---- Eff vs y
  TGraphAsymmErrors* effVSy1Copy = static_cast<TGraphAsymmErrors*>(effVSy1->Clone()); // We make clones to do not modify them
  TGraphAsymmErrors* effVSy2Copy = static_cast<TGraphAsymmErrors*>(effVSy2->Clone());
  
  TGraphAsymmErrors *ratioY = CreateRatioGraph("RatioEffVsY","file1/file2 tracking efficiency versus rapidity",*effVSy1,*effVSy2);
  globalRatios.Add(ratioY);
  
  TGraphAsymmErrors* ratioYCopy = static_cast<TGraphAsymmErrors*>(ratioY->Clone());
 
  globalRatiosAndEff.Add(DrawRatio("RatioEffVSyAndEff","Comparison file1&file2 tracking efficiency versus rapidity", effVSy1Copy,effVSy2Copy,ratioYCopy));
  //-----

  //-----Eff vs Pt
  TGraphAsymmErrors* effVSpt1Copy = static_cast<TGraphAsymmErrors*>(effVSpt1->Clone()); // We make clones to do not modify them
  TGraphAsymmErrors* effVSpt2Copy = static_cast<TGraphAsymmErrors*>(effVSpt2->Clone());

  TGraphAsymmErrors *ratioPt = CreateRatioGraph("RatioEffVsPt","File1/File2 tracking efficiency versus Pt",*effVSpt1,*effVSpt2);
  globalRatios.Add(ratioPt);
  
  TGraphAsymmErrors* ratioPtCopy = static_cast<TGraphAsymmErrors*>(ratioPt->Clone());
  
   globalRatiosAndEff.Add(DrawRatio("RatioEffVSptAndEff","Comparison file1&file2 tracking efficiency versus Pt",effVSpt1Copy,effVSpt2Copy,ratioPtCopy));
  //-----
  
  //----Eff vs phi
  TGraphAsymmErrors* effVSphi1Copy = static_cast<TGraphAsymmErrors*>(effVSphi1->Clone()); // We make clones to do not modify them
  TGraphAsymmErrors* effVSphi2Copy = static_cast<TGraphAsymmErrors*>(effVSphi2->Clone());
  
  TGraphAsymmErrors *ratioPhi = CreateRatioGraph("RatioEffVsPhi","file1/file2 tracking efficiency versus phi",*effVSphi1,*effVSphi2);
  globalRatios.Add(ratioPhi);
  
  TGraphAsymmErrors* ratioPhiCopy = static_cast<TGraphAsymmErrors*>(ratioPhi->Clone());
  
  globalRatiosAndEff.Add(DrawRatio("RatioEffVSphiAndEff","Comparison file1&file2 tracking efficiency versus phi",effVSphi1Copy,effVSphi2Copy,ratioPhiCopy));
  //-------
  
//   //----Eff vs y vs phi

//   TH2F *effVSyVSphi1 = static_cast<TH2F*>(file1->FindObjectAny("trackingEffVsphi-y"));
//   if (!effVSyVSphi1) {
//     printf("Efficiency vs rapidity vs phi from 1 not found\n");
//     return;
//   }
//   TH2F *effVSyVSphi2 = static_cast<TH2F*>(file2->FindObjectAny("trackingEffVsphi-y"));
//   if (!effVSyVSphi2) {
//     printf("Efficiency vs rapidity vs phi from 2 not found\n");
//     return;
//   }
//   Int_t nBins2dX = effVSyVSphi1->GetXaxis()->GetNbins();
//   Int_t nBins2dY = effVSyVSphi1->GetYaxis()->GetNbins();
//   Double_t eff12D,eff22D,ratio2D;
  
//   TH2F *effVSphiVSyRatio = new TH2F("RatioEffVSphiVSy","Eff1/Eff2 vs phi vs y",nBins2dX, effVSyVSphi1->GetXaxis()->GetBinLowEdge(1), effVSyVSphi1->GetXaxis()->GetBinUpEdge(nBins2dX),nBins2dY, effVSyVSphi1->GetYaxis()->GetBinLowEdge(1), effVSyVSphi1->GetYaxis()->GetBinUpEdge(nBins2dY));
//   effVSphiVSyRatio->GetXaxis()->SetTitle("phi");
//   effVSphiVSyRatio->GetYaxis()->SetTitle("y");
    
//   for (Int_t i = 1 ; i <= nBins2dX ; i++ )
//   {
//     for (Int_t j = 1 ; j <= nBins2dY ; j++ )
//     {
//       eff12D = effVSyVSphi1->GetBinContent(i,j);
//       eff22D = effVSyVSphi2->GetBinContent(i,j);

//       if (eff12D > 0. && eff22D > 0.)
//       {
//         ratio2D = eff12D/eff22D;
// //        ratio2DErrh = rat*TMath::Sqrt(effDErrh*effDErrh/effD*effD + effSErrl*effSErrl/effS*effS);
// //        ratio2DErrl = rat*TMath::Sqrt(effDErrl*effDErrl/effD*effD + effSErrh*effSErrh/effS*effS);
//       }
//       if (eff12D == 0 && eff22D == 0)
//       {
//         ratio2D = 1.;
// //        ratio2DErrh = 0.;
// //        ratio2DErrl = 0.;
//       }
//       if (eff12D == 0 && eff22D > 0.)
//       {
//         ratio2D = 0.;
// //        ratio2DErrh = 0.;
// //        ratio2DErrl = 0.;
//       }
//       if (eff12D > 0. && eff22D == 0)
//       {
//         ratio2D = 2.;
// //        ratio2DErrh = 0.;
// //        ratio2DErrl = 0.;
//       }
//       effVSphiVSyRatio->SetBinContent(i,j,ratio2D);
//     }
//   }
  
  
//   TH2F *effVSphiVSyRatioRapBins = new TH2F();
//   effVSphiVSyRatioRapBins->GetXaxis()->SetTitle("phi");
//   effVSphiVSyRatioRapBins->GetYaxis()->SetTitle("y");
//   effVSphiVSyRatioRapBins->SetName("RatioEffVSphiVSyRapBins");
//   effVSphiVSyRatioRapBins->SetTitle("Eff1/Eff2 vs phi vs y");

  
//   Int_t nxBins = effVSphiVSyRatio->GetXaxis()->GetNbins();
//   Int_t nyBins = effVSphiVSyRatio->GetYaxis()->GetNbins();
  
//   Double_t xBinEdge[nxBins+1];
//   Double_t yBinEdge[nyBins+1];
  
//   for (Int_t ybin = 0 ; ybin <= nyBins ; ybin++)
//   {
//     yBinEdge[ybin] = 2*TMath::ATan(TMath::Exp((effVSphiVSyRatio->GetYaxis()->GetBinLowEdge(ybin+1))));
//   }
//   for (Int_t xbin = 0 ; xbin <= nxBins ; xbin++)
//   {
//     xBinEdge[xbin] = effVSphiVSyRatio->GetXaxis()->GetBinLowEdge(xbin+1);
//   }
  
//   effVSphiVSyRatioRapBins->SetBins(nxBins,xBinEdge,nyBins,yBinEdge);
  
//   for (Int_t xbin = 1 ; xbin <= nxBins ; xbin++)
//   {
//     for (Int_t ybin = 1 ; ybin <= nyBins ; ybin++)
//     {
//       effVSphiVSyRatioRapBins->SetBinContent(xbin,ybin,effVSphiVSyRatio->GetBinContent(xbin,ybin));
//     }
//   }
//   globalRatiosAndEff.Add(effVSphiVSyRatio);
//   globalRatiosAndEff.Add(effVSphiVSyRatioRapBins);

 
//   //--------
  
  TString hname2 = "Chamber";
  
  // Get Chamber and DE 1 and 2 graphs
//  TObjArray *listChEffVSrun1 = static_cast<TObjArray*>(file1->FindObjectAny("ChambersEffVSrun"));
//  if (!listChEffVSrun1) {
//    printf("list of Chamber efficiencies vs run from 1 not found\n");
//    return;
//  }
//  TObjArray *listChEffVSrun2 = static_cast<TObjArray*>(file2->FindObjectAny("ChambersEffVSrun"));
//  if (!listChEffVSrun2) {
//    printf("list of Chamber efficiencies vs run from 2 not found\n");
//    return;
//  }

  TObjArray *listChEffVSDE1 = static_cast<TObjArray*>(file1->FindObjectAny(Form("%sEffperDERatios",hname2.Data())));
  if (!listChEffVSDE1) {
    printf("list of Chamber efficiencies per DE from 1 not found\n");
    return;
  }
  TObjArray *listChEffVSDE2 = static_cast<TObjArray*>(file2->FindObjectAny(Form("%sEffperDERatios",hname2.Data())));
  if (!listChEffVSDE2) {
    printf("list of Chamber efficiencies per DE from 2 not found\n");
    return;
  }
  
//  TObjArray *listDEEffVSrun1 = static_cast<TObjArray*>(file1->FindObjectAny("DEEffVSrun"));
//  if (!listDEEffVSrun1) {
//    printf("list of DE efficiencies vs run from 1 not found\n");
//    return;
//  }
//  TObjArray *listDEEffVSrun2 = static_cast<TObjArray*>(file2->FindObjectAny("DEEffVSrun"));
//  if (!listDEEffVSrun2) {
//    printf("list of DE efficiencies vs run from 2 not found\n");
//    return;
//  }
  
  // Graph for global efficiency vs run
  TGraphAsymmErrors* g1 ;//= static_cast<TGraphAsymmErrors*>(listChEffVSrun1->At(0));
  TGraphAsymmErrors* g2 ;//= static_cast<TGraphAsymmErrors*>(listChEffVSrun2->At(0));
  
  
  //----Eff vs run  
//  TGraphAsymmErrors *ratioEffvsrRun = CreateRatioGraph("RatioEffVsRun","file1/file2 tracking efficiency versus run",*g1,*g2);
//  globalRatios.Add(ratioEffvsrRun);
//  
//  TGraphAsymmErrors* ratioEffvsrRunCopy = static_cast<TGraphAsymmErrors*>(ratioEffvsrRun->Clone());
//  
//  globalRatiosAndEff.Add(DrawRatio("RatioEffVsRunAndEff","Comparison file1&file2 tracking efficiency versus run",g1,g2,ratioEffvsrRunCopy));
  //-------

  //globalRatios.Add(CreateRatioGraph("RatioEffVsRun","file1/file2 tracking efficiency versus run",*g1,*g2));
  
  // Create a list with the Chamber and DE ratios
//  TObjArray chamberVSrunRatios;
//  TObjArray deVSrunRatios;
  TObjArray chamberVSdeRatios;
  
//  TObjArray chamberVSrunRatiosAndEff;
//  TObjArray deVSrunRatiosAndEff;
  TObjArray chamberVSdeRatiosAndEff;
  
  // Compute the ratios for Chamber vs run
//  for (Int_t nList = 1 ; nList < listChEffVSrun1->GetEntries() ; nList++)
//  {
//    g1 = static_cast<TGraphAsymmErrors*>(listChEffVSrun1->At(nList));
//    g2 = static_cast<TGraphAsymmErrors*>(listChEffVSrun2->At(nList));
//    if (!g1 || !g2 )
//    {
//      printf("Error readig from Chamber efficiency vs run list \n");
//      return;
//    }
//    //----Eff of Chs vs run
//    TString name =  Form("RatioEffCh%dVsRun",nList); TString title = Form("Chamber %d file1/file2 tracking efficiency versus run",nList);
//    
//    TGraphAsymmErrors *ratioEffChVsrRun = CreateRatioGraph(name.Data(),title.Data(),*g1,*g2);
//    chamberVSrunRatios.Add(ratioEffChVsrRun);
//    
//    TGraphAsymmErrors* ratioEffChVsrRunCopy = static_cast<TGraphAsymmErrors*>(ratioEffChVsrRun->Clone());
//    
//    TString nameRatio =  Form("RatioEffCh%dVsRunAndEff",nList); TString titleRatio = Form("Comparison file1&file2 Ch%d tracking efficiency versus run",nList);
//    chamberVSrunRatiosAndEff.Add(DrawRatio(nameRatio.Data(),titleRatio.Data(),g1,g2,ratioEffChVsrRunCopy));
//    //-------
//
//    
////    chamberVSrunRatios.Add(CreateRatioGraph(,,*g1,*g2));
//    
//  }
  
  //Load the mapping for the DE histos
  AliCDBManager::Instance()->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
  AliCDBManager::Instance()->SetRun(0);
  AliMUONCDB::LoadMapping();
  AliMpDEIterator deit;

  // Loop over Chambers
  for (Int_t ich = 0 ; ich < 10 ; ich++)
  {
    // Compute the ratios for DE vs run
    deit.First(ich);
  
//    while ( !deit.IsDone() )
//    {
//      TString currentDEName = Form("EffDE%dVSrun",deit.CurrentDEId());
//      g1 = static_cast<TGraphAsymmErrors*>(listDEEffVSrun1->FindObject(currentDEName.Data()));
//      g2 = static_cast<TGraphAsymmErrors*>(listDEEffVSrun2->FindObject(currentDEName.Data()));
//      
//      TString name =  Form("RatioEffDE%dVsRun",deit.CurrentDEId()); TString title = Form("DE %d file1/file2 tracking efficiency versus run",deit.CurrentDEId());
//      if (!g1 || !g2 )
//      {
//        printf("Error readig from DE efficiency vs run list \n");
//        return;
//      }
//      //----Eff of DEs vs run
//      TGraphAsymmErrors *ratioEffDEvsRun = CreateRatioGraph(name.Data(),title.Data(),*g1,*g2);
//      deVSrunRatios.Add(ratioEffDEvsRun);
//      
//      TGraphAsymmErrors* ratioEffDEvsRunCopy = static_cast<TGraphAsymmErrors*>(ratioEffDEvsRun->Clone());
//      
//      TString nameRatio =  Form("RatioEffDE%dVsRunAndEff",deit.CurrentDEId()); TString titleRatio = Form("Comparison file1&file2 DE%d tracking efficiency versus run",deit.CurrentDEId());
//      deVSrunRatiosAndEff.Add(DrawRatio(nameRatio.Data(),titleRatio.Data(),g1,g2,ratioEffDEvsRunCopy));
//      //-------
//
////      deVSrunRatios.Add(CreateRatioGraph(name.Data(),title.Data(),*g1,*g2));
//      
//      deit.Next();
//    }
  
    // Compute the ratios for Ch vs DE
    TString hname3 = "RatioEff";
    g1 = static_cast<TGraphAsymmErrors*>(listChEffVSDE1->FindObject(Form("%sCh%dVsDE",hname3.Data(),ich+1)));
    g2 = static_cast<TGraphAsymmErrors*>(listChEffVSDE2->FindObject(Form("%sCh%dVsDE",hname3.Data(),ich+1)));
    
    if (!g1 || !g2 )
    {
      printf("Error reading from Chamber efficiency per DE list \n");
      return;
    }
    TString name =  Form("DiffRatioEffCh%dVsDE",ich+1); TString title = Form("Chamber %d file1/file2 tracking efficiency versus DE",ich+1);
    //----Eff of CHs vs DE
    TGraphAsymmErrors *ratioEffChvsDE = CreateRatioGraph(name.Data(),title.Data(),*g1,*g2);
    chamberVSdeRatios.Add(ratioEffChvsDE);
    
    TGraphAsymmErrors* ratioEffChvsDECopy = static_cast<TGraphAsymmErrors*>(ratioEffChvsDE->Clone());
    
    TString nameRatio =  Form("RatioEffCh%dVsDEAndEff",ich+1); TString titleRatio = Form("Comparison file1&file2 Ch%d tracking efficiency versus DE",ich+1);
    chamberVSdeRatiosAndEff.Add(DrawRatio(nameRatio.Data(),titleRatio.Data(),g1,g2,ratioEffChvsDECopy));
    //-------

    
//    chamberVSdeRatios.Add(CreateRatioGraph(name.Data(),title.Data(),*g1,*g2));
    
  }

  //Beautify graphs
  BeautifyGraphs(globalRatios,"Eff1/Eff2");
//  BeautifyGraphs(deVSrunRatios,"Eff1/Eff2");
//  BeautifyGraphs(chamberVSrunRatios,"Eff1/Eff2");
  BeautifyGraphs(chamberVSdeRatios,"Eff1/Eff2");

//  BeautifyGraphs(globalRatiosAndEff,"Eff1/Eff2");
//  BeautifyGraphs(deVSrunRatiosAndEff,"Eff1/Eff2");
//  BeautifyGraphs(chamberVSrunRatiosAndEff,"Eff1/Eff2");
//  BeautifyGraphs(chamberVSdeRatiosAndEff,"Eff1/Eff2");

  // set bin labels
//  SetRunLabel(deVSrunRatios,irun,runs);
//  SetRunLabel(chamberVSrunRatios,irun,runs);
//  SetRunLabel(globalRatios,irun,runs,1); //Write it in such a way the number is the position on the list of the graph you want to label
//  
  // save output
  TFile* file = new TFile("ComparisonEfficiency.root","update");
  
  globalRatios.Write("GlobalEffComp", TObject::kOverwrite | TObject::kSingleKey);
//  chamberVSrunRatios.Write("ChambersEffVSrunRatios", TObject::kOverwrite | TObject::kSingleKey);
//  deVSrunRatios.Write("DEEffVSrunRatios", TObject::kOverwrite | TObject::kSingleKey);
  chamberVSdeRatios.Write("ChamberEffperDEComp", TObject::kOverwrite | TObject::kSingleKey);
  
  globalRatiosAndEff.Write("GlobalEffCompAndEffs", TObject::kOverwrite | TObject::kSingleKey);
//  chamberVSrunRatiosAndEff.Write("ChambersEffVSrunRatiosAndEff", TObject::kOverwrite | TObject::kSingleKey);
//  deVSrunRatiosAndEff.Write("DEEffVSrunRatiosAndEff", TObject::kOverwrite | TObject::kSingleKey);
  chamberVSdeRatiosAndEff.Write("ChamberEffperDECompAndEff", TObject::kOverwrite | TObject::kSingleKey);

  
   
  file->Close();
 
  file1->Close();
  file2->Close();
   


}