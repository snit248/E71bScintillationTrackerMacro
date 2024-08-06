#include <iostream>
#include <fstream>
#include <map>
#include <ctime>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"

int SandRateGraph() {
    // ファイル名
    const char* filename = "../Analysis/SandmuonRate.dat";

    // 入力ファイルのストリーム
    std::ifstream infile(filename);

    // イベント数をカウントするマップ (key: date string, value: event count)
    std::map<std::string, int> eventCounts;

    // データファイルから値を読み込む
    int RawID;
    long Unixtime;
    int SnadmuonID;

    while (infile >> RawID >> Unixtime >> SnadmuonID) {
        // Unix時間を日付に変換
        time_t t = Unixtime;
        struct tm* tm_info = localtime(&t);
        char buffer[11];
        strftime(buffer, 11, "%Y-%m-%d", tm_info);

        // マップに日付をキーとしてイベント数をカウント
        std::string date(buffer);
        eventCounts[date]++;
    }

    // ROOTのヒストグラムを作成
    TH1F* hist = new TH1F("daily_events", "Daily Event Count", eventCounts.size(), 0, eventCounts.size());

    // X軸ラベル用の配列
    std::vector<std::string> labels;
    int bin = 1;
    for (const auto& pair : eventCounts) {
        hist->SetBinContent(bin, pair.second);
        labels.push_back(pair.first);
        bin++;
    }

    // X軸のラベル設定
    for (int i = 1; i <= labels.size(); ++i) {
        hist->GetXaxis()->SetBinLabel(i, labels[i - 1].c_str());
    }

    // キャンバスを作成し、ヒストグラムを描画
    TCanvas* canvas = new TCanvas("canvas", "Daily Event Count", 800, 600);
    hist->Draw();

    // 結果を保存
    canvas->SaveAs("../graphs/daily_event_count.png");

    // メモリの解放
    delete hist;
    delete canvas;

    return 0;
}
