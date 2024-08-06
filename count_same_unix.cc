#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TLegend.h>

void count_same_unix() {
    const Int_t nindex = 250;
    const Float_t threshold = 40;
    const Float_t PE_threshold = 2.5;
    const Int_t nHistograms = 8; // 8個のヒストグラム
    const Int_t ch_each = 31;

    //TString mergefilename = "../merge/All_1700735651_1701478713_conv.root";
    TString mergefilename = "../merge/ninja_rawdata_2021_4_15.root";
    TFile *mergefile = new TFile(mergefilename);
    TTree *mergetree = (TTree*)mergefile->Get("tree");

    if (!mergetree) {
        std::cerr << "Cannot find TTree in the file." << std::endl;
        return;
    }

    Int_t adc[nindex];
    UInt_t unixtime[nindex]; //守山さんのデータの時
    //Int_t unixtime[nindex]; //自分のデータの時
    Int_t leadtime[nindex];
    Int_t trailtime[nindex];
    Float_t pe[nindex];
    mergetree->SetBranchAddress("ADC", adc);
    mergetree->SetBranchAddress("UNIXTIME", unixtime);
    mergetree->SetBranchAddress("LEADTIME", leadtime);
    mergetree->SetBranchAddress("TRAILTIME", trailtime);
    mergetree->SetBranchAddress("PE", pe);

    Int_t nentries = mergetree->GetEntries();
    Int_t befunix=0;
    Int_t count=0;

    for(Int_t ientry = 0; ientry < nentries; ientry++) {
        mergetree->GetEntry(ientry);
        //if(unixtime[0]>0){
        //    std::cout<<unixtime[0]<<std::endl;
        //}
        if(unixtime[245]==befunix){
            count++;
            std::cout<<"Index = "<<ientry<<" and "<<ientry-1<<std::endl;
        }
        befunix=unixtime[0];
        //for(Int_t index = 0; index < nindex; index++) {
        //    
        //}
    }
    std::cout<<"Counter = "<<count<<::std::endl;
}
