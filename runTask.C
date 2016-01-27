void runTask ( TString runMode, TString analysisMode,
               TString inputName,
               TString inputOptions = "",
               TString softVersions = "",
               TString analysisOptions = "",
               TString taskOptions = "" )
{

  gROOT->LoadMacro(gSystem->ExpandPathName("$TASKDIR/runTaskUtilities.C"));

  // This setpus the analysis: i.e. loads needed libs, creates the manager
  SetupAnalysis(runMode,analysisMode,inputName,inputOptions,softVersions,analysisOptions, "libPWGPPMUONlite.so","$ALICE_ROOT/include $ALICE_PHYSICS/include");

  Bool_t isMC = IsMC(inputOptions);

  // Add here your tasks
  gROOT->LoadMacro("$ALICE_PHYSICS/PWGPP/macros/AddTaskMTRchamberEfficiency.C");
  AliAnalysisTaskTrigChEff* task = AddTaskMTRchamberEfficiency(isMC);

  // This starts the analysis
  StartAnalysis(runMode,analysisMode,inputName,inputOptions);
}