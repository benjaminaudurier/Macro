#include "Riostream.h"
#include "TUrl.h"
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TGrid.h"

///Exemple of macro to run AddTaskMuMu on saf3 run by run 
/// author : Benjamin Audurier




Bool_t isMC = kFALSE;


//______________________________________________________________________________
AliAnalysisTask* runMuMuLoop(TString smode = "local", TString inputFileName = "Find;BasePath=/alice/data/2015/LHC15o/000244918/muon_calo_pass1/AOD/*;FileName=AliAOD.Muons.root;Mode=cache;" )
{
    // path for macro usefull for saf3
    gROOT->LoadMacro(gSystem->ExpandPathName("$HOME/Documents/Analysis/Macro/Philippe/runTaskFacilities.C"));

    // --- analysis setup ---
    TString rootVersion = "";
    TString alirootVersion = "";
    TString aliphysicsVersion = "vAN-20151215-1";

    TString dataDir = "/alice/cern.ch/user/p/ppillot/Data/LHC15n/reco/PbPbParam_AlignV6";
    
    TString dataPattern = "*AliESDs.root";
    TString runFormat = "%09d";
    TString outDir = "Data/LHC15n/reco/PbPbParam_AlignV6/Phys/all";
    TString analysisMacroName = "Physics";
    Int_t ttl = 30000;
    Int_t maxFilesPerJob = 100;
    Int_t maxMergeFiles = 10;
    Int_t maxMergeStages = 2; 

    TString extraLibs="PWGmuon";
    TString extraIncs="include";
    TString extraTasks="";
    TString extraPkgs=""; 

    // Copy file to run on the saf
    TList pathList=0x0; 
    pathList.SetOwner();
    // pathList.Add(new TObjString("$HOME/alicesw/aliphysics/mumu/src/PWG/muon"));
    TList fileList; fileList.SetOwner();

     // --- prepare the analysis environment ---
    Int_t mode = PrepareAnalysis(smode, inputFileName, extraLibs, extraIncs, extraTasks, extraPkgs, pathList, fileList);

    fileList.Add(new TObjString("runMuMuLoop.C"));
    fileList.Add(new TObjString("AddTaskMuMu.C"));

    // --- run the analysis (saf3 is a special case as the analysis is launched on the server) ---
     // --- run the analysis (saf3 is a special case as the analysis is launched on the server) ---
    if (mode == kSAF3Connect) {
      
      RunAnalysisOnSAF3(fileList, aliphysicsVersion, inputFileName);
      
    } else {
      
      CreateAnalysisTrain();
      
      RunAnalysis(smode, inputFileName, rootVersion, alirootVersion, aliphysicsVersion, extraLibs, extraIncs, extraTasks, extraPkgs, dataDir, dataPattern, outDir, analysisMacroName, runFormat, ttl, maxFilesPerJob, maxMergeFiles, maxMergeStages);
      
    }
}


//______________________________________________________________________________
void CreateAnalysisTrain()
{
  /// create the analysis train and configure it
  
  // analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("MuonPhysicsAnalysis");
  
  // Debug mode
  //mgr->SetDebugLevel(3);
  
  // AOD handler
  AliInputEventHandler* aodH = new AliAODInputHandler;
  mgr->SetInputEventHandler(aodH);
 
  // Fill the trigger list with desired trigger combinations (See on ALICE log book for denomination)
  //==============================================================================
    
  TList* triggers = new TList; // Create pointer for trigger list
  triggers->SetOwner(kTRUE); // Give rights to trigger liser
  if (!isMC)
  {
      // PbPb trigger
      triggers->Add(new TObjString("CINT7-B-NOPF-MUFAST"));//MB &0MUL 
      triggers->Add(new TObjString("CINT7-B-NOPF-MUFAST&0MSL"));//MB &0MUL 
      triggers->Add(new TObjString("CINT7-B-NOPF-MUFAST&0MUL"));//MB &0MUL
      triggers->Add(new TObjString("CMUL7-B-NOPF-MUFAST"));// MUL
      triggers->Add(new TObjString("CMSL7-B-NOPF-MUFAST"));// MSL
      triggers->Add(new TObjString("CMSL7-B-NOPF-MUFAST&0MUL"));// MSL &0MUL
  }
    
    // Load centrality task
    //==============================================================================
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AddTaskMultSelection(kFALSE); // user
  
    // Load task
    //==============================================================================
    TString outputname = AliAnalysisManager::GetAnalysisManager()->GetCommonFileName(); // Create output name in case of no dataset selected
    gROOT->LoadMacro("AddTaskMuMu.C");
    AddTaskMuMu(outputname.Data(),triggers,"PbPb2015",isMC);
    cout <<"add task mumu done"<< endl;
}

