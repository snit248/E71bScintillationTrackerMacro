#include <TFile.h>
#include <TTree.h>
#include <iostream>

void analyze_leadtime(const char* rootfile_name) {
    // ファイルを開く
    TFile *f = TFile::Open(rootfile_name, "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "ファイルを開けませんでした: " << rootfile_name << std::endl;
        return;
    }

    // TTreeオブジェクトを取得
    TTree *tree = (TTree*)f->Get("tree");
    if (!tree) {
        std::cerr << "TTreeを取得できませんでした。" << std::endl;
        f->Close();
        return;
    }

    // ブランチに変数を接続
    const int MAX_CHANNELS = 64;
    int LEADTIME[MAX_CHANNELS];
    tree->SetBranchAddress("LEADTIME", LEADTIME);

    // 各チャンネルのカウント用配列を初期化
    int count[MAX_CHANNELS] = {0};

    // エントリーをループしてカウント
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        for (int ch = 0; ch < MAX_CHANNELS; ch++) {
            if (LEADTIME[ch] != -1) {
                count[ch]++;
            }
        }
    }

    // 結果を出力
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
        std::cout << "チャンネル " << ch << ": -1でないエントリー数 = " << count[ch] << std::endl;
    }

    // ファイルを閉じる
    f->Close();
}

void Check_LT_entries() { // この関数名をファイル名と同じにしてください
    const char* rootfile_name = "../merge/X2_1700708898_conv.root"; // ここにROOTファイルの名前を入れてください
    analyze_leadtime(rootfile_name);
}