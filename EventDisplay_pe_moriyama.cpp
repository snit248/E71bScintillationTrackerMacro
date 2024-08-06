//あるEvent numberにおけるEvent Display
//チャンネルごとの光量を反映
void EventDisplay_pe_moriyama(){ //Entry numberを指定

  const Int_t nindex = 250;

  const Float_t threshold = 3.5;

  const Int_t offsety[4]  = {20,  4, 16,  0};
  const Int_t offsetx[4]  = { 0, 16,  4, 20};
  const Int_t offsetz[4]  = { 1,  7, 10, 16};


  ////読み込むファイルの指定////
  TString dirname="../E71b_ALL_ST/";
  TString mergefilename = dirname+"E71b_ST_data.root";
  cout << "Reading：" << mergefilename << endl;
  TFile *mergefile = new TFile(mergefilename);
  TTree *mergetree = (TTree*)mergefile->Get("tree");

  Int_t adc[nindex];
  UInt_t unixtime[nindex];
  Float_t pe[nindex];

  mergetree->SetBranchAddress("ADC", adc);
  mergetree->SetBranchAddress("UNIXTIME", unixtime);
  mergetree->SetBranchAddress("PE", pe);

  mergetree->GetEntries();


  ////EntryMatch_20210415.datの読み込み////
  const Int_t nline = 46838;  //行数
  ifstream fin1("./MultiHitRate.dat");
  Int_t tmp1;
  Int_t trkentry[nline];
  for(Int_t iline=0; iline<nline; iline++){
    fin1 >> tmp1 >> trkentry[iline];
    //cout << trkentry[iline] << endl;
  }


  ////channel.datの読み込み////
  Char_t buffer[256];
  TString mapname = "./src/channel_new.dat";
  ifstream map(mapname);
  Int_t tmp;
  Int_t module[nindex], view[nindex], pln[nindex], ch[nindex];
  map.getline(buffer, 100);
  for(Int_t index=0; index<nindex; index++){
    map>>tmp>>module[index]>>tmp>>view[index]>>pln[index]>>ch[index];
  }



  ///MatchしたEntry numberのEventDisplay////
  for(Int_t ientry=0; ientry<mergetree->GetEntries(); ientry++){
    mergetree->GetEntry(ientry);

    //(Int_t iline=0; iline<nline; iline++){
      //if(ientry==trkentry[iline]){
      if(ientry==1){

        ////キャンバスの準備////
        TCanvas *c = new TCanvas("c", "c", 750, 1000);
        c->Divide(2,1);
        c->cd(1)->DrawFrame(0 , -500, 20, 500, "Event Display (Sideview);z [mm];y [mm]");
        c->cd(2)->DrawFrame(20, -500, 40, 500, "Event Display (Topview);z [mm];x [mm]");


        ////チャンネルごとの光量を反映させるための2次元ヒストグラム////
        //TH2F *名前 = new TH2F("名前", "タイトル", xのビン数, xmin, xmax, yのビン数, ymin, ymax);
        TH2F *sidehist = new TH2F("sidehist", "", 20, 0,  20, 250, -500, 500);
        sidehist->SetMinimum(0);
        sidehist->SetMaximum(20);
        TH2F *tophist  = new TH2F("tophist",  "", 20, 20, 40, 250, -500, 500);
        tophist->SetMinimum(0);
        tophist->SetMaximum(20);

        TBox *scint = new TBox();

        cout << "Entry number：" << ientry << endl;

        cout << "-----Unixtime of each module-----" << endl;

        Int_t tdcflag = 0;

        for(Int_t index=0; index<250; index++){

          ////各モジュールのunixtimeが一致しているかを確認////
          if(index==0||index==16||index==141||index==156) cout << "Module No：" << module[index] << "：" << unixtime[index] << endl;

          ////光量がthresholdを超えているか////
          if(pe[index]<threshold) continue; //thresholdより小さかったらループを抜ける

          tdcflag = 1;
          if(view[index]==0){
            for(Int_t z=0; z<3; z++){
              for(Int_t y=0; y<6; y++){
                sidehist->Fill(offsetz[pln[index]]+1.*z, -500+32*ch[index]+offsety[pln[index]]+4.*y, pe[index]);
              }
            }
          }
          else if(view[index]==1){
            for(Int_t z=0; z<3; z++){
              for(Int_t x=0; x<6; x++){
                tophist->Fill(20+offsetz[pln[index]]+1.*z, 500-32*ch[index]-offsetx[pln[index]]-4.*(x+1), pe[index]);
              }
            }
          }
        }

        ////チャンネルに色を付けていく////
        if(tdcflag){
          for(Int_t index=0; index<250; index++){

            if(index==17||index==66||index==240) scint->SetFillColor(kBlack);

            else if(index==157||index==163||index==184||index==246) scint->SetFillColor(13);

            else scint->SetFillColor(19);

            if(view[index]==0){
              c->cd(1);
              scint->DrawBox(offsetz[pln[index]], -500+32*ch[index]+offsety[pln[index]], offsetz[pln[index]]+3, -500+32*ch[index]+24+offsety[pln[index]]);
            }
            else if(view[index]==1){
              c->cd(2);
              scint->DrawBox(offsetz[pln[index]]+20, 500-32*ch[index]-offsetx[pln[index]], offsetz[pln[index]]+23, 500-32*ch[index]-24-offsetx[pln[index]]);
            }
          }
        }else{
          cerr << "No ADC data and Multi hit TDC data has no meaning..." << endl;
        }
        c->cd(1);
        sidehist->Draw("same colz");
        c->cd(2);
        tophist->Draw("same colz");
        //c->SaveAs(Form("../figures/EventDisplay_20210415/ED_pe_fiber(eve=%d,thre=%.1f).pdf", ientry, threshold));
      }
    }
  //}
  //}
}
