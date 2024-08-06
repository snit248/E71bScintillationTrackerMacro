#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TLegend.h>

void Search_OC() {
    const Int_t nindex = 64;
    const Float_t adc_threshold = 820;
    const Float_t ToT_threshold = 40;
    const Float_t PE_threshold = 3.5;
    const Int_t nHistograms = 8; // 8個のヒストグラム
    const Int_t ch_each = 31;

    TString mergefilename = "../data/Y2_0037_1708219820.root";
    //All_1700735651_1701478713_conv.root
    //1702446582
    //1702533973 E71b 9
    //1702797691 E71b 10
    //All_1700735651_1702797714_conv.root
    //1703173071
    //All_1705039280_conv.root
    //All_1705045718_conv.root
    //All_1705561018_conv.root
    //TString runtimename = "1705563849"; //E71b testLowEntries16FlatCableReverse 1/18
    //All_1705566581_conv
    //1706672531 test 1/31
    TFile *mergefile = new TFile(mergefilename);
    TTree *mergetree = (TTree*)mergefile->Get("tree");

    if (!mergetree) {
        std::cerr << "Cannot find TTree in the file." << std::endl;
        return;
    }

    Int_t adc[nindex];
    Int_t unixtime[nindex];
    Int_t leadtime[nindex];
    Int_t trailtime[nindex];
    Double_t pe[nindex];
    mergetree->SetBranchAddress("ADC", adc);
    mergetree->SetBranchAddress("UNIXTIME", unixtime);
    mergetree->SetBranchAddress("LEADTIME", leadtime);
    mergetree->SetBranchAddress("TRAILTIME", trailtime);
    mergetree->SetBranchAddress("PE", pe);

    Char_t buffer[256];
    TString mapname = "../src/channel_new.dat";
    std::ifstream map(mapname);
    if (!map.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }

    map.getline(buffer, 100);
    Int_t tmp, module[nindex], ch_mod[nindex];
    Int_t view[nindex], pln[nindex], ch[nindex];

    for(Int_t index = 0; index < nindex; index++) {
        map >> tmp >> module[index] >> ch_mod[index] >> view[index] >> pln[index] >> ch[index];
    }

    // ヒストグラムの作成（0-3がY、4-7がX）
    //TH1F *histograms[nHistograms];
    //for (int i = 0; i < nHistograms; ++i) {
    //    histograms[i] = new TH1F(Form("h%d", i), Form("Histogram Channel;Count"), ch_each, 0, ch_each);
    //    histograms[i]->SetMinimum(0); // Y軸の最小値を設定
    //    histograms[i]->SetMaximum(6000); // Y軸の最大値を設定
    //}

    // TH1F* hist[nindex]; のように変更
    TH1F* hist[nindex]; // 各チャンネルのヒストグラム用のポインタ配列

    for(Int_t i = 0; i < nindex; i++) {
        hist[i] = new TH1F(Form("hist%d", i), Form("Histogram for Channel %d;ADC Counts;Entries", i), 100, 0, 1000);
    }

    // エントリのループとヒストグラムへの入力...
    Int_t nentries = mergetree->GetEntries();
    for(Int_t ientry = 0; ientry < nentries; ientry++) {
        mergetree->GetEntry(ientry);
        for(Int_t index = 0; index < nindex; index++) {
            if(adc[index] > adc_threshold) {
                hist[index]->Fill(adc[index]); // 修正: hist[index]->Fill(...) を使用
            }
        }
    }

    // ヒストグラムの描画...
    TCanvas *c = new TCanvas("c", "Histograms", 800, 600);
    c->Divide(8, 8); // 仮に8x8のグリッドに分ける
    for(Int_t i = 0; i < nindex; i++) {
        c->cd(i+1);
        hist[i]->Draw();
    }

    // キャンバスの更新など...
    c->Update();
}