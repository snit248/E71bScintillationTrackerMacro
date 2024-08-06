#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TString.h>
#include <iostream>
#include <fstream>

void Gausfit() {
    TString ifilename = "../E71b_ALL_ST/E71b_ST_data.root";  // 読み込むファイル名
    TFile *file = new TFile(ifilename);  // ファイルを読み込む
    TTree *tree = (TTree*)file->Get("tree");  // TTreeの作成

    Char_t buffer[256];
    TString mapname = "../src/channel_new.dat";
    ifstream map(mapname);
    map.getline(buffer, 100);
    Int_t tmp;
    Int_t module[250], ch_mod[250];
    Int_t view[250], pln[250], ch[250];

    for(Int_t index=0; index<250; index++){
      map>>tmp>>module[index]>>ch_mod[index]>>view[index]>>pln[index]>>ch[index];
      #ifdef DEBUG
      cout<<module[index]<<" "<<ch_mod[index]<<endl;
      #endif
    }

    Int_t readADC[250];
    tree->SetBranchAddress("ADC", readADC);

    Int_t ch_N = 250; //チャンネル総数(All)
    //Int_t ch_N = 64; // チャンネル総数
    Int_t  canvas_N = 16; //キャンバスの数(All)
    //Int_t canvas_N = 4; // キャンバスの数

    TCanvas *c[canvas_N];

    TH1F *h[ch_N];
    for(Int_t icanvas = 0; icanvas < canvas_N; icanvas++) {
        c[icanvas] = new TCanvas(Form("c%d", icanvas), Form("c%d", icanvas), 1200, 800);
        c[icanvas]->Divide(4, 4);  // 1枚のキャンバスを4行4列に分割
    }

    // Npeak.datに書き込んで出力
    ofstream fout("../src/Gausfit.dat");  // All

    // チャンネル数でループする
    for(Int_t i = 0; i < ch_N; i++) {
        std::cout << i << std::endl;

        h[i] = new TH1F(Form("h%d", i), Form("h%d", i), 200, 650, 1050);  // ヒストグラムの定義
        c[i / 16]->cd(i % 16 + 1);  // キャンバス内の区画移動
        gPad->SetLogy();  // Logスケールにする
        tree->Draw(Form("ADC[%d]>>h%d", i, i));  // 各チャンネルのADCのグラフを描く

        TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 0, 0);

        //上チップ
        if(0<=ch_mod[i] && ch_mod[i]<=31){
            if(module[i]==0){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 750, 875);
                fitFunc->SetParameters(200000, 780, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       500, 820, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==1){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 770, 870);
                fitFunc->SetParameters(200000, 810, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       530, 848, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==2){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 795, 920);
                fitFunc->SetParameters(270000, 810, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       600, 884, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==3){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 790, 890);
                fitFunc->SetParameters(270000, 810, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       950, 862, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else{
                continue;
            }
        }
        //下チップ
        else if(32<=ch_mod[i] && ch_mod[i]<=63){
            if(module[i]==0){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 750, 875);
                fitFunc->SetParameters(200000, 780, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       500, 820, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==1){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 770, 895);
                fitFunc->SetParameters(200000, 810, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       500, 850, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==2){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 795, 895);
                fitFunc->SetParameters(250000, 815, 7,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       1000, 862, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else if(module[i]==3){
                // 初期値を設定して3つのガウス関数をフィッティング
                //ダブルガウシアン
                TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3)", 790, 900);
                fitFunc->SetParameters(270000, 810, 6,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
                                       600, 875, 12);  // 第2ガウス関数の初期値
                h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット
            }
            else{
                continue;
            }
        }
        else{
            continue;
        }

        //トリプルガウシアン
        //TF1 *fitFunc = new TF1("fitFunc", "gaus(0) + gaus(3) + gaus(6)", 800, 950);
        //fitFunc->SetParameters(90000, 820, 20,  // 第1ガウス関数の初期値 (振幅, 平均, 標準偏差)
        //                       2000, 865, 20,  // 第2ガウス関数の初期値
        //                       350, 910, 20); // 第3ガウス関数の初期値
        //h[i]->Fit(fitFunc, "R");  // ヒストグラムにフィット

        // フィット結果を取得
        Double_t peak1 = fitFunc->GetParameter(1);  // 第1ガウス関数の平均
        Double_t peak2 = fitFunc->GetParameter(4);  // 第2ガウス関数の平均
        //Double_t peak3 = fitFunc->GetParameter(7);  // 第3ガウス関数の平均

        //トリプルガウシアン
        //fout << i << " " << peak1 << " " << peak2 << " " << peak3 << std::endl;  // 結果をファイルに書き込む

        //ダブルガウシアン
        fout << i << " " << peak1 << " " << peak2 << std::endl;  // 結果をファイルに書き込む

        h[i]->Draw("same");  // ヒストグラムを再描画してフィット結果を表示
    }

    fout.close();  // ファイルを閉じる
}