#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TSystem.h>

void MultiHitTDC_dist() {
    TFile *file = new TFile("../E71b_ALL_ST/E71b_ST_data.root");
    if (!file || file->IsZombie()) {
        std::cerr << "File could not be opened!" << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("tree");
    if (!tree) {
        std::cerr << "Tree could not be found!" << std::endl;
        return;
    }

    // ヒストグラムを描画する際の条件を指定
    // ここでは、LEADTIMEの条件を直接Draw関数内で使用します。
    tree->Draw("LEADTIME>>(300,0,4100)","LEADTIME-TRAILTIME > 40");

    // キャンバスを取得し、縦軸を対数に設定
    TCanvas *c1 = (TCanvas*)gROOT->GetListOfCanvases()->At(0);
    if (c1) {
        c1->SetLogy();
        c1->Update();
    }
}
