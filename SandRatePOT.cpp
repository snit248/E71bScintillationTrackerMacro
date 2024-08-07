//#define DEBUG

void SandRatePOT(){

  using namespace std;

  TString qsdfilename = {"../../POT/E71b_ALL_POT/E71bALLPOT.root"};
  TString trkfilename = {"../E71b_ALL_ST/E71b_ST_data.root"};
  Int_t daystart   = 1700665200; //2021-11-23 00:00:00
  Int_t rangestart = 1700665200;
  Int_t rangeend   = 1708819200; //2024-02-25 00:00:00

  // BSD (QSD) File Preparation
  TFile *qsdfile = new TFile(qsdfilename);
  TTree *bsd = (TTree*)qsdfile->Get("bsd");

  Int_t trig_sec[3];
  Int_t spill_flag;
  Int_t good_spill_flag;
  Double_t ct_np[5][9];
  bsd->SetBranchAddress("trg_sec", trig_sec);
  bsd->SetBranchAddress("spill_flag", &spill_flag);
  bsd->SetBranchAddress("good_spill_flag", &good_spill_flag);
  bsd->SetBranchAddress("ct_np", ct_np);

  // Tracker Merged File Preparation
  TFile *trkfile = new TFile(trkfilename);
  TTree *trktree = (TTree*)trkfile->Get("tree");

  Float_t pe[250];
  Int_t unixtime[250];
  trktree->SetBranchAddress("PE", pe);
  trktree->SetBranchAddress("UNIXTIME", unixtime);

  // Variables for BSD Read
  const Int_t nbsd = bsd->GetEntries();
  cout << "Total Number of Entries in the BSD File is " << nbsd << endl;
  //vector:double型の要素を格納、.push_back()で要素を追加
  vector<Double_t> bdate, bentry, bpot;
  Double_t totalpot = 0.0;

  // Variables for Tracker Read
  const Int_t ntrk = trktree->GetEntries();
  Int_t itrk = 0;
  cout << "Total Number of Entries in the Tracker Data File is " << ntrk << endl;
  Int_t nscint = 0;
  Int_t nhit = 0;
  vector<Float_t> tdate, tentry, tpot;

  Int_t oneday = 86400; // 1日=86400秒
  Double_t daypot = 0.0; // 初期化
  vector<Double_t> day, pot, event, dayerr, yerr;
  //一番最後のエントリーを取得し、そのunixtimeをmaxunixtimeとする
  trktree->GetEntry(ntrk-1);
  const Int_t maxunixtime = unixtime[0];
  cout << "maxunixtime：" << maxunixtime << endl;
  bsd->GetEntry(nbsd-1);
  const Int_t maxbsdtime = trig_sec[0];
  cout << "maxbsdtime：" << maxbsdtime << endl;

  //トラッカーの最初のエントリーを取得
  trktree->GetEntry(itrk);

  for(int ibsd = 0; ibsd < nbsd; ibsd++) {
    bsd->GetEntry(ibsd);

    //POTのunixtimeがトラッカーのmaxunixtimeより大きくなったら終わり
    if(maxunixtime < trig_sec[0]) {
      cout << endl;
      cout << "maxunixtime：" << maxunixtime << endl;
      cout << "trig_sec[0]：" << trig_sec[0] << endl;
      cout << "End here!";
      break;
    }

    totalpot += ct_np[4][0];
    ct_np[4][0] /= 1.e15;
    daypot += ct_np[4][0];

    //トラッカーのunixtimeがBSDのtrig_sec[0]より小さい間ループ
    while(unixtime[0] <= trig_sec[0]) {
        //BSDのtrig_sec[0]とトラッカーのunixtimeの差が1秒以下ならループを抜ける
      if(trig_sec[0] - unixtime[0] <= 1) {
        for(int i = 0; i < 250; i++) {
          if(pe[i] > 2.5) nscint++;
        }
        break;
      }

      itrk++;
      trktree->GetEntry(itrk);

      //いらない気もする
      while(unixtime[0] > maxunixtime) {
        itrk++;
        trktree->GetEntry(itrk);
      }
    }

    if(nscint > 3) nhit++;

    nscint = 0;

    //vectorに値入れていく
    if(trig_sec[0] >= daystart + oneday) {
      day.push_back(daystart + 9 * 3600 + oneday / 2);
      dayerr.push_back(oneday / 2.);
      pot.push_back(daypot);
      if (daypot != 0) { // daypotがゼロでないことを確認
        event.push_back(nhit / daypot);
        yerr.push_back(sqrt(nhit) / daypot);
      } else {
        event.push_back(0.);
        yerr.push_back(0.);
      }

      daystart += oneday;
      daypot = 0.0;
      nhit = 0;
    }

    if(ibsd % 100 == 0) fprintf(stderr, "\r%d events in the BSD file are finished!!", ibsd);
  }

  cout << endl;

  ///ここまで計算///
  ///ここからグラフ描画///

  Int_t n = day.size();  // 修正: nを宣言し、dayのサイズで初期化

  //サンドミューオンレートのグラフ
  TGraphErrors *g1 = new TGraphErrors(n, &day[0], &event[0], &dayerr[0], &yerr[0]);
  g1->SetMarkerStyle(20);
  gStyle->SetTimeOffset(-788918400);
  g1->GetYaxis()->SetTitleOffset(0.9);
  g1->GetYaxis()->SetRangeUser(0.6, 1.0); // Adjusted to show both datasets
  g1->GetXaxis()->SetTimeDisplay(1);
  g1->GetXaxis()->SetTimeFormat("%m/%d");
  g1->GetXaxis()->SetRangeUser(rangestart, rangeend);
  g1->GetXaxis()->SetNdivisions(n + 2);
  g1->SetTitle("Event Rate of the NINJA Tracker (E71b);;# of events/10^{15} protons");

  //POTのグラフ
  TH1D *g2 = new TH1D("POT", "POT", n-1, &day[0]);
  for(int i=0; i<n; i++) {
    g2->SetBinContent(i+1, pot[i]);
  }
  g2->GetXaxis()->SetTimeDisplay(1);
  g2->GetXaxis()->SetTimeFormat("%m/%d");
  g2->GetXaxis()->SetRangeUser(rangestart, rangeend);
  g2->GetXaxis()->SetNdivisions(n + 2);
  g2->SetTitle("POT of the NINJA Tracker (E71b);Date;POT (10^{15} protons)");

  TCanvas *c1 = new TCanvas("c1", "Event Rate Plot", 1600, 600);
  c1->Divide(2, 1);
  c1->SetBottomMargin(1.2);

  c1->cd(1);
  c1->SetGrid();
  g1->Draw("AP");

  c1->cd(2);
  c1->SetGrid();
  g2->Draw();

gStyle->SetOptFit(1111);
}