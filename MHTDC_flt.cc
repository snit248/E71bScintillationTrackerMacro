#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <stdlib.h>


void MHTDC_flt() {
    // ROOTファイルを開く。ここでは"your_file.root"と仮定します。
    // 実際のファイル名に置き換えてください。
    TFile *file = new TFile("./lt_All_1700986953_1701075597_conv.root");
    //1700735626 E71b 1 11/23

    // TTreeを読み込む。"your_tree"を実際のツリー名に置き換えてください。
    TTree *tree = (TTree*)file->Get("new_tree");

    // ヒストグラムを描画
    tree->Draw("LEADTIME>>(100,0,4100)","LEADTIME!=-1");
    //tree->Draw("LEADTIME!=-1");

    // キャンバスを更新して、縦軸を対数に設定
    gPad->Update(); 
    gPad->SetLogy(); 
}
