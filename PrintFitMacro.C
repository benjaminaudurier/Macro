//
//  PrintFitMacro.c
//  
//
//  Created by Benjamin Audurier on 09/06/15.
//
//

// Macro who prints fit results

#include <AliLog.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <AliAnalysisMuMu.h>
#include <AliAnalysisMuMuSpectra.h>
#include <AliMergeableCollection.h>
#include <AliCounterCollection.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TROOT.h>
#include <iostream> 

//Some strings and constants
char                   * sfile="AnalysisResultsNew.root";
char                   * sasso="";
char                   * sasso2="";
char                   * beamYear="mumu.PbPb2015.config";

TString striggerDimuon ="CMUL7-B-NOPF-MUFAST";
TString striggerMB     ="CINT7-B-NOPF-MUFAST";
TString seventType     ="PSALL";
TString spairCut       ="pALLPAIRYPAIRPTIN0.0-10.0RABSMATCHLOWETAPDCA";
TString scentrality    ="V0M_00.00_90.00";

Double_t FNorm         =15.22;
Double_t BR            =0.005;

void PrintDist(TObjString* swhat,Bool_t yield,AliAnalysisMuMu &ana);
void PrintYield(AliAnalysisMuMuSpectra *spec,AliAnalysisMuMu &an,TObjString* cent);

//_____________________________________________________________________________
void PrintFitMacro(char         * what ="PT",const char * printWhat = "distribution",int debug =0)
{    

    AliLog::SetGlobalDebugLevel(debug);
    
    Bool_t PrintDistribution = kFALSE;
    Bool_t Raa               = kFALSE; 
    Bool_t print             = kFALSE;
    Bool_t yield             = kFALSE;

    TObjArray* sprint = TString(printWhat).Tokenize(",");
    
    //Set bool
    if(sprint->FindObject("raa")) Raa                        =kTRUE;
    if(sprint->FindObject("distribution")) PrintDistribution =kTRUE;
    if(sprint->FindObject("save")) print                     =kTRUE;
    if(sprint->FindObject("yield")) yield                    =kTRUE;


    //General conf.
    TObjArray* whatArray= TString(what).Tokenize(",");
    TIter nextWhat(whatArray);
    TObjString* swhat;
    
    // main object
    AliAnalysisMuMu analysis(sfile,sasso,sasso2,beamYear);

    //_____ Draw 
    while ( ( swhat = static_cast<TObjString*>(nextWhat()) ) )
    {
        analysis.DrawFitResults("PSI",swhat->String().Data(),"histo",print);
        analysis.PrintNofParticle("PSI","NofJPsi",swhat->String(),kFALSE);
        
        if (Raa) {
            if(swhat->String().Contains("INTEGRATED")) analysis.RAAasGraphic("PSI",swhat->String().Data(),"externFile_PT.txt","externFile_CENT.txt",scentrality.Data(),kFALSE);
            else if(swhat->String().Contains("Y")) analysis.RAAasGraphic("PSI","Y","externFile_Y.txt","externFile_CENT.txt",scentrality.Data(),kFALSE);
            else if(swhat->String().Contains("PT")) analysis.RAAasGraphic("PSI","PT","externFile_PT.txt","externFile_CENT.txt",scentrality.Data(),kFALSE);
            else continue;
        }

        if (PrintDistribution) PrintDist(swhat,yield,analysis);
    }

    return ;    
} 

//___________________________________________
void PrintDist(TObjString* swhat,Bool_t yield,AliAnalysisMuMu &ana)
{
    

    TObjArray* whatCent= TString(scentrality.Data()).Tokenize(",");
    TIter nextCent(whatCent);
    TObjString* scent;

    while ( ( scent = static_cast<TObjString*>(nextCent()) ) ) 
    {
        //________Get spectra
        TString spectraPath= Form("/%s/%s/%s/%s/%s-%s",seventType.Data(),striggerDimuon.Data(),scent->String().Data(),spairCut.Data(),"PSI",swhat->String().Data());
        printf("--- Centrality = %s\n", scent->String().Data());

        AliAnalysisMuMuSpectra * spectra = static_cast<AliAnalysisMuMuSpectra*>(ana.OC()->GetObject(spectraPath.Data()));
        if(!spectra)
        {
            cout << Form("Cannot find spectra with name %s",spectraPath.Data()) <<endl;
            return;
        }
        new TCanvas;
        spectra->Plot("NofJPsi","",kTRUE)->DrawCopy("");

        if (yield) PrintYield(spectra,ana,scent); 
    }

    

}

//___________________________________________
void PrintYield(AliAnalysisMuMuSpectra *spec,AliAnalysisMuMu &an,TObjString* cent)
{
    TH1* hyield = static_cast<TH1*>(spec->Plot("NofJPsi","",kTRUE)->Clone());
    Double_t MUL = an.CC()->GetSum(Form("trigger:%s/centrality:%s/event:%s",striggerDimuon.Data(),cent->String().Data(),seventType.Data()));
    // printf("Number of MUL = %0.f\n", MUL);
    hyield->Scale(1/(MUL*FNorm*BR));

    hyield->SetTitle(Form("J/psi Yield with MUL = %0.f and FNorm = %0.2f ",MUL,FNorm));
    hyield->GetYaxis()->SetTitle(Form("Yield w/o AccxEff corr."));
    new TCanvas;
    hyield->DrawCopy("e0");
}

