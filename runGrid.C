/*
 *  runGrid.c
 *  aliroot
 *
 *  Created by Benjamin Audurier 11-26-2015.
 *  Copyright 2010 SUBATECH. All rights reserved.
 *
 */

TString rootVersion = "v5-34-30";
TString alirootVersion = "v5-07-01-3";
TString aliphysicsVersion = "vAN-20151015-1";
TString dataDir = "/alice/data/2015/LHC15n";
TString dataPattern = "muon_calo_pass1/*AliESDs.root";
TString runFormat = "%09d";
TString outDir = "Analysis/LHC15n/TrackingEfficiency/Data";
Int_t ttl = 30000;
Int_t maxFilesPerJob = 150;
Int_t maxMergeFiles = 10;
Int_t maxMergeStages = 4;

// --- prepare environment ---
TString extraLibs="";
TString extraIncs="include";
TString extraTasks="AliAnalysisTaskMuonTrackingEffLocal";

//TString alignStorage = "alien://folder=/alice/simulation/2008/v4-15-Release/Residual";
TString alignStorage = "alien://folder=/alice/data/2015/OCDB";

//______________________________________________________________________________
void runGrid(TString smode = "full", TString inputFileName = "../run-list.txt",
             Bool_t applyPhysSel = kFALSE, Bool_t mc = kFALSE, Bool_t embedding = kFALSE)
{
    /// Study the MUON performances
    
    gROOT->LoadMacro("/Users/audurier/Documents/Analysis/Macro_Utile/runTaskFacilities.C");
    
    // --- Check runing mode ---
    Int_t mode = GetMode(smode, inputFileName);
    if(mode < 0) {
        Error("runMuonEfficiency","Please provide either an ESD root file a collection of ESDs or a dataset.");
        return;
    }
    
    // // --- copy files needed for this analysis ---
    // TList pathList; pathList.SetOwner();
    // pathList.Add(new TObjString("/Users/audurier/Documents/Analysis/Tasks"));
    
    // TList fileList; fileList.SetOwner();
    // fileList.Add(new TObjString("AliAnalysisTaskMuonTrackingEffLocal.cxx"));
    // fileList.Add(new TObjString("AliAnalysisTaskMuonTrackingEffLocal.h"));
    
    // CopyFileLocally(pathList, fileList);
    
    LoadAlirootLocally(extraLibs, extraIncs, extraTasks);
    
    AliAnalysisGrid *alienHandler = 0x0;
    
    if (mode == kProof || mode == kProofLite) return;
    else if (mode == kGrid || mode == kTerminate) {
        TString analysisMacroName = "Eff";
        alienHandler = static_cast<AliAnalysisGrid*>(CreateAlienHandler(smode, rootVersion, alirootVersion, aliphysicsVersion, inputFileName, dataDir, dataPattern, outDir, extraLibs, extraIncs, extraTasks, analysisMacroName, runFormat, ttl, maxFilesPerJob, maxMergeFiles, maxMergeStages));
        if (!alienHandler) return;
    }
    
    // --- Create the analysis train ---
    CreateAnalysisTrain(applyPhysSel, mc, embedding, alienHandler);
    
    // --- Create input object ---
    TObject* inputObj = CreateInputObject(mode, inputFileName);
    
    // --- start analysis ---
    StartAnalysis(mode, inputObj);
    
}

//______________________________________________________________________________
void CreateAnalysisTrain(Bool_t applyPhysSel, Bool_t mc, Bool_t embedding, TObject* alienHandler)
{
    /// create the analysis train and configure it
    
    LoadLocalLibs(kTRUE);
    
    // analysis manager
    AliAnalysisManager *mgr = new AliAnalysisManager("MuonEfficiencyAnalysis");
    
    // Connect plugin to the analysis manager if any
    if (alienHandler) mgr->SetGridHandler(static_cast<AliAnalysisGrid*>(alienHandler));
    
    // ESD input
    AliESDInputHandler* esdH = new AliESDInputHandler();
    esdH->SetReadFriends(kFALSE);
    esdH->SetInactiveBranches("*");
    esdH->SetActiveBranches("MuonTracks MuonClusters MuonPads AliESDRun. AliESDHeader. AliMultiplicity. AliESDFMD. AliESDVZERO. SPDVertex. PrimaryVertex. AliESDZDC. AliESDTZERO.");
    mgr->SetInputEventHandler(esdH);
    
    // event selection in case of physicselection
    UInt_t offlineTriggerMask;
    if (applyPhysSel) {
        gROOT->LoadMacro("/Users/audurier/alicesw/aliphysics/mumu/inst/OADB/macros/AddTaskPhysicsSelection.C");
        AliPhysicsSelectionTask* physicsSelection = AddTaskPhysicsSelection(mc && !embedding);
        if(!physicsSelection) {
            Error("CreateAnalysisTrain","AliPhysicsSelectionTask not created!");
            return;
        }
        //offlineTriggerMask = AliVEvent::kAny;
        offlineTriggerMask = AliVEvent::kMUS7;
        // offlineTriggerMask = AliVEvent::kMUU7 | AliVEvent::kMuonUnlikeLowPt8;
    }
    /*
     // centrality selection
     gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskCentrality.C");
     AliCentralitySelectionTask *taskCentrality = AddTaskCentrality();
     if(!taskCentrality) {
     Error("CreateAnalysisTrain","AliCentralitySelectionTask not created!");
     return;
     }
     if (applyPhysSel) taskCentrality->SelectCollisionCandidates(offlineTriggerMask);
     if (mc && !embedding) taskCentrality->SetMCInput();
     */
    // track selection
    AliMuonTrackCuts trackCuts("stdCuts", "stdCuts");
    trackCuts.SetAllowDefaultParams();
    //  trackCuts.SetFilterMask(0);
    //  trackCuts.SetCustomParamFromRun(169099, "pass2_muon");
    //  trackCuts.CustomParam()->SetChi2NormCut(3.5);
    trackCuts.SetFilterMask(AliMuonTrackCuts::kMuMatchLpt | AliMuonTrackCuts::kMuEta |
                            AliMuonTrackCuts::kMuThetaAbs | AliMuonTrackCuts::kMuPdca);
    trackCuts.SetIsMC(mc && !embedding);
    
    // Muon efficiency analysis
    gROOT->LoadMacro("AddTaskMUONTrackingEfficiency.C");
    AliAnalysisTaskMuonTrackingEffLocal* muonEfficiency = AddTaskMUONTrackingEfficiency(trackCuts,"");
    if(!muonEfficiency) {
        Error("CreateAnalysisTrain","AliAnalysisTaskMuonTrackingEffLocal not created!");
        return;
    }
    if (applyPhysSel) muonEfficiency->SelectCollisionCandidates(offlineTriggerMask);
    if (!alignStorage.IsNull()) muonEfficiency->SetAlignStorage(alignStorage.Data());
    //muonEfficiency->SetRecoParamStorage("alien://folder=/alice/cern.ch/user/p/ppillot/OCDB2012_newReco");
    muonEfficiency->SetMuonPtCut(1.);
    muonEfficiency->UseMCLabel(kFALSE);
    muonEfficiency->EnableDisplay(kTRUE);
    
    
}

