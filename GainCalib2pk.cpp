#include <iostream>
#include <fstream>
#include <sstream>
#include <TCanvas.h>
#include <TGraph.h>
#include <TF1.h>

void GainCalib2pk() {
    ifstream fin("./rc/Gausfit.dat");  // Gausfit.datを読み込む
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open the input file." << std::endl;
        return;
    }

    ofstream fout("./rc/Gainlist.dat");  // Gainlist.datを出力
    if (!fout.is_open()) {
        std::cerr << "Error: Could not open the output file." << std::endl;
        return;
    }

    const Int_t ch = 64;  // チャンネル総数
    double tmp;
    double Y[2];  // 読み込んだピークを入れる配列
    double X[2] = {0, 1};  // X軸 0, 1

    TF1 *function = new TF1("function", "[0]*x+[1]");

    TGraph *g[ch];  // TGraphクラスのポインタを作成

    Int_t canvas_N = 4;  // キャンバスの数
    TCanvas *c[canvas_N];  // キャンバスの作成

    for (Int_t icanvas = 0; icanvas < canvas_N; icanvas++) {
        c[icanvas] = new TCanvas(Form("c%d", icanvas), Form("c%d", icanvas), 1200, 800);
        c[icanvas]->Divide(4, 4);  // 4行4列に分割
    }

    for (Int_t i = 0; i < ch; i++) {
        // 1行を読み込む
        std::string line;
        if (!std::getline(fin, line)) {
            std::cerr << "Error: Could not read data for channel " << i << std::endl;
            break;
        }

        // 文字列ストリームを使って、行を分割してデータを読み取る
        std::istringstream iss(line);
        if (!(iss >> tmp >> Y[0] >> Y[1])) {
            std::cerr << "Error: Incorrect data format in line " << i + 1 << std::endl;
            break;
        }

        // 読み込んだデータの確認
        std::cout << "Channel " << i << ": Y[0] = " << Y[0] << ", Y[1] = " << Y[1] << std::endl;

        g[i] = new TGraph(2, X, Y);  // new TGraph(プロットの数, 配列名, 配列名);

        g[i]->Fit(function);  // functionをFitさせる

        g[i]->SetTitle(Form("h%d", i));  // グラフにタイトルをつける
        g[i]->GetXaxis()->SetTitle("p.e.");  // X軸の名前
        g[i]->GetYaxis()->SetTitle("adc");  // Y軸の名前
        g[i]->SetMarkerStyle(8);  // プロットの形
        g[i]->SetMarkerColor(1);  // プロットの色
        g[i]->SetMarkerSize(0.5);  // プロットのサイズ

        c[i / 16]->cd(i % 16 + 1);  // キャンバスの区画移動
        g[i]->Draw("AP");  // APはDrawオプション、A:グラフにまわりに軸表示、P:マーカーでプロット

        double gain = function->GetParameter(0);  // 0番目のパラメータ[0]の値を得る、[0]はグラフの傾き=ゲイン
        fout << i << " " << Y[0] << " " << gain << std::endl;  // i、Y[0]、gainの順に書き込み
        // iはチャンネル番号、Y[0]はぺデスタル、gainはゲイン
    }

    fin.close();  // ファイルを閉じる
    fout.close();  // ファイルを閉じる
}