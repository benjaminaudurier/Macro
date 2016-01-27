//_____________________________________________________________________________
void FnormMacro(
              const char* filename="../LHC15g.MuMu.1.root",
              const char* associatedSimFileName="",
              const char* associatedSimFileName2="",
              const char* beamYear="PbPb2011",const int DebugLevel =0)
{


  // //_____ FNorm
    // analysis.ComputeIntFnormFromCounters("",kFALSE);
    // //_____ 


  AliAnalysisMuMu ana(filename,associatedSimFileName,associatedSimFileName2,beamYear);
  AliLog::SetGlobalDebugLevel(DebugLevel);


  if (!ana.OC() || !ana.CC())
  {
    AliError("No mergeable/counter collection. Consider Upgrade()");
    return ;
  }
  else
  {
    cout <<      " ================================================================ " << endl;
    cout <<      "                  Compute Mean Fnorm From Counters                " << endl;
    cout <<      " ================================================================ " << endl;
  }


  // Get configuration settings
  TObjArray* eventTypeArray   = ana.Config()->GetListElements(AliAnalysisMuMuConfig::kEventSelectionList,IsSimulation());
  TObjArray* triggerMuonArray = ana.Config()->GetListElements(AliAnalysisMuMuConfig::kDimuonTriggerList,IsSimulation());
  TObjArray* triggerMBArray   = ana.Config()->GetListElements(AliAnalysisMuMuConfig::kMinbiasTriggerList,IsSimulation());
  TObjArray* centralityArray  = ana.Config()->GetListElements(AliAnalysisMuMuConfig::kCentralitySelectionList, IsSimulation());

  // Iterator for loops
  TIter nextTriggerMuon(triggerMuonArray);
  TIter nextTriggerMB(triggerMBArray);
  TIter nextEventType(eventTypeArray);
  TIter nextCentrality(centralityArray);

  // Strings
  TObjString* striggerMuon;
  TObjString* striggerMB;
  TObjString* seventType;
  TObjString* scentrality;

  //Pointers on histo
  TH1*h(0x0);
  TH1*h1(0x0);
  TH1*h2(0x0);
  
  Double_t FNormOverStat(0.);
  Double_t FNormTotError(0.);
  Double_t FNormTotErrorInverse(0.);
  Double_t FNormTotErrorSys(0.);
  Double_t Norm(1.);

  Int_t n =0; //counter

  nextEventType.Reset();
  // Loop on each envenType (see MuMuConfig)
  //==============================================================================
  while ( ( seventType = static_cast<TObjString*>(nextEventType())) )
  {
    AliDebug(1,Form("EVENTTYPE %s",seventType->String().Data()));
    nextTriggerMuon.Reset();
    // Loop on each Muon trigger (see MuMuConfig)
    //==============================================================================
    while ( ( striggerMuon = static_cast<TObjString*>(nextTriggerMuon())) )
    {
      AliDebug(1,Form("-MUON TRIGGER %s",striggerMuon->String().Data()));
      nextTriggerMB.Reset();
      // Loop on each MB trigger (not the ones in MuMuConfig but the ones set)
      //==============================================================================
      while ( ( striggerMB = static_cast<TObjString*>(nextTriggerMB())) )
      {
        AliDebug(1,Form("-- MB PAIRCUT %s",striggerMB->String().Data()));
        nextCentrality.Reset();
        // Loop on each centrality
        //==============================================================================
        while ( ( scentrality = static_cast<TObjString*>(nextCentrality()) ) )
        {
          TString id(Form("/FNORM-%s/%s/%s/PbPb",striggerMuon->String().Data(),seventType->String().Data(),scentrality->String().Data())); // Path where are saved histos in the mergeable collection

          h = OC()->Histo(id.Data(),Form("hFNormIntVSrun_%s",striggerMB->String().Data()));
          if (!h)
          {
            AliDebug(1,Form("Can't get histo %s/hFNormIntVSrun_%s",id.Data(),striggerMB->String().Data()));
            continue;
          }

          h1 = OC()->Histo(id.Data(),Form("hFNormInt_%s",striggerMB->String().Data()));
          if (!h1)
          {
            AliDebug(1,Form("Can't get histo %s/hFNormInt_%s",id.Data(),striggerMB->String().Data()));
            continue;
          }

          h2 = OC()->Histo(id.Data(),Form("hFNormIntSys_%s",striggerMB->String().Data()));
          if (!h2)
          {
            AliDebug(1,Form("Can't get histo %s/hFNormIntSys_%s",id.Data(),striggerMB->String().Data()));
            continue;
          }

          cout << Form("Fnorm from %s/%s added",id.Data(),h1->GetName()) << endl;
          cout << Form("Fnorm from %s/%s added",id.Data(),h2->GetName()) << endl;

          // Normalise with respect to centrality
          if (scentrality->String().Contains("V0M_00.00_90.00"))
          {
            Norm = 1.;

            FNormOverStat        = FNormOverStat + (Norm*h1->GetBinContent(1)) /(TMath::Power(Norm *h1->GetBinError(1),2.));
            FNormTotError        = FNormTotError + 1./(TMath::Power(Norm*h1->GetBinError(1),2.));
            FNormTotErrorInverse = FNormTotErrorInverse + 1./(TMath::Power(Norm*h1->GetBinError(1),-2.));
            FNormTotErrorSys     = FNormTotErrorSys + 1./(TMath::Power(Norm*h2->GetBinContent(1),2.));

            cout <<"--- Quantities from histogram             :                " << endl;
            cout <<" - Norm                                   = " << Norm << endl;
            cout <<" - FNormHisto                             = " << h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << h2->GetBinContent(1) << endl;
            cout <<"--- Quantities (normalized) from histogram:                " << endl;
            cout <<" - FNormHisto                             = " << Norm*h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << Norm*h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << Norm*h2->GetBinContent(1) << endl;
            cout <<"--- After addition                        :                 " << endl;
            cout <<" - FNormOverStat                          = " << FNormOverStat << endl;
            cout <<" - FNormTotError                          = " << FNormTotError << endl;
            cout <<" - FNormTotErrorInverse                   = " << FNormTotErrorInverse << endl;
            cout <<" - FNormTotErrorSys                       = " << FNormTotErrorSys << endl;
            }
          else if (scentrality->String().Contains("V0M_10.00_50.00"))
          {
            Norm = (1./0.4)*0.445*0.9;

            FNormOverStat        = FNormOverStat + (Norm*h1->GetBinContent(1)) /(TMath::Power(Norm *h1->GetBinError(1),2.));
            FNormTotError        = FNormTotError + 1./(TMath::Power(Norm*h1->GetBinError(1),2.));
            FNormTotErrorInverse = FNormTotErrorInverse + 1./(TMath::Power(Norm*h1->GetBinError(1),-2.));
            FNormTotErrorSys     = FNormTotErrorSys + 1./(TMath::Power(Norm*h2->GetBinContent(1),2.));

            cout <<"--- Quantities from histogram             :                " << endl;
            cout <<" - Norm                                   = " << Norm << endl;
            cout <<" - FNormHisto                             = " << h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << h2->GetBinContent(1) << endl;
            cout <<"--- Quantities (normalized) from histogram:                " << endl;
            cout <<" - FNormHisto                             = " << Norm*h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << Norm*h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << Norm*h2->GetBinContent(1) << endl;
            cout <<"--- After addition                        :                 " << endl;
            cout <<" - FNormOverStat                          = " << FNormOverStat << endl;
            cout <<" - FNormTotError                          = " << FNormTotError << endl;
            cout <<" - FNormTotErrorInverse                   = " << FNormTotErrorInverse << endl;
            cout <<" - FNormTotErrorSys                       = " << FNormTotErrorSys << endl;

          }
          else if (scentrality->String().Contains("V0M_00.00_07.50"))
          {
            Norm = (1./0.075)*0.443*0.9;

            FNormOverStat        = FNormOverStat + (Norm*h1->GetBinContent(1)) /(TMath::Power(Norm *h1->GetBinError(1),2.));
            FNormTotError        = FNormTotError + 1./(TMath::Power(Norm*h1->GetBinError(1),2.));
            FNormTotErrorInverse = FNormTotErrorInverse + 1./(TMath::Power(Norm*h1->GetBinError(1),-2.));
            FNormTotErrorSys     = FNormTotErrorSys + 1./(TMath::Power(Norm*h2->GetBinContent(1),2.));

            cout <<"--- Quantities from histogram             :                " << endl;
            cout <<" - Norm                                   = " << Norm << endl;
            cout <<" - FNormHisto                             = " << h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << h2->GetBinContent(1) << endl;
            cout <<"--- Quantities (normalized) from histogram:                " << endl;
            cout <<" - FNormHisto                             = " << Norm*h1->GetBinContent(1) << endl;
            cout <<" - FNormHistoError                        = " << Norm*h1->GetBinError(1) << endl;
            cout <<" - FNormHistoSys                          = " << Norm*h2->GetBinContent(1) << endl;
            cout <<"--- After addition                        :                 " << endl;
            cout <<" - FNormOverStat                          = " << FNormOverStat << endl;
            cout <<" - FNormTotError                          = " << FNormTotError << endl;
            cout <<" - FNormTotErrorInverse                   = " << FNormTotErrorInverse << endl;
            cout <<" - FNormTotErrorSys                       = " << FNormTotErrorSys << endl;

          }
          else
          {
            AliError("Check this method for centrality selection !");
            return;
          }

          n++;
        }
      }
    }
  }

cout << "Mean FNorm  computed from " << n <<" results = " << FNormOverStat/FNormTotError << " +/- " <<
    TMath::Sqrt(FNormTotErrorInverse) << " (stat) +/-" << TMath::Sqrt(FNormTotErrorSys) << " (sys) " <<endl;

delete triggerMuonArray ;
delete triggerMBArray ;
delete eventTypeArray ;
delete centralityArray ;
}
