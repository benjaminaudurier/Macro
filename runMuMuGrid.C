///
/// Example macro to run the AliAnalysisTaskMuMu task
///
/// \author L. Aphecetche
///

//__________Global Settings
//
// -----grid------
TString rootVersion = "v5-34-30";//v5-34-30-alice-7 
TString alirootVersion = "v5-07-19-1";
TString aliphysicsVersion = "vAN-20160105-1";

// -----For data-----
TString dataDir = "/alice/data/2015/LHC15o";
TString dataPattern = "muon_calo_pass1/AOD/*AliAOD.Muons.root";
TString runFormat = "%09d";
TString outDir = "Analysis/LHC15o/MuMuPar";


// extra to be load on CF
TString extraLibs="";
TString extraIncs="";
TString extraTasks="";
TString extraPkgs="";

//__________Config. Param. for grid macro.
Int_t ttl = 30000;
Int_t maxFilesPerJob = 100;
Int_t maxMergeFiles = 10;
Int_t maxMergeStages = 2;
//__________

//__________

//______________________________________________________________________________
AliAnalysisTask* runMuMuGrid(TString smode = "test",
                         TString inputFileName = "AliAOD.Muons.root",
                         Bool_t simulations=kFALSE)
{
  // --- general analysis setup ---
  TString rootVersion = "";
  TString alirootVersion = "";
  TString aliphysicsVersion = "vAN-20160112-1";
  TString extraLibs="";
  TString extraIncs="include";
  TString extraTasks="";
  TString extraPkgs="";

  TList pathList; pathList.SetOwner();
  TList fileList; fileList.SetOwner();
  
  // --- grid specific setup ---
  TString dataDir = "/alice/data/2015/LHC15o";
  TString dataPattern = "muon_calo_pass1/AOD/*/AliAOD.Muons.root";
  TString runFormat = "%09d";
  TString outDir = "";
  TString analysisMacroName = "MuMu";
  Int_t ttl = 30000;
  Int_t maxFilesPerJob = 100;
  Int_t maxMergeFiles = 10;
  Int_t maxMergeStages = 2;
  gROOT->LoadMacro("$HOME/Documents/Analysis/Macro/Philippe/runTaskFacilities.C");
  
  // --- prepare the analysis environment ---
  Int_t mode = PrepareAnalysis(smode, inputFileName, extraLibs, extraIncs, extraTasks, extraPkgs, pathList, fileList, '\0');
  
  // --- run the analysis (saf3 is a special case as the analysis is launched on the server) ---
  if (mode == kSAF3Connect) {
    if (!RunAnalysisOnSAF3(fileList, aliphysicsVersion, inputFileName)) return;
    }
    
  } else {


    TString Triggers = "CINT7-B-NOPF-MUFAST,CMUL7-B-NOPF-MUFAST";
    TString inputs = "0MSL:17,0MSH:18,0MLL:19,0MUL:20,0V0M:3";


    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AliMultSelectionTask * task = AddTaskMultSelection(kFALSE); // user
    if(smode.Contains("local"))task -> SetAlternateOADBforEstimators ("LHC15o");
    

    gROOT->LoadMacro("$ALICE_PHYSICS/PWG/muon/AddTaskMuMuMinvBA.C");
    AddTaskMuMuMinvBA("MuMuBA",Triggers.Data(),inputs.Data(),"PbPb2015",simulations);
    cout <<"add task mumu done"<< endl;
    
    if (smode == "saf3" && splitDataset) AliAnalysisManager::GetAnalysisManager()->SetSkipTerminate(kTRUE);
    
    RunAnalysis(smode, inputFileName, rootVersion, alirootVersion, aliphysicsVersion, extraLibs, extraIncs, extraTasks, extraPkgs, dataDir, dataPattern, outDir, analysisMacroName, runFormat, ttl, maxFilesPerJob, maxMergeFiles, maxMergeStages);
    
  }
  
}




