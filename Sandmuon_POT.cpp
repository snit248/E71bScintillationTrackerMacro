#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <vector>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <fstream>

void Sandmuon_POT() {
  /*
    // ROOTファイルを開く
    TFile *file = TFile::Open("../../POT/E71b_ALL_POT/E71bALLPOT.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Could not open the file." << std::endl;
        return;
    }

    // TTreeを読み込む
    TTree *tree = nullptr;
    file->GetObject("bsd", tree);
    if (!tree) {
        std::cerr << "Could not read the TTree." << std::endl;
        file->Close();
        return;
    }
    */
/*
    TFile file("../../POT/E71b_ALL_POT/E71bALLPOT.root"); // ファイルを読み込む
    TTree *tree = (TTree*)file.Get("bsd"); // 保存されているTTreeを読み込む
    if (!tree) {
        std::cerr << "Could not read the TTree from Sand Muon file." << std::endl;
        file.Close();
        return;
    }

    // ブランチから値を読み込むための変数を設定
    Int_t trg_sec[3];
    Double_t ct_np[5][9];
    Int_t good_spill_flag;

    tree->SetBranchAddress("trg_sec", trg_sec);
    tree->SetBranchAddress("ct_np", ct_np);
    tree->SetBranchAddress("good_spill_flag", &good_spill_flag);

    // データを読み込み、全てのPOTを積算
    Double_t total_pot = 0;

    std::vector<std::pair<Int_t, Double_t>> daily_total_pot; // Unix時間と累積POTをペアで保存

    Long64_t nentries = tree->GetEntries();
    std::cout << "Total entries in BSD tree: " << nentries << std::endl;
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        // Unix時間を取得
        Int_t unixTime = trg_sec[2] + 32400;

        // 積算POTを計算
        total_pot += ct_np[4][0];
        daily_total_pot.push_back(std::make_pair(unixTime, total_pot));
    }
    std::cout << "Total POT entries: " << daily_total_pot.size() << std::endl;
    file.Close();
    */


    
    // サンドミューオンイベント
    TString filename = "../E71b_ALL_ST/E71b_ST_data.root"; // 読み込むファイルのディレクトリ

    TFile readfile(filename); // ファイルを読み込む
    TTree *treeMu = (TTree*)readfile.Get("tree"); // 保存されているTTreeを読み込む
    if (!treeMu) {
        std::cerr << "Could not read the TTree from Sand Muon file." << std::endl;
        readfile.Close();
        return;
    }

    Int_t readadc[250];  // 読み込んだ値を詰める箱
    Int_t readunixtime;  // ここを修正: UInt_t から Int_t に変更
    Float_t pe[250];

    treeMu->SetBranchAddress("ADC", readadc);
    treeMu->SetBranchAddress("PE", pe);
    treeMu->SetBranchAddress("UNIXTIME", &readunixtime);
    
    

    // データを読み込み、全てのPOTを積算
    Int_t total_muon = 0;
    Int_t unixTimeMu;
    Int_t countX;
    Int_t countY;

    std::vector<std::pair<Int_t, Int_t>> daily_total_muon; // Unix時間と累積muonをペアで保存
    

    Long64_t nentriesMu = treeMu->GetEntries();
    
    std::cout << "Total entries in Sand Muon tree: " << nentriesMu << std::endl;
    // 総イベント数でループする
    for (Int_t ientry = 0; ientry < nentriesMu; ientry++) {
      // デバッグメッセージ
        //std::cout << "Processing entry: " << ientry << std::endl;
        std::cout<<readadc[0]<<"  "<<pe[0]<<"  "<<readunixtime<<std::endl;
        treeMu->GetEntry(ientry); // treeにientry番目の値を詰める　ここでメモリリーク？
        countX = 0; // view==1のchの数を数える
        countY = 0; // view==0のchの数を数える
        // 2.5p.e.を超えたchの出力
        for (Int_t ich = 0; ich < 250; ich++) {
            if (pe[ich] > 2.5 && ich != 50 && ich != 115) {
                if (ich < 125 || ich == 249) {
                    countY++;
                } else {
                    countX++;
                }
            }
        }

        if (countX > 1 && countY > 1) {
            unixTimeMu = readunixtime + 32400;
            // 積算サンドミューオン
            total_muon += 1;
            std::cout<<unixTimeMu<<std::endl;
            daily_total_muon.push_back(std::make_pair(unixTimeMu, total_muon));
            std::cout<<daily_total_muon.back().first<<"   "<<daily_total_muon.back().second<<std::endl;
            continue;
        }
    }
    std::cout << "Total Muon entries: " << daily_total_muon.size() << std::endl;
    


    // グラフの描写
    //TGraph *graph = new TGraph(daily_total_pot.size());
    //for (size_t i = 0; i < daily_total_pot.size(); i++) {
    //    graph->SetPoint(i, daily_total_pot[i].first, daily_total_pot[i].second);
    //}
    //graph->SetLineColor(kBlack); // グラフの色

    TGraph *graphMu = new TGraph(daily_total_muon.size());
    for (size_t i = 0; i < daily_total_muon.size(); i++) {
        graphMu->SetPoint(i, daily_total_muon[i].first, daily_total_muon[i].second);
    }
    //graphMu->SetLineColor(kRed); // グラフの色

    // グラフのスタイルを設定
    graphMu->SetTitle("NINJA Recorded Sand Muon;Date;Recorded Sand Muon");
    graphMu->GetXaxis()->SetTimeDisplay(1);
    graphMu->GetXaxis()->SetTimeFormat("%m/%d");
    //graphMu->SetLineStyle(1);
    //graphMu->GetXaxis()->SetTimeDisplay(1);
    //graphMu->GetXaxis()->SetTimeFormat("%m/%d");
    //graphMu->SetLineStyle(1);

    // アンチエイリアシングを有効にする
    gStyle->SetCanvasPreferGL(kTRUE);

    // グラフを描画
    TCanvas *c1 = new TCanvas("c1", "Recorded Sand Muon", 800, 600);
    c1->SetHighLightColor(2); // 次の線の色設定
    c1->ToggleEventStatus(); // イベントステータスの表示を切り替え
    c1->ToggleToolBar(); // ツールバーの表示を切り替え

    graphMu->Draw("ALP");
    //graph->Draw("LP same");

    // フォント設定
    Int_t fontID = 42; // Arialなどの標準フォント。日本語をサポートしているフォントIDに変更する必要があるかもしれません
    Double_t fontSize = 0.04; // フォントサイズ

    // レジェンドを作成
    TLegend *legend = new TLegend(0.1, 0.7, 0.6, 0.9); // レジェンドの位置
    legend->SetTextFont(fontID); // フォントを設定
    legend->SetTextSize(0.04); // フォントサイズを設定
    //legend->AddEntry(graph, "Total POT", "l");
    legend->AddEntry(graphMu, "Total Muon", "l");
    legend->Draw();
    
    // ファイルを閉じる
}