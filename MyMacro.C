//
//  MyMacro.c
//  
//
//  Created by Benjamin Audurier on 03/12/14.
//
//

// Macro loadded by runMuMuResult.sh

//_____________________________________________________________________________
void FitMacro(const char* filename="LHC15g.MuMu.1.root",
                  const char* associatedSimFileName="",
                  const char* associatedSimFileName2="",
                  const char* beamYear="PbPb2011",const char* what ="", const int DebugLevel =0)
{    
	// the analysis
    AliAnalysisMuMu analysis(filename,associatedSimFileName,associatedSimFileName2,beamYear);

    //Set debug level 
    AliLog::SetGlobalDebugLevel(DebugLevel);

    // Perform fits and draw them   
    // analysis.CleanAllSpectra();
    // analysis.Config()->Print();
    // 
    
    
    // //_____ FNorm
    // analysis.ComputeIntFnormFromCounters("",kFALSE);
    // //_____ 
    
    // analysis.ComputeJpsiYield("INTEGRATED");
    
    //_____ Plot ALl Fnorm
    // TH1* h = analysis.OC()->Histo("/FNORM-CPBI1MUL-B-NOPF-MUON/PSALL/V0M_00.00_10.00/hFNormIntVSrun_CCENT_R2-B-NOPF-ALLNOTRD");
    // if (!h) return;
    // h->Scale()
    // h->Draw("Clone");
     
    //_____ Fit 
    // analysis.Jpsi("integrated","",kFALSE,kFALSE);
    // analysis.DrawFitResults("PSI","INTEGRATED","histo",kFALSE);
    // analysis.GetDistribution("INTEGRATED");

    
    // analysis.Jpsi("pt","BENJ",kFALSE,kFALSE);
    // analysis.DrawFitResults("PSI","PT","histo",kFALSE); 
    
    // analysis.Jpsi("phi","BENJ",kFALSE,kFALSE);
    // analysis.DrawFitResults("PSI","PHI","histo",kFALSE);
    // analysis.GetDistribution("PHI");

    // analysis.Jpsi("pt","BENJ",kFALSE,kFALSE);
    // analysis.DrawFitResults("PSI","PT","histo",kFALSE);
    // analysis.GetDistribution("PT");

    // analysis.PrintNofParticle("PSI","NofJPsi","INTEGRATED",kFALSE);
    // analysis.PrintNofParticle("PSI","NofJPsi","Y",kFALSE);
    // analysis.PrintNofParticle("PSI","NofJPsi","PT",kFALSE);
    //_____ 

    // //_____ Raa
    // RAA(analysis);

    //_____ 


    return ;    
}


//_____________________________________________________________________________
void RAA(AliAnalysisMuMu &ana)
{
    ana.RAAasGraphic("PSI","Y","externFile_Y.txt","externFile_CENT.txt",kFALSE);
    ana.RAAasGraphic("PSI","INTEGRATED","externFile_PT.txt","externFile_CENT.txt",kFALSE);
    ana.RAAasGraphic("PSI","PT","externFile_PT.txt","externFile_CENT.txt",kFALSE);
}

//_____________________________________________________________________________
void PrintCounters(AliAnalysisMuMu &ana)
{
    // ana.CC()->Print("trigger/centrality/event","trigger:CVHN-B-NOPF-ALLNOTRD|CVHN_R2-B-NOPF-ALLNOTRD|CCENT-B-NOPF-ALLNOTRD|CCENT_R2-B-NOPF-ALLNOTRD/centrality:V0M_00.00_07.50/event:ALL/run:169557");
    // ana.CC()->Print("trigger/centrality/event","trigger:CVHN-B-NOPF-ALLNOTRD|CVHN_R2-B-NOPF-ALLNOTRD|CCENT-B-NOPF-ALLNOTRD|CCENT_R2-B-NOPF-ALLNOTRD&0MUL/centrality:V0M_00.00_07.50/event:ALL/run:169557");
    // ana.CC()->Print("run/trigger/centrality/event","trigger:CVLN_B2-B-NOPF-ALLNOTRD|CVLN-B-NOPF-ALLNOTRD|CVLN_R1-B-NOPF-ALLNOTRD|CSEMI-B-NOPF-ALLNOTRD|CSEMI_R1-B-NOPF-ALLNOTRD&0MUL/centrality:V0M_10.00_50.00/event:ALL");
    // ana.CC()->Print("run/trigger/centrality/event","trigger:CPBI2_B1-B-NOPF-ALLNOTRD&0MUL/centrality:V0M_00.00_90.00/event:ALL");
    ana.CC()->Print("run/trigger/centrality/event","trigger:CPBI2_B1-B-NOPF-ALLNOTRD/centrality:V0M_00.00_90.00/event:ALL");

}












