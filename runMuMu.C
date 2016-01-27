///
/// Example macro to run the AliAnalysisTaskMuMu task
///
/// \author Benjamin Audurier
///


// Sim :
// Find;BasePath=/alice/cern.ch/user/l/laphecet/Analysis/LHC13d/simjpsi/CynthiaTuneWithRejectList/195760/;FileName=AliAOD.Muons.root
// 
// Data:
// Find;BasePath=/alice/data/2013/LHC13d/000195760/ESDs/muon_pass2/AOD134;FileName=AliAOD.root

Bool_t runOnTFileCollection = kFALSE;

//______________________________________________________________________________
AliAnalysisTask* runMuMu(TString runMode, 
                        TString analysisMode,
                        TString inputName       = "Find;BasePath=/alice/data/2015/LHC15o/000244918/muon_calo_pass1/AOD/*;FileName=AliAOD.Muons.root;Tree=/aodTree;Filter=AODMUONONLY_PBPB2015;Mode=cache; ",
                        TString inputOptions    = "",
                        TString analysisOptions = "",//split
                        TString softVersions    = "",
                        TString taskOptions     = "" )
{
    //Copy parfile and macro
    // gSystem->Exec("cp $MACRODIR/newVaf/AliceVaf.par AliceVaf.par"); //Copy baseline    
    gROOT->LoadMacro(gSystem->ExpandPathName("$TASKDIR/runTaskUtilities.C"));     
     
    // Macro to connect to proof. First argument useless for saf3
    SetupAnalysis(runMode,analysisMode,inputName,inputOptions,softVersions,analysisOptions, "libPWGmuon.so",". $ALICE_ROOT/include $ALICE_PHYSICS/include");
    
    //Flag for MC
    Bool_t isMC = IsMC(inputOptions);

    TString Triggers = "CINT7-B-NOPF-MUFAST,CINT7-B-NOPF-MUFAST&0MSL,CINT7-B-NOPF-MUFAST&0MUL,CMUL7-B-NOPF-MUFAST,CMSL7-B-NOPF-MUFAST,CMSL7-B-NOPF-MUFAST&0MUL";
    TString inputs = "0MSL:17,0MSH:18,0MLL:19,0MUL:20,0V0M:3";

    // // Load baseline
    // //==============================================================================
    // gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddTaskBaseLine.C");
    // AddTaskBaseLine();
    
    // Load centrality task
    // //==============================================================================
    // gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
    // AliMultSelectionTask * task = AddTaskMultSelection(kFALSE); // user
    // task -> SetAlternateOADBforEstimators ("LHC15o");
    
    // gROOT->LoadMacro("/Users/audurier/alicesw/aliphysics/mumu/src/PWG/muon/AddTaskMuMuTrain.C");
    gROOT->LoadMacro("$ALICE_PHYSICS/PWG/muon/AddTaskMuMuMinvBA.C");
    AddTaskMuMuMinvBA("MuMuBA",Triggers.Data(),inputs.Data(),"PbPb2015",isMC);
    cout <<"add task mumu done"<< endl;

    // Start analysis
    //==============================================================================
    StartAnalysis(runMode,analysisMode,inputName,inputOptions);     
   
    // delete triggers;
}

