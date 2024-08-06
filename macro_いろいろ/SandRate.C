//#define DEBUG

void SandRate(Int_t subrun = 0){

  using namespace std;

  TString qsdfilename[1] = {"../merge/merge_MR86b.root"};
  TString trkfilename[1] = {"../merge/All_1618018135_conv.root"};
  Int_t daystart[1]   = {1617980400}; //2021-04-10 00:00:00
  Int_t rangestart[1] = {1617980400};
  //Int_t rangeend[1]   = {1619535600}; //2021-04-22 00:00:00
  Int_t rangeend[1]   = {1619103600}; //2021-04-23 00:00:00

  ////BSD (QSD) File Preparation////
  //TString qsdfilename = "../merge/merge_MR86b.root";
  TFile *qsdfile = new TFile(qsdfilename[subrun]);
  TTree *bsd = (TTree*)qsdfile->Get("bsd");

  Int_t trig_sec[3];
  Int_t spill_flag;
  Int_t good_spill_flag;
  Double_t ct_np[5][9];
  bsd->SetBranchAddress("trg_sec", trig_sec);
  bsd->SetBranchAddress("spill_flag", &spill_flag);
  bsd->SetBranchAddress("good_spill_flag", &good_spill_flag);
  bsd->SetBranchAddress("ct_np", ct_np);


  ////Tracker Merged File Preparation////
  TFile *trkfile = new TFile(trkfilename[subrun]);
  TTree *trktree = (TTree*)trkfile->Get("tree");

  Float_t pe[250];
  UInt_t unixtime[250];
  trktree->SetBranchAddress("PE", pe);
  trktree->SetBranchAddress("UNIXTIME", unixtime);


  ////Variables for BSD Read////
  const Int_t nbsd = bsd->GetEntries();
  cout << "Total Number of Entries in the BSD File is " << nbsd << endl;
  vector<Double_t> bdate, bentry, bpot; //可変長配列の用意
  Double_t totalpot;


  ////Variables for Tracker Read////
  const Int_t ntrk = trktree->GetEntries();
  Int_t itrk = 0;
  cout << "Total Number of Entries in the Tracker Data File is " << ntrk << endl;
  Int_t nscint = 0;
  Int_t nhit = 0;
  vector<Float_t> tdate, tentry, tpot;


  //Int_t daystart=1573052400;
  //Int_t daystart=1578927600;
  Int_t oneday = 86400; //1日=86400秒
  Double_t daypot; //Daily total pot
  vector<Double_t> day, pot, event, dayerr, yerr;
  trktree->GetEntry(ntrk-1);
  const UInt_t maxunixtime = unixtime[0];
  cout << "maxunixtime：" << maxunixtime << endl;
  bsd->GetEntry(nbsd-1);
  const Int_t maxbsdtime = trig_sec[0];
  cout << "maxbsdtime：" << maxbsdtime << endl;

  trktree->GetEntry(itrk);


  //////////////////////////////////////////////////////////////////////////////
  //for(int ibsd=70949; ibsd<427180; ibsd++){
  //for(int ibsd=0; ibsd<nbsd; ibsd++){
  for(int ibsd=70949; ibsd<nbsd; ibsd++){
    bsd->GetEntry(ibsd);

    //trackerのmaxunixtimeを超えたらループ終了
    if(maxunixtime<trig_sec[0]){
      cout << endl;
      cout << "maxunixtime：" << maxunixtime << endl;
      cout << "trig_sec[0]：" << trig_sec[0] << endl;
      cout << "End here!";
      break;
    }

    if(!good_spill_flag) continue;    //もしgood_spill_flag=1でなかったら、ループの最初に戻る

    totalpot += ct_np[4][0];  //potの総数を計算
    ct_np[4][0] /= 1.e15; //potを10^15で割っていく, # of events/10^15potを出すため
    daypot += ct_np[4][0];  //potを10^15で割ったものを足していく


    //--------------------------------------------------------------------------
    while(unixtime[0]<=trig_sec[0]){  //trackerのunixtimeがビームデータのtrig_secの間にあるとき

      //もし時間の差が1以内だったら
      if(trig_sec[0]-unixtime[0]<=1){
	      for(int i=0; i<250; i++){
	        if(pe[i]>2.5) nscint++;  //2.5p.e.を超えたchをカウント
	      }
	      break;
      }

      itrk++;
      trktree->GetEntry(itrk);


      while(unixtime[0]>maxunixtime){
	       itrk++;
	       trktree->GetEntry(itrk);
      }
    }
    //--------------------------------------------------------------------------

    //閾値を超えたchが4つ以上ならカウント
    if(nscint>3) nhit++;

    nscint = 0; //nscintの初期化

    //配列day,dayerr,pot,event.,yerrに値を詰め込んでいく
    if(trig_sec[0]>=daystart[subrun]+oneday){
      day.push_back(daystart[subrun]+9*3600+oneday/2);
      dayerr.push_back(oneday/2.);
      pot.push_back(daypot);
      event.push_back(nhit/daypot);
      yerr.push_back(sqrt(nhit)/daypot);
#ifdef DEBUG
      cout << nhit << " " << daypot << endl;
#endif
      daystart[subrun] += oneday; //1日を足す
      daypot = 0.;  //初期化
      nhit = 0;     //初期化
    }
    //プログレス出力
    if(ibsd%1==0) fprintf(stderr, "\r%d events in the BSD file are finished!!",ibsd);
  }
  //////////////////////////////////////////////////////////////////////////////

  cout << endl;


  ////Graph Preparation////
  Int_t n = pot.size(); //size()は配列の要素数を調べる
  TGraphErrors *g = new TGraphErrors(n, &day[0], &event[0], &dayerr[0], &yerr[0]);
  g->SetMarkerStyle(20);
  gStyle->SetTimeOffset(-788918400);  //ROOT時間 1995年1月1日00:00:00からの経過時間
  g->GetYaxis()->SetTitleOffset(0.9);
  g->GetYaxis()->SetRangeUser(0.6, 0.8);
  g->GetXaxis()->SetTimeDisplay(1); //横軸に時間を使いたい時の関数
  g->GetXaxis()->SetTimeFormat("%m/%d");  //month/day
  g->GetXaxis()->SetRangeUser(rangestart[subrun], rangeend[subrun]);
  g->GetXaxis()->SetNdivisions(n+2);


  ////event[]の合計と平均を求める////
  Double_t sum_event = 0; //event[j]の合計
  Double_t ave_event; //event[j]の平均
  Int_t N = 0; //jのカウンター

  for(int j=0; j<n ;j++){
    N++;
    sum_event += event[j];
    cout << j << ":" << day[j] << " " << event[j] << " " << dayerr[j] << " " << yerr[j] << endl;
  }
  ave_event = sum_event/N;
  cout << "ave_event=" << ave_event << endl;



  char buf[128];
  time_t labeltime = rangestart[subrun]-(time_t)oneday*2;
  //time_t labeltime=1573052400-86400*2;
  //time_t labeltime=1578927600-86400*2;

  struct tm *ptm;
  for(int nbin=0; nbin<n+3; nbin++){
    labeltime += (time_t)oneday;
    ptm = localtime(&labeltime);
    strftime(buf, sizeof(buf), "%m/%d", ptm);
    //if(nbin==0||nbin>43) g->GetXaxis()->ChangeLabel(nbin,90,0.,-1,-1,-1,"");
    g->GetXaxis()->ChangeLabel(nbin,90,.025,-1,-1,-1,buf);
  }

  g->SetTitle("Event Rate of the NINJA Tracker (Run9);;# of events/10^{15} protons");
  //g->SetTitle("Event Rate of the NINJA Tracker (Physics Run a-1);;# of events/10^{15} protons");
  //g->SetTitle("Event Rate of the NINJA Tracker (Physics Run a-2);;# of events/10^{15} protons");

#ifdef DEBUG
  cout << n << endl;
#endif

  TCanvas *c1 = new TCanvas("c1","Event Rate Plot",1000,600);
  c1->SetGrid();
  c1->SetBottomMargin(1.2);
  g->Draw("AP");

/*
  ////プロットの平均の直線を引く////
  time_t xmin = 1617980400; //2021-04-10 00:00:00
  time_t xmax = 1619017200; //2021-04-22 00:00:00

  cout << "xmin:" << xmin << "  xmax:" << xmax << endl;

  TLine *l;

  l = new TLine(xmin, ave_event, xmax, ave_event);

  l->SetLineStyle(2);
  l->SetLineColor(2);
  l->SetLineWidth(2);

  l->Draw("same");
*/



  ////プロットの平均の直線を引く////
  TF1 *line = new TF1("line","[0]");
  line->SetLineColor(kRed);
  line->SetLineStyle(2);
  line->SetLineWidth(4);
  line->SetParName(0, "Event Rate");
  g->Fit(line);
  line->Draw("SAME");
  //gStyle->SetOptFit(1111);  //fit情報を出す
                            //左から確率、χ2/Ndof、エラー、パラメータを表示

  Double_t par = line->GetParameter(0);
  cout << "average：" << par << endl;


  TText *tex = new TText();
  TString TotalPot = Form("Total POT: %.3e", totalpot);
  tex->DrawTextNDC(0.15,0.8,TotalPot);

  //c1->SaveAs(Form("../fig/ntracker_sandrate_%d.pdf",subrun+1));
  //c1->SaveAs("../fig/ntracker_sandrate_2.pdf");
  //c1->SaveAs(Form("../fig/ntracker_sandrate_%d.png",subrun+1));
  //c1->SaveAs("../fig/ntracker_sandrate_2.png");

}
