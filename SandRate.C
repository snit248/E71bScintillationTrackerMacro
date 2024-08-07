//#define DEBUG

void SandRate(Int_t subrun = 0){

  using namespace std;

  TString qsdfilename[1] = {"../../POT/E71b_ALL_POT/E71bALLPOT.root"};
  TString trkfilename[1] = {"../E71b_ALL_ST/E71b_ST_data.root"};
  Int_t daystart[1]   = {1700665200}; //2021-11-23 00:00:00
  Int_t rangestart[1] = {1700665200};
  Int_t rangeend[1]   = {1708819200}; //2024-02-25 00:00:00

  // BSD (QSD) File Preparation
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

  // Tracker Merged File Preparation
  TFile *trkfile = new TFile(trkfilename[subrun]);
  TTree *trktree = (TTree*)trkfile->Get("tree");

  Float_t pe[250];
  Int_t unixtime[250];
  trktree->SetBranchAddress("PE", pe);
  trktree->SetBranchAddress("UNIXTIME", unixtime);

  // Variables for BSD Read
  const Int_t nbsd = bsd->GetEntries();
  cout << "Total Number of Entries in the BSD File is " << nbsd << endl;
  vector<Double_t> bdate, bentry, bpot;
  Double_t totalpot = 0.0;

  // Variables for Tracker Read
  const Int_t ntrk = trktree->GetEntries();
  Int_t itrk = 0;
  cout << "Total Number of Entries in the Tracker Data File is " << ntrk << endl;
  Int_t nscint = 0;
  Int_t nhit = 0;
  Int_t nhit_total = 0;
  vector<Float_t> tdate, tentry, tpot;

  Int_t oneday = 86400; // 1日=86400秒
  Double_t daypot = 0.0; // 初期化
  vector<Double_t> day, pot, event, dayerr, yerr;
  trktree->GetEntry(ntrk-1);
  const Int_t maxunixtime = unixtime[0];
  cout << "maxunixtime：" << maxunixtime << endl;
  bsd->GetEntry(nbsd-1);
  const Int_t maxbsdtime = trig_sec[0];
  cout << "maxbsdtime：" << maxbsdtime << endl;

  trktree->GetEntry(itrk);

  for(int ibsd = 0; ibsd < nbsd; ibsd++) {
    bsd->GetEntry(ibsd);

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

    while(unixtime[0] <= trig_sec[0]) {
      if(trig_sec[0] - unixtime[0] <= 1) {
        for(int i = 0; i < 250; i++) {
          if(pe[i] > 2.5) nscint++;
        }
        break;
      }

      itrk++;
      trktree->GetEntry(itrk);

      while(unixtime[0] > maxunixtime) {
        itrk++;
        trktree->GetEntry(itrk);
      }
    }

    if(nscint > 3){
      nhit++;
      nhit_total++;
    }

    nscint = 0;

    if(trig_sec[0] >= daystart[subrun] + oneday) {
      day.push_back(daystart[subrun] + 9 * 3600 + oneday / 2);
      dayerr.push_back(oneday / 2.);
      pot.push_back(daypot);
      if (daypot != 0) { // daypotがゼロでないことを確認
        event.push_back(nhit / daypot);
        yerr.push_back(sqrt(nhit) / daypot);
      } else {
        event.push_back(0.);
        yerr.push_back(0.);
      }

      daystart[subrun] += oneday;
      daypot = 0.0;
      nhit = 0;
    }

    if(ibsd % 100 == 0) fprintf(stderr, "\r%d events in the BSD file are finished!!", ibsd);
  }

  cout << endl;

  Int_t n = day.size();  // 修正: nを宣言し、dayのサイズで初期化

  Double_t ave_event;

  //debag
  for(int j = 0; j < n; j++) {
    // UNIXタイムを日時に変換して出力
    TDatime date(day[j]);
    cout << j << ": " << date.AsString() << " " << event[j] << " " << dayerr[j] << " " << yerr[j] << " " << pot[j] << endl;
  }
  
  ave_event = nhit_total*1.e15 / totalpot;
  cout << "ave_event=" << ave_event << endl;

  TGraphErrors *g = new TGraphErrors(n, &day[0], &event[0], &dayerr[0], &yerr[0]);
  g->SetMarkerStyle(20);
  gStyle->SetTimeOffset(-788918400);
  g->GetYaxis()->SetTitleOffset(0.9);
  g->GetYaxis()->SetRangeUser(0., 1.0); // Adjusted to show both datasets
  g->GetXaxis()->SetTimeDisplay(1);
  g->GetXaxis()->SetTimeFormat("%m/%d");
  g->GetXaxis()->SetRangeUser(rangestart[subrun], rangeend[subrun]);
  g->GetXaxis()->SetNdivisions(n + 2);
  g->SetTitle("Event Rate of the NINJA Tracker (E71b);Date(2023-2024);# of events/10^{15} protons");

  TCanvas *c1 = new TCanvas("c1", "Event Rate Plot", 2000, 1200);
  c1->SetGrid();
  c1->SetBottomMargin(1.2);

  g->Draw("AP");

gStyle->SetOptFit(1111);

TText *tex = new TText();
TString TotalPot = Form("Total POT: %.3e", totalpot);
tex->DrawTextNDC(0.45, 0.6, TotalPot);

c1->Print("../../graphs/event_rate_daybyday_e71b.jpg");

}