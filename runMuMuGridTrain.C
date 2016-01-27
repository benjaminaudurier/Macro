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
AliAnalysisTask* runMuMuGridTrain(const char* dataset="runList-AOD.txt",
                         TString where="saf",
                         Bool_t simulations=kFALSE,
                         Bool_t baseline=kFALSE)
{
  gROOT->LoadMacro("/Users/audurier/Documents/Analysis/Macro/Philippe/runTaskFacilities.C");// where are macros to load on grid, caf etc.
  TString sds(dataset); // To work with dataset

  // --- Check runing mode ---
  
  if(sds.Length()>0) {
    Int_t mode = GetMode(where, sds);
  
    if(mode < 0) {
    Error("runGenTuner","Please provide either an AOD root file a collection of AODs or a dataset.");
    return;
    }
  }
  
  // --- prepare environment ---
  LoadAlirootLocally(extraLibs, extraIncs, extraTasks,extraPkgs);
  // Choose the CF
  AliAnalysisGrid *alienHandler = 0x0;
  if(sds.Length()>0 ){
    if (mode == kProof || mode == kProofLite) return;
    else if (mode == kGrid || mode == kTerminate) {
        TString analysisMacroName = "MuMu";
        alienHandler = static_cast<AliAnalysisGrid*>(CreateAlienHandler(where,  alirootVersion,  aliphysicsVersion,  dataset, dataDir, dataPattern, outDir,  extraLibs,
           extraIncs,  extraTasks,  extraPkgs,  analysisMacroName, runFormat, ttl, maxFilesPerJob, maxMergeFiles, maxMergeStages ));
        if (!alienHandler) return;
    }
  }
  // analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("MuMu");
  // Connect plugin to the analysis manager if any
  if (alienHandler) mgr->SetGridHandler(static_cast<AliAnalysisGrid*>(alienHandler));

  // Checkout input and select the correct input handler
  //==============================================================================
  if ( dataPattern.Contains("AOD") ){
    input = new AliAODInputHandler;
  }
  else if ( dataPattern.Contains("ESD") ){
    input = new AliESDInputHandler;
  }
  else{
    std::cout << "Cannot get input type !" << std::endl;
    return 0;
  }

  mgr->SetInputEventHandler(input); // Set the input handler
    
  //For the train version
//  char * triggerlist = "CINT7-B-NOPF-MUFAST,CINT7-B-NOPF-MUFAST&0MSL,CINT7-B-NOPF-MUFAST&0MUL,CMUL7-B-NOPF-MUFAST,CMSL7-B-NOPF-MUFAST,CMSL7-B-NOPF-MUFAST&0MUL";
    char * triggerlist = "CINT7-B-NOPF-MUFAST";
    char * inputs = "0MSL:17,0MSH:18,0MLL:19,0MUL:20,0V0M:3";

  TString outputname(AliAnalysisManager::GetCommonFileName()); // Create output name in case of no dataset selected
  
  // Load tasks
  //==============================================================================
  if (!baseline){
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    AliMultSelectionTask *multi  =AddTaskMultSelection(kFALSE);
      if(mode==kLocal){
          multi->SetAlternateOADBFullManualBypass("/Users/audurier/alicesw/aliphysics/mumu/inst/OADB/COMMON/MULTIPLICITY/data/OADB-LHC15o.root");
          multi->SetAlternateOADBforEstimators("LHC15o");
      }
    
    gROOT->LoadMacro("$ALICE_PHYSICS/PWG/muon/AddTaskMuMuMinvBA.C");
    AliAnalysisTask* mumu = AddTaskMuMuMinvBA("MuMu",triggerlist,inputs,"PbPb2015",simulations);
  }
  else {
      gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddTaskBaseLine.C");
      AddTaskBaseLine();
  }
  if (!mgr->InitAnalysis()){
      cout << "Could not InitAnalysis" << endl;
    return 0;
  }

  // Start analysis
  //==============================================================================
  if ( sds.Length()>0 ){
    TStopwatch timer;
    // --- Create input object ---
    TObject* inputObj = CreateInputObject(mode, sds);
    // --- start analysis ---
    if(mode==kLocal) mgr->SetNSysInfo(10);
    
    StartAnalysis(mode,inputObj);
    timer.Print();
    if(mode==kLocal) mgr->ProfileTask("AliAnalysisTaskMuMu");
  }
  else{
    mgr->PrintStatus();
    task->Print();
//  return task;
    TChain* c = CreateLocalChain("list.aod.txt");
//      mgr->SetNSysInfo(10);
    TStopwatch timer;
//    mgr->SetDebugLevel(10);
    mgr->StartAnalysis("local",c);
    timer.Print();
//    if (baseline) mgr->ProfileTask("baseline");
  }
  AliCodeTimer::Instance()->Print();// Timer
}

//______________________________________________________________________________
TChain* CreateLocalChain(const char* filelist)
{
    TChain* c = new TChain("aodTree");
    
    char line[1024];
    
    ifstream in(filelist);
    while ( in.getline(line,1024,'\n') )
        {
        c->Add(line);
        }
    return c;
}




