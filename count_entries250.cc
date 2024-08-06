#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

void count_entries250() {
    const Int_t nindex = 250;
    const Float_t threshold = 40;

    TString mergefilename = "../merge/All_1700735651_1701478713_conv.root";
    TFile *mergefile = new TFile(mergefilename);
    TTree *mergetree = (TTree*)mergefile->Get("tree");

    if (!mergetree) {
        std::cerr << "Cannot find TTree in the file." << std::endl;
        return;
    }

    Int_t adc[nindex];
    Int_t leadtime[nindex];
    Int_t trailtime[nindex];
    mergetree->SetBranchAddress("ADC", adc);
    mergetree->SetBranchAddress("LEADTIME", leadtime);
    mergetree->SetBranchAddress("TRAILTIME", trailtime);

    TH1F *h = new TH1F("h", "Channel vs. Count;Channel;Count", nindex, 0, nindex);

    Int_t nentries = mergetree->GetEntries();
    std::cout << "Number of entries in the tree: " << nentries << std::endl;

    for(Int_t ientry=0; ientry<nentries; ientry++){
        mergetree->GetEntry(ientry);

        for(Int_t index=0; index<nindex; index++){
            if(leadtime[index]-trailtime[index] > threshold) {
                h->Fill(index);
            }
        }
    }

    // 平均エントリ数の計算
    Double_t totalEntries = 0;
    for (int i = 0; i < nindex; ++i) {
        totalEntries += h->GetBinContent(i+1);
    }
    Double_t averageEntries = totalEntries / nindex;

    std::cout<<"average entries"<<":"<<averageEntries<<std::endl;

    // 平均の半分を下回るチャンネルのみ出力
    for (int i = 0; i < nindex; ++i) {
        if (h->GetBinContent(i+1) < averageEntries / 2) {
            std::cout << "Channel " << i << ": " << h->GetBinContent(i+1) << std::endl;
        }
    }

    h->Draw();
    mergefile->Close();
}
