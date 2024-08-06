#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

//#define DEBUG

void GainCalc(){

  const Int_t nch = 250;
  const Int_t nmodule = 4;
  const Int_t ncanvas = 4;
  const TString unixtime = "1618018135";

  TString ifilename = "../merge/All_"+unixtime+"_conv.root";
  TFile *file = new TFile(ifilename);
  TTree *tree = (TTree*)file->Get("tree");

  TString mapname = "./src/channel.dat";
  ifstream map(mapname);
  Char_t buffer[256];
  map.getline(buffer, 100);
  Int_t tmp;
  Int_t module[nch], ch_mod[nch];

  ////Reading gain output file////
  TString ofilename = "../merge/gain_"+unixtime+"_test.dat";
  ofstream ofile(ofilename);

  ////Canvas Preparation////
  TCanvas *c[nmodule][ncanvas];
  TCanvas *cplot[nmodule][ncanvas];
  const TString modulename[nmodule] = {"Y1",  "Y2",  "X1",  "X2"};
  const TString order[ncanvas] = {"1st", "2nd", "3rd", "4th"};
  for(Int_t imodule=0; imodule<nmodule; imodule++){
    for(Int_t icanvas=0; icanvas<ncanvas; icanvas++){
      c[imodule][icanvas] = new TCanvas(Form("c%d%d",imodule,icanvas), modulename[imodule]+" "+order[icanvas]+" 16 channels", 1200, 800);
      c[imodule][icanvas]->Divide(4, 4);
      cplot[imodule][icanvas] = new TCanvas(Form("cplot%d%d",imodule,icanvas), modulename[imodule]+" "+order[icanvas]+" gain plot", 900, 600);
      cplot[imodule][icanvas]->Divide(4, 4);
    }
  }
 //cout << "Canvas Preparation is OK" << endl;

  ////hist for peak search////
  TH1F *hist[nch], *histzoom[nch];
  const Int_t maxpeaks = 10*2;
  TSpectrum *s = new TSpectrum(maxpeaks);
  Double_t *xpeaks = {};
  Double_t searchped, fitped;
  vector<Double_t> xpeaksmod;
  Double_t firstpeakcheck = 0;
  Double_t firstpeak = 0;
  Int_t corrangemin, corrangemax;


  ////graph for gain plot////
  TGraph *gainplot[nch];
  TF1 *func = new TF1("func","[0]*x+[1]"); //Fitする用の関数

  //for(Int_t ich=0; ich<nch; ich++){
  for(Int_t ich=0; ich<3; ich++){
    map>>tmp>>module[ich]>>ch_mod[ich]>>tmp>>tmp>>tmp;
#ifdef DEBUG
    cout<<endl<<module[ich]<<" "<<ch_mod[ich]<<endl;
#endif
    //if(module[ich]!=2) continue;

    c[module[ich]][ch_mod[ich]/16]->cd(ch_mod[ich]%16+1);
    hist[ich] = new TH1F(Form("hist%d",ich),Form("Ch %d;ADC;Entries",ich),1000,0,4000);
    tree->Draw(Form("adc[%d]>>hist%d", ich, ich));
    s->Search(hist[ich], 3., "goff", 1e-1);
    xpeaks = s->GetPositionX();
    Double_t pedestal = xpeaks[0];  //ペデスタルの時のADCの値を取得
    searchped = pedestal;
    cout << "searchped：" << searchped << endl;
    corrangemin = (Int_t)xpeaks[0]+140; //次のピークを探す範囲の設定
    corrangemax = (Int_t)xpeaks[0]+640;


    ////ADCの範囲を設定し直したヒストグラムからピークサーチ////
    //0~4000 → pedestal+140~pedestal+640 の範囲に狭めているからhistzoom
    histzoom[ich] = new TH1F(Form("histzoom%d",ich),Form("Ch %d;ADC;Entries",ich),100,corrangemin,corrangemax);
    tree->Draw(Form("adc[%d]>>histzoom%d", ich, ich));  //まずはhistzoomを描く

    Int_t nfound = s->Search(histzoom[ich], 1.75, "", 1e-2);  //ピークの数を調べる
    if(nfound<1) continue;  //もしピークが1つも検出できなかったら飛ばす
    xpeaks = s->GetPositionX(); //ピークごとのADCの値を配列xpeaksに入れていく
    for(Int_t k=0; k<nfound;k++) xpeaksmod.push_back(xpeaks[k]);  //xpeaksの値を光量の昇順に配列xpeaksmodに詰めていく
    sort(xpeaksmod.begin(), xpeaksmod.end()); //xpeaksmodの要素すなわちピークを昇順にしていく
    Double_t gain = (xpeaksmod[2]-xpeaksmod[0])/2.; //gainの計算
    cout << "Gain：" << gain << endl;
    firstpeakcheck = (xpeaksmod[0]-pedestal)/gain;  //firstpeakの光量の計算
    firstpeak = round(firstpeakcheck);  //round(): 四捨五入して整数にする
    cout << "firstpeakcheck：" << firstpeakcheck << endl;
    cout << "firstpeak：" << firstpeak << endl;

    vector<Double_t> pe;
    Double_t petmp=firstpeak;
    Int_t petmpround;
    for(Int_t k=0; k<5; k++){ //kはプロットしたいpeの数
      petmp = (xpeaksmod[k]-pedestal)/gain; //各ピークの光量を計算
      petmpround = round(petmp);  //光量を四捨五入して整数にする
      pe.push_back(firstpeak+k);  //整数にした光量を配列peに詰めていく
    }
    cout << "CH:" << ich << endl;

    ////xpeakmodとpeの要素を全部cout////
    for(auto itr=xpeaksmod.begin(); itr!=xpeaksmod.end(); ++itr){
      cout << *itr << " " << endl;
    }
    for(auto itr=pe.begin(); itr!=pe.end(); ++itr){
      cout << *itr << " " << endl;
    }

    cplot[module[ich]][ch_mod[ich]/16]->cd(ch_mod[ich]%16+1);
    gainplot[ich] = new TGraph(5, &pe[0], &xpeaksmod[0]);
    gainplot[ich]->SetTitle(Form("Gain Plot for ch %d;p.e.;ADC count",ich));
    gainplot[ich]->SetMarkerStyle(20);
    gainplot[ich]->GetXaxis()->SetRangeUser(2,8);
    gainplot[ich]->Draw("AP");
    func->SetParameter(0, gain);
    func->SetParameter(1, pedestal);
    //gainplot[ich]->Fit("func","Q","",2.5,7.5);
    gainplot[ich]->Fit("func","Q","",2,8);

    gain = func->GetParameter(0);
    pedestal = func->GetParameter(1);
    fitped = pedestal;

    //cout << ich << " Peaksearched result: " << searchped << " Fitting result: " << fitped << endl;
    cout << "Peaksearched result: " << searchped << endl;
    cout << "Fitting result: " << fitped << endl;

    //output fileに書き込み
    ofile << ich << " " << pedestal << " " << gain << endl;

    xpeaksmod.clear();
    fprintf(stderr, "\r[%d/%d]", ich+1, nch); //chのプログレス出力
    cout << endl << endl;
  }

  ofile.close();  //output fileを閉じる

/*
  ////グラフをpdf保存////
  for(Int_t imodule=0; imodule<nmodule; imodule++){
    for(Int_t icanvas=0; icanvas<ncanvas; icanvas++){
      c[imodule][icanvas]->SaveAs(Form("../figures/calibration_histo%d%d.pdf",imodule,icanvas));
      cplot[imodule][icanvas]->SaveAs(Form("../figures/calibration_plot%d%d.pdf",imodule,icanvas));
    }
  }
*/

}
