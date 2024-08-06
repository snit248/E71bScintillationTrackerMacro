#include <iostream>
#include <fstream>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TLegend.h>
#include <TDatime.h>

void count_entries_xyAuto(TString runtimename) {
    const Int_t nindex = 250;
    const Float_t ToT_threshold = 40;
    const Float_t PE_threshold = 3.5;
    const Int_t nHistograms = 8; // 8個のヒストグラム
    const Int_t ch_each = 31;

    TString mergefilename = "../merge/All_"+runtimename+"_conv.root";
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
    Float_t pe[nindex];
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
    TH1F *histograms[nHistograms];
    for (int i = 0; i < nHistograms; ++i) {
        histograms[i] = new TH1F(Form("h%d", i), Form("Histogram Channel;Count"), ch_each, 0, ch_each);
        histograms[i]->SetMinimum(0); // Y軸の最小値を設定
        histograms[i]->SetMaximum(3000); // Y軸の最大値を設定
    }

    Int_t nentries = mergetree->GetEntries();
    for(Int_t ientry = 0; ientry < nentries; ientry++) {
        mergetree->GetEntry(ientry);
        //時間分け
        //1701939967 突然ch122が壊れた日
        //if(unixtime[0]>1702703000){
            for(Int_t index = 0; index < nindex; index++) {
                //Y16uchipのみthresholdを下げる
                if((index >= 16 && index <=23) || (index >= 47 && index <=54) || (index >= 78 && index <=85) || (index >= 109 && index <=117) || index==249){
                    //if(leadtime[index]-trailtime[index] > ToT_threshold) {
                    if(pe[index] > PE_threshold) {
                        if(module[index] == 0 || module[index] == 1) { // Yの振り分け
                            histograms[pln[index]]->Fill(ch[index]);
                        } else if(module[index] == 2 || module[index] == 3) { // Xの振り分け
                            histograms[pln[index] + 4]->Fill(ch[index]);
                        }
                    }
                }else{
                    //if(leadtime[index]-trailtime[index] > ToT_threshold) {
                    if(pe[index] > PE_threshold) {
                        if(module[index] == 0 || module[index] == 1) { // Yの振り分け
                            histograms[pln[index]]->Fill(ch[index]);
                        } else if(module[index] == 2 || module[index] == 3) { // Xの振り分け
                            histograms[pln[index] + 4]->Fill(ch[index]);
                        }
                    }
                }
            }
        //}
    }

    TCanvas *c = new TCanvas("c", "Hit Count", 800, 600);
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->SetHeader("Histograms", "C");

    TDatime date;
    TString dateStr;
    dateStr.Form("%d%02d%02d%02d%02d", date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute());

    // Yのヒストグラムを描画して保存
    TCanvas *cY = new TCanvas("cY", "Hit Count Y", 800, 600);
    for (int i = 0; i < nHistograms / 2; ++i) { // nHistograms / 2 = 4 (Yのヒストグラム数)
        histograms[i]->SetLineColor(i + 1);
        histograms[i]->Draw(i == 0 ? "" : "same");
    }
    cY->Update();
    TString yHistName = "../../graphs/Y_" + dateStr + "_hist.png";
    cY->SaveAs(yHistName);

    // Xのヒストグラムを描画して保存
    TCanvas *cX = new TCanvas("cX", "Hit Count X", 800, 600);
    for (int i = nHistograms / 2; i < nHistograms; ++i) { // nHistograms / 2 = 4 (Xのヒストグラム数)
        histograms[i]->SetLineColor(i + 1);
        histograms[i]->Draw(i == nHistograms / 2 ? "" : "same");
    }
    cX->Update();
    TString xHistName = "../../graphs/X_" + dateStr + "_hist.png";
    cX->SaveAs(xHistName);

    legend->Draw();
    c->Update();
}
