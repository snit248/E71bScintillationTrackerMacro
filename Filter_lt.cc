#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>

void Filter_lt() {
    // 元のROOTファイルを開く
    //TFile *input_file = new TFile("../merge/All_1700903564_conv.root", "READ");
    //TFile *input_file = new TFile("../merge/All_1700735626_conv.root", "READ");
    TFile *input_file = new TFile("../merge/All_1700986953_1701075597_conv.root", "READ");
    TTree *input_tree = (TTree*)input_file->Get("tree");

    // LEADTIME ブランチを読み込むための変数
    Int_t leadtime[250]; // 仮定として配列サイズを250とする
    input_tree->SetBranchAddress("LEADTIME", leadtime);

    // 新しいファイルとTTreeの作成
    //TFile *output_file = new TFile("lt1700903564.root", "RECREATE");
    //TFile *output_file = new TFile("lt1700735626.root", "RECREATE");
    TFile *output_file = new TFile("lt_All_1700986953_1701075597_conv.root", "RECREATE");
    TTree *output_tree = new TTree("new_tree", "Filtered Tree");

    // 出力用の変数とブランチを設定
    Int_t out_leadtime[250]; // 仮定として配列サイズを250とする
    output_tree->Branch("LEADTIME", out_leadtime, "leadtime[250]/I");

    // 元のTTreeを走査
    Long64_t nentries = input_tree->GetEntries();
    for (Long64_t i = 0; i < nentries; ++i) {
        input_tree->GetEntry(i);

        // LEADTIMEの条件を満たす要素の数をカウント
        int count = 0;
        for (int j = 0; j < 250; ++j) {
            if (leadtime[j] != -1) {
                out_leadtime[count++] = leadtime[j];
            }
        }

        // 条件を満たす場合、新しいTTreeに値を保存
        if (count >= 5) {
            output_tree->Fill();
        }
    }

    // ファイルに書き込み
    output_tree->Write();
    output_file->Close();
    input_file->Close();
}
