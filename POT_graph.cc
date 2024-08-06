#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <vector>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

void POT_graph() {
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

    // ブランチから値を読み込むための変数を設定
    Int_t trg_sec[3];
    Double_t ct_np[5][9];
    Int_t good_spill_flag;
    Int_t spill=0;

    tree->SetBranchAddress("trg_sec", &trg_sec);
    tree->SetBranchAddress("ct_np", &ct_np);
    tree->SetBranchAddress("good_spill_flag", &good_spill_flag);

    // データを読み込み、全てのPOTを積算
    Double_t total_pot = 0;
    Double_t total_pot_st = 0;
    Double_t total_pot_stles = 0;

    std::vector<std::pair<Int_t, Double_t>> daily_total_pot; // Unix時間と累積POTをペアで保存
    std::vector<std::pair<Int_t, Double_t>> daily_total_pot_st; // Unix時間とSTへの累積POTをペアで保存
    std::vector<std::pair<Int_t, Double_t>> daily_total_pot_stles; //Unix時間とSTとLESへの累積POTをペアで保存
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        //if(good_spill_flag==1){
            //std::cout<<good_spill_flag<<std::endl;
            // Unix時間を取得
            Int_t unixTime = trg_sec[2] + 32400;

            // 積算POTを計算
            total_pot += ct_np[4][0];
            daily_total_pot.push_back(std::make_pair(unixTime, total_pot));
        //    }

        // STグラフ
        // ここに total_pot_st の計算とデータポイントの追加を行う
        if((trg_sec[2]>=1700735626 && trg_sec[2]<=1700898139) || (trg_sec[2]>=1700903564 && trg_sec[2]<=1701070993) || (trg_sec[2]>=1701075572 && trg_sec[2]<=1701232497) || (trg_sec[2]>=1701239897 && trg_sec[2]<=1701316261) || (trg_sec[2]>=1701336021 && trg_sec[2]<=1701415009) || (trg_sec[2]>=1701431185 && trg_sec[2]<=1701478641) || (trg_sec[2]>=1701478689 && trg_sec[2]<=1702409668) || (trg_sec[2]>=1702446582 && trg_sec[2]<=1702518509) || (trg_sec[2]>=1702533973 && trg_sec[2]<=1702703554) || (trg_sec[2]>=1702797691 && trg_sec[2]<=1703136419) || (trg_sec[2]>=1703173071 && trg_sec[2]<=1703489501) || (trg_sec[2]>=1707637325 && trg_sec[2]<=1708187346) || (trg_sec[2]>=1708219820)){
        total_pot_st += ct_np[4][0];
        daily_total_pot_st.push_back(std::make_pair(unixTime, total_pot_st));
        //printf("%d\n",unixTime);
        }

        // STとシフターグラフ
        // シフター欠損区間 2023/12/1  23:40頭から～23:46一杯 1701441600から1701442020
        // ここに total_pot_stles の計算とデータポイントの追加を行う
        if((trg_sec[2]>=1700735626 && trg_sec[2]<=1700898139) || (trg_sec[2]>=1700903564 && trg_sec[2]<=1701070993) || (trg_sec[2]>=1701075572 && trg_sec[2]<=1701232497) || (trg_sec[2]>=1701239897 && trg_sec[2]<=1701316261) || (trg_sec[2]>=1701336021 && trg_sec[2]<=1701415009) || (trg_sec[2]>=1701431185 && trg_sec[2]<=1701441600) || (trg_sec[2]>=1701442020 && trg_sec[2]<=1701478641) || (trg_sec[2]>=1701478689 && trg_sec[2]<=1702409668) || (trg_sec[2]>=1702446582 && trg_sec[2]<=1702518509) || (trg_sec[2]>=1702533973 && trg_sec[2]<=1702703554) || (trg_sec[2]>=1702797691 && trg_sec[2]<=1703136419) || (trg_sec[2]>=1703173071 && trg_sec[2]<=1703489501) || (trg_sec[2]>=1707637325 && trg_sec[2]<=1708187346) || (trg_sec[2]>=1708219820)){
        total_pot_stles += ct_np[4][0];
        daily_total_pot_stles.push_back(std::make_pair(unixTime, total_pot_stles));
        spill++;
        }
    }
    printf("ST efficiency : %lf\n",total_pot_st/total_pot);
    printf("ST and LES efficiency : %lf\n",total_pot_stles/total_pot);
    printf("Total spill : %d\n",spill);
    printf("Toal POT : %lf\n",total_pot);
    printf("Toal POT(ST recorded) : %lf\n",total_pot_stles);

    // グラフを作成
    TGraph *graph = new TGraph();
    for (size_t i = 0; i < daily_total_pot.size(); i++) {
        graph->SetPoint(i, daily_total_pot[i].first, daily_total_pot[i].second);
    }
    graph->SetLineColor(kBlack); // グラフの色を設定

    // STグラフ
    TGraph *graph_st = new TGraph();
    for (size_t i = 0; i < daily_total_pot_st.size(); i++) {
        graph_st->SetPoint(i, daily_total_pot_st[i].first, daily_total_pot_st[i].second);
    }
    graph_st->SetLineColor(kBlue); // グラフの色を設定

    // STとLESグラフ
    TGraph *graph_stles = new TGraph();
    for (size_t i = 0; i < daily_total_pot_stles.size(); i++) {
        graph_stles->SetPoint(i, daily_total_pot_stles[i].first, daily_total_pot_stles[i].second);
    }
    graph_stles->SetLineColor(kRed); // グラフの色を設定

    // グラフのスタイルを設定
    graph->SetTitle("Accumulated POT of NINJA;Date;Accumulated POT");
    graph->GetXaxis()->SetTimeDisplay(1);
    graph->GetXaxis()->SetTimeFormat("%m/%d");

    // グラフを描画
    graph->Draw("ALP");
    graph_st->Draw("LP same");
    graph_stles->Draw("LP same");

    // フォント設定
    Int_t fontID = 42; // Arialなどの標準フォント。日本語をサポートしているフォントIDに変更する必要があるかもしれません
    Double_t fontSize = 0.04; // フォントサイズ

    // レジェンドを作成
    TLegend *legend = new TLegend(0.1, 0.7, 0.6, 0.9); // レジェンドの位置
    legend->SetTextFont(fontID); // フォントを設定
    legend->SetTextSize(0.04); // フォントサイズを設定
    legend->AddEntry(graph, "Total POT", "l");
    legend->AddEntry(graph_st, "Measured POT by ST", "l");
    legend->AddEntry(graph_stles, "Measured POT by ST and LES", "l");
    legend->Draw();
    
    // ファイルを閉じる
    file->Close();
}