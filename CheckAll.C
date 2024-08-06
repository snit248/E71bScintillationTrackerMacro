#include <iostream>
#include <fstream>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TF1.h>

void CheckAll()
{

  const Int_t NumId = 250; // チャンネル総数

  //// BSD (QSD) File Preparation ////
  TString bsdfilename = "../merge/merge_MR86b.root";
  TFile *bsdfile = new TFile(bsdfilename);
  TTree *bsd = (TTree *)bsdfile->Get("bsd");

  Int_t trig_sec[3];
  Int_t spill_flag;
  Int_t good_spill_flag; // 物理的に解析可能なデータ
  Double_t ct_np[5][9];  // pot
  bsd->SetBranchAddress("trg_sec", trig_sec);
  bsd->SetBranchAddress("spill_flag", &spill_flag);
  bsd->SetBranchAddress("good_spill_flag", &good_spill_flag);
  bsd->SetBranchAddress("ct_np", ct_np);

  //// Tracker Preparation ////
  TString filename = "../merge/All_1618018135_conv.root";
  TFile *file = new TFile(filename);
  TTree *tree = (TTree *)file->Get("tree");

  const Int_t nentry = tree->GetEntries(); // エントリー総数

  // trackerの各値を入れる配列
  Int_t adc[NumId] = {};
  UInt_t unixtime[NumId] = {};
  Int_t tl[NumId] = {}; // lead time
  Int_t tt[NumId] = {}; // trail time
  Float_t pe[NumId] = {};

  tree->SetBranchAddress("ADC", adc);
  tree->SetBranchAddress("UNIXTIME", unixtime);
  tree->SetBranchAddress("LEADTIME", tl);
  tree->SetBranchAddress("TRAILTIME", tt);
  tree->SetBranchAddress("PE", pe);

  //// channl.datの読み込み ////
  // viewとchを読み込む
  std::ifstream map("./src/channel.dat");
  char buf[256];
  map.getline(buf, 100);
  int tmp;
  int view[NumId], ch[NumId];
  for (int id = 0; id < NumId; id++)
  {
    map >> tmp >> tmp >> tmp >> view[id] >> tmp >> ch[id];
  }

  TString viewname[2] = {"Y", "X"};
  TH1I *hithist[2];
  TCanvas *hitc[2];

  // X,Yそれぞれのモジュールのヒットヒストグラムとキャンバスの用意
  for (int i = 0; i < 2; i++)
  {
    hithist[i] = new TH1I(Form("hithist%d", i), "Number of hits in " + viewname[i] + ";# of planes;Entries", 10, 0, 10);
    hitc[i] = new TCanvas(Form("hitc%d", i), Form("hitc%d", i));
  }

  // trackerのヒットヒストグラムとキャンバスの用意
  TH1I *tothist = new TH1I("tothist", "Number of hits in the tracker;# of hits;Entries", 10, 0, 10);
  TCanvas *totc = new TCanvas("totc", "totc");

  // Leadtimeのヒストグラムとキャンバスの用意
  TH1I *leadhist = new TH1I("leadhist", "Leadtime Distribution;Leading time [ns];Entries", 1024, 0, 4096);
  TCanvas *leadc = new TCanvas("leadc", "leadc");

  // 初期化
  Int_t nhitstot = 0;
  Int_t nhitstmp = 0;
  Int_t bunbo, bunsi;
  bunbo = 0;
  bunsi = 0;

  Int_t nhits[2] = {};
  Int_t tdcflag[NumId] = {};
  Int_t tdchits[8] = {};

  Double_t x[6], y[6], xerr[6], yerr[6];

  // 初期化
  for (int i = 0; i < 6; i++)
  {
    x[i] = i + 1; // x[5]={0, 1, 2, 3, 4, 5}
    xerr[i] = 0.; // xerr[5]={0.0, 0.0, 0.0, 0.0, 0.0}
    y[i] = 0.;    // y[5]={0.0, 0.0, 0.0, 0.0, 0.0}
    yerr[i] = 0.; // yerr[5]={0.0, 0.0, 0.0, 0.0, 0.0}
  }

  int nbsd = bsd->GetEntries(); // ビームのエントリー総数

  bsd->GetEntry(nbsd - 1);
  const int maxbsdtime = trig_sec[0];

  tree->GetEntry(nentry - 1);
  const int maxunixtime = unixtime[0];

  int ientry = 0;
  tree->GetEntry(ientry);

  // for(int ibsd=0; ibsd<430000; ibsd++){
  // for(int ibsd=0; ibsd<nbsd; ibsd++){
  for (int ibsd = 70949; ibsd < nbsd; ibsd++)
  {
    bsd->GetEntry(ibsd);

    if (maxunixtime < trig_sec[0])
    {
      std::cout << "End when ibsd is " << ibsd << std::endl;
      break;
    }

    if (good_spill_flag == 1)
    {

      while (unixtime[0] <= trig_sec[0])
      {

        if (trig_sec[0] - unixtime[0] <= 1)
        {
          bunbo++; // ビームデータとtrackerで時間が一致したヒットの数

          for (int id = 0; id < NumId; id++)
          {
            if (view[id] == -1)
              continue; // Dead chは飛ばす

            // ヒット数のカウント
            if (pe[id] > 2.5)
            {
              nhits[view[id]]++; // X,Y各モジュールのそれぞれのヒットの数
              nhitstot++;        // hitのtotal
              nhitstmp++;        // nhitstotと同じ数、なんのために？
            }

            tdcflag[id] = -1; // tdcflagを-1に初期化、なんで-1？

            // TDCヒットのカウント
            if (nhitstmp > 0)
            { // ヒットが1つでもあれば
              for (int itdc = 0; itdc < 8; itdc++)
              {
                // for(int itdc=1; itdc<7; itdc++){ //itdcはバンチ差1,2,3,4,5,6

                // if(fabs(4080-580*itdc-tl[id])<100&&tl[id]-tt[id]>40){
                if (fabs(4080 - 580 * itdc - tl[id]) < 100 && tl[id] > 0)
                {
                  // if(tl[id]-tt[id]>40){
                  // if(tl[id]>0){
                  tdchits[itdc]++;
                  tdcflag[id] = itdc; //バンチの場所
                }
              } // itdc
            }
          } // id

          // hithistにFillしていく
          for (int iview = 0; iview < 2; iview++)
          {
            hithist[iview]->Fill(nhits[iview]);
            nhits[iview] = 0;
          }

          // tothistにFillしていく
          tothist->Fill(nhitstot);
          nhitstot = 0;

          //複数イベントであれば
          if (nhitstmp > 1)
            bunsi++; // bunsiは複数イベントの数

          for (int itdc = 0; itdc < 8; itdc++)
          {
            // for(int itdc=1; itdc<8; itdc++){
            if (tdchits[itdc] > 3)
            {
              if (itdc > 0 && itdc < 7)
                y[itdc - 1]++;
              for (int id = 0; id < 250; id++)
              {
                if (tdcflag[id] == itdc)
                {
                  leadhist->Fill(tl[id]);
                }
              }
            }
            tdchits[itdc] = 0;
          }

          nhitstmp = 0;
          break;
        } // fi trig-unix<1

        ientry++;
        tree->GetEntry(ientry);

        while (unixtime[0] > maxunixtime)
        {
          ientry++;
          tree->GetEntry(ientry);
        }
      } // end while unix<trig
    }   // fi spill flag

    /*
        for(int j=0; j<6; j++) cout << y[j]<< " ";
        cout << endl;
    */
  } // ibsd

  std::cout << "y = ";
  for (int j = 0; j < 6; j++)
    std::cout << y[j] << ", ";
  std::cout << std::endl;

  std::cout << "ienrty : " << ientry << " "
            << "bunsi : " << bunsi << " ";
  std::cout << "bunbo : " << bunbo << " "
            << "bunsi/bunbo : " << (Double_t)bunsi / bunbo << std::endl;
  std::cout << "bunsi/ientry : " << (Double_t)bunsi / ientry << std::endl;

  for (int iview = 0; iview < 2; iview++)
  {
    hitc[iview]->cd();
    hithist[iview]->Draw();
  }

  totc->cd();
  tothist->Draw();

  leadc->cd();
  leadhist->SetStats(0);
  leadhist->Draw();
  gPad->SetLogy();

  for (int i = 0; i < 6; i++)
  {
    yerr[i] = sqrt(y[i]);
  }

  TGraphErrors *g = new TGraphErrors(6, x, y, xerr, yerr);
  TCanvas *cg = new TCanvas("cg", "cg");
  cg->cd();
  // g->SetMarkerStyle(20);
  g->GetXaxis()->SetRangeUser(0, 10);
  g->GetYaxis()->SetRangeUser(0, 3.e3);
  g->GetYaxis()->SetTitleOffset(1.2);
  g->SetTitle("TDC check;Bunch difference;Entries");
  g->Draw("AP");
  // gStyle->SetOptFit(1111);

  TF1 *line = new TF1("line", "[0]*(x-[1])", 0, 9);
  line->SetParameter(1, 8.);
  g->Fit(line, "", "", .5, 7.5);
}
