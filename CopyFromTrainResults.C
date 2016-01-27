///
/// Macro to make a new TFile from train results usable for the MuMu Framework
/// 
/// author : Benjamin Audurier

#include "TString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TDirectoryFile.h"
#include <string>
#include "AliMergeableCollection.h"
#include "AliCounterCollection.h"
#include "AliAnalysisMuMuBinning.h"
#include "TKey.h"
#include "AliLog.h"



void CopyFromTrainResults(TString file,TString directory = "MuMuLowPt")
{
	AliMergeableCollection*oc =0x0;
	AliCounterCollection  *cc =0x0;
	AliAnalysisMuMuBinning*bin = 0x0;
	TDirectoryFile        *d =0x0;

	//Open file
	TFile*  f =TFile::Open(file.Data());
	if(!f) return;
	d = static_cast<TDirectoryFile*>(f->FindObjectAny(directory.Data()));
	if (d)printf("directory found !\n");
    else return;
	
	// oc points on the histogramme collection
    oc = static_cast<AliMergeableCollection*>(f->FindObjectAny("OC")->Clone());
    if (!oc) oc = static_cast<AliMergeableCollection*>(f->FindObjectAny("MC")->Clone());
    if (oc)printf("oc found !\n");
    else return;
        // cc point on mergeable collection
    cc = static_cast<AliCounterCollection*>(f->FindObjectAny("CC")->Clone());
    if (cc)printf("cc found !\n");
    else return;
    
    // Iterator on bin type
    TIter next(d->GetListOfKeys());
    TKey* key;

    // bin points on each bin type    
    while ( ( key = static_cast<TKey*>(next())) && !bin ) {
    	if ( strcmp(key->GetClassName(),"AliAnalysisMuMuBinning")==0 ) { 
    		bin = dynamic_cast<AliAnalysisMuMuBinning*>(key->ReadObj()->Clone());
    	}
    }
    if (bin)printf("bin found !\n");
    else return;

    f->Close();


    TFile* f1 = new TFile("AnalysisResultsNew.root","recreate");
    oc->Write("OC");
    cc->Write();
   	bin->Write();
    f1->Close();

}