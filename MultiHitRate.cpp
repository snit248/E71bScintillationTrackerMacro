#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <vector>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

void MultiHitRate(){

  TString filename = "../E71b_ALL_ST/E71b_ST_data.root"; //読み込むファイルのディレクトリ

  TFile readfile(filename); //ファイルを読み込む
  TTree *tree = (TTree*)readfile.Get("tree"); //保存されているTTreeを読み込む

  Int_t readadc[250];  //読み込んだ値を詰める箱
  UInt_t readunixtime[250];

  Int_t pln[250];
  Int_t view[250];
  float_t pe[250];

  Int_t adc[250];  //各チャンネルの値を入れる箱
  UInt_t unixtime[250];

  Int_t flg_x=0;
  Int_t flg_y=0;


  //配列を含んだ場合のTTreeの読み方
  //これによってreadadcとreadunixtimeを配列として扱える
  tree->SetBranchAddress("ADC", readadc);
  tree->SetBranchAddress("UNIXTIME", &readunixtime);
  tree->SetBranchAddress("PLN", pln);
  tree->SetBranchAddress("VIEW", view);
  tree->SetBranchAddress("PE", pe);

  ////pedestalとgainの読み込み////
  Char_t buffer1[256]; //メモリ領域の確保
  TString gainname = "./Gainlist_new_bymyeyes.dat"; //読み込むdatファイルの名前
  ifstream gainfile(gainname);  //gainnameの読み込み

  ////ファイルが開けているかの確認////
  if(!gainfile.is_open()){
    cerr << "Could not open the file - " << gainname << endl;
  }

  gainfile.getline(buffer1, 100);
  Int_t tmp1;  //仮の箱
  Double_t pedestal[250]; //読み込んだpedestalとgainを入れる箱
  Double_t gain[250];

  for(Int_t index=0; index<250; index++){
    gainfile >> tmp1 >> pedestal[index] >> gain[index];  //データを1行ずつ読み込む
    //cout << index << " " << pedestal[index] << " " << gain[index] << endl;  //読み込んだデータをcout
  }

  ofstream fout("../Analysis/MultiHitRate_x2y2.dat");

  Double_t mergelight[250]; //計算した光量(mergelight)を入れる箱
  Int_t i = 0;


  ////総イベント数でループする////
  for(Int_t ientry=0; ientry<tree->GetEntries(); ientry++){
    tree->GetEntry(ientry); //treeにientry番目の値を詰める
    Int_t N = 0; //カウンター、2.5p.e.以上のチャンネルを数える
    Int_t countX = 0; //view==1のchの数を数える
    Int_t countY = 0; //view==0のchの数を数える

    ////初期化////
    for(int i=0; i<250; i++){
      adc[i] = 0;
      unixtime[i] = 0;
      mergelight[i] = 0;
    }


    ////イベント毎の各チャンネルのadcとunixtimeを詰め込む////
    ////mergelightの計算////
    for(Int_t ich=0; ich<250; ich++){
      adc[ich] = readadc[ich];
      unixtime[ich] = readunixtime[ich];
      mergelight[ich] = (adc[ich]-pedestal[ich])/gain[ich];
    }

    Int_t Ypln[4]={0,0,0,0}, Xpln[4]={0,0,0,0};

    ////2.5p.e.を超えたchが存在するentry numberとunixtimeを出力////
    ////2.5p.e.を超えたchの出力////
    for(Int_t ich=0; ich<250; ich++){
      if(pe[ich]>2.5&&ich!=50&&ich!=115){
        ////view=0////
        if(view[ich]==0){
          Ypln[pln[ich]]++;
        }else{
        ////view=1////
          Xpln[pln[ich]]++;
        }
      }
   }

    for(Int_t ipln=0; ipln<4; ipln++){
      if(Xpln[ipln]>1){
        flg_x++;
      }
      if(Ypln[ipln]>1){
        flg_y++;
      }
    }
    
    if(flg_x>>1 && flg_y>>1){
      i++;
      cout << ientry << " " << unixtime[0] << " " << i << endl;
      fout << ientry << " " << unixtime[0] << " " << i << endl;
    }

    flg_x = 0;
    flg_y = 0;
 }
}
