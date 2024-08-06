//#define DEBUG

void SandRate(Int_t subrun = 0){

  using namespace std;
  gStyle->SetOptStat(0);
  TString qsdfilename[2] = {"../bsd/merge/bsd_merge.root",
			    "../bsd/merge/bsd_merge_2.root"};
  TString trkfilename[2] = {"../merge/All_1573054729_conv.root",
			    "../merge/All_1578809458_conv.root"};
  Int_t daystart[2]   = {1573052400, 1578927600};
  Int_t rangestart[2] = {1573052400, 1578927600};
  Int_t rangeend[2]   = {1576767600, 1581519600};

  /*BSD (QSD) File Preparation*/
  //TString qsdfilename = "../bsd/merge/bsd_merge.root";
  //TString qsdfilename = "../bsd/merge/bsd_merge_2.root";
  TFile *qsdfile = new TFile(qsdfilename[subrun]);
  TTree *bsd = (TTree*)qsdfile->Get("bsd");

  Int_t trig_sec[3];
  Int_t spill_flag;
  Int_t good_spill_flag;
  Double_t ct_np[5][9];
  bsd->SetBranchAddress("trg_sec",     trig_sec);
  bsd->SetBranchAddress("spill_flag", &spill_flag);
  bsd->SetBranchAddress("good_spill_flag", &good_spill_flag);
  bsd->SetBranchAddress("ct_np",       ct_np);
  
  /*Tracker Merged File Preparation*/
  //TString trkfilename = "../merge/All_1573054729_conv.root";//run a-1
  //TString trkfilename = "../merge/All_1578809458_conv.root";//run a-2
  TFile *trkfile = new TFile(trkfilename[subrun]);
  TTree *trktree = (TTree*)trkfile->Get("tree");

  Float_t pe[250];
  UInt_t unixtime[250];
  trktree->SetBranchAddress("PE", pe);
  trktree->SetBranchAddress("UNIXTIME", unixtime);

  /*Variables for QSD Read*/
  const Int_t nbsd = bsd->GetEntries();
  cout<<"Total Number of Entries in the QSD File is "<<nbsd<<endl;
  vector<Double_t> bdate, bentry, bpot;
  Double_t totalpot;

  /*Variables for Tracker Read*/
  const Int_t ntrk = trktree->GetEntries();
  Int_t itrk=0;
  cout<<"Total Number of Entries in the Tracker Data File is "<<ntrk<<endl;
  Int_t nscint=0;
  Int_t nhit=0;
  vector<Float_t> tdate, tentry, tpot;


  //Int_t daystart=1573052400;
  //Int_t daystart=1578927600;
  Int_t oneday=86400;
  Double_t daypot;//Daily total pot
  vector<Double_t> day, pot, event, dayerr, yerr;
  trktree->GetEntry(ntrk-1);
  const UInt_t maxunixtime = unixtime[0];
  bsd->GetEntry(nbsd-1);
  const Int_t maxbsdtime = trig_sec[0];

  trktree->GetEntry(itrk);

  for(int ibsd=0; ibsd<nbsd; ibsd++){
    bsd->GetEntry(ibsd);
    if(!good_spill_flag) continue;
    totalpot+=ct_np[4][0];
    ct_np[4][0] /= 1.e15;
    daypot += ct_np[4][0];


    while(unixtime[0]<=trig_sec[0]){
      if(trig_sec[0]-unixtime[0]<=1){
	for(int i=0; i<250; i++){
	  if(pe[i]>2.5) nscint++;
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
    if(nscint>3) nhit++;
    nscint=0;
    if(trig_sec[0]>=daystart[subrun]+oneday){
      day.push_back(daystart[subrun]+9*3600+oneday/2);
      dayerr.push_back(oneday/2.);
      pot.push_back(daypot);
      event.push_back(nhit/daypot);
      yerr.push_back(sqrt(nhit)/daypot);
#ifdef DEBUG
      cout << nhit << " " << daypot << endl;
#endif
      daystart[subrun] += oneday;
      daypot=0.;
      nhit=0;
    }
    if(ibsd%1000==0) fprintf(stderr, "\r%d events in the QSD file are finished!!",ibsd);
  }
  cout << endl;

  Int_t n = pot.size();
  TGraphErrors *g = new TGraphErrors(n, &day[0], &event[0], &dayerr[0], &yerr[0]);
  g->SetMarkerStyle(20);
  gStyle->SetTimeOffset(-788918400);
  g->GetYaxis()->SetTitleOffset(0.9);
  g->GetYaxis()->SetRangeUser(0.7, 0.9);
  g->GetXaxis()->SetTimeDisplay(1);
  g->GetXaxis()->SetTimeFormat("%m/%d");
  g->GetXaxis()->SetRangeUser(rangestart[subrun], rangeend[subrun]);
  //g->GetXaxis()->SetRangeUser(1573052400, 1576767600);
  //g->GetXaxis()->SetRangeUser(1578927600, 1581519600);
  g->GetXaxis()->SetNdivisions(n+2);
  char buf[128];
  time_t labeltime=rangestart[subrun]-(time_t)oneday*2;
  //time_t labeltime=1573052400-86400*2;
  //time_t labeltime=1578927600-86400*2;
  struct tm *ptm;
  for(int nbin=0; nbin<n+2+1; nbin++){
    labeltime += (time_t)oneday;
    ptm=localtime(&labeltime);
    strftime(buf, sizeof(buf), "%m/%d", ptm);
    if(nbin==0||nbin>43) g->GetXaxis()->ChangeLabel(nbin,90,0.,-1,-1,-1,"");
    g->GetXaxis()->ChangeLabel(nbin,90,.03,-1,-1,-1,buf);
      }
  g->SetTitle(Form("Event Rate of the NINJA Tracker (Physics Run a-%d);;# of events/10^{15} protons",subrun+1));
  //g->SetTitle("Event Rate of the NINJA Tracker (Physics Run a-1);;# of events/10^{15} protons");
  //g->SetTitle("Event Rate of the NINJA Tracker (Physics Run a-2);;# of events/10^{15} protons");
 
#ifdef DEBUG
  cout<<n<<endl;
#endif
  TCanvas *c1 = new TCanvas("c1","Event Rate Plot",2000,600);
  c1->SetGrid();
  c1->SetBottomMargin(1.2);
  g->Draw("AP");

  TF1 *line = new TF1("line","[0]");
  line->SetLineColor(kRed);
  line->SetLineStyle(2);
  line->SetLineWidth(2);
  line->SetParName(0, "Event Rate");
  g->Fit(line);
  line->Draw("SAME");

  TText *tex = new TText();
  TString TotalPot = Form("Total POT: %.3e", totalpot);
  tex->DrawTextNDC(0.15,0.8,TotalPot);

  c1->SaveAs(Form("../fig/ntracker_sandrate_%d.pdf",subrun+1));
  //c1->SaveAs("../fig/ntracker_sandrate_2.pdf");
  c1->SaveAs(Form("../fig/ntracker_sandrate_%d.png",subrun+1));
  //c1->SaveAs("../fig/ntracker_sandrate_2.png");
}
