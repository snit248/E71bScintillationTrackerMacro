void EventRate(Int_t subrun = 0){

  using namespace std;

  /*BSD (QSD) File Preparation*/
  TString qsdfilename[2] = {"../bsd/merge/bsd_merge.root",
			    "../bsd/merge/bsd_merge_2.root"};
  TString trkfilename[2] = {"../merge/All_1573054729_conv.root",
			    "../merge/All_1578809458_conv.root"};
  Int_t daystart[2]   = {1573052400, 1578927600};
  Int_t rangestart[2] = {1573052400, 1578927600};
  Int_t rangeend[2]   = {1576767600, 1581519600};

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
  TFile *trkfile = new TFile(trkfilename[subrun]);
  TTree *trktree = (TTree*)trkfile->Get("tree");

  UInt_t unixtime[250];
  trktree->SetBranchAddress("UNIXTIME", unixtime);

  /*Variables for QSD Graphs*/
  const Int_t nbsd = bsd->GetEntries();
  cout<<"Total Number of Entries in the QSD File is "<<nbsd<<endl;
  vector<Float_t> bdate, bentry, bpot;
  Int_t goodentry=0;
  Double_t pottot=0;

  /*Variables for Tracker Graphs*/
  const Int_t ntrk = trktree->GetEntries();
  Int_t itrk=0;
  cout<<"Total Number of Entries in the Tracker Data File is "<<ntrk<<endl;
  vector<Float_t> tdate, tentry, tpot;
  Int_t goodtrk=0;
  Double_t tpottot=0;
  Float_t ratio=0;

  Float_t effentry, effpot;

  trktree->GetEntry(ntrk-1);
  const UInt_t maxunixtime = unixtime[0];
  bsd->GetEntry(nbsd-1);
  const Int_t maxbsdtime = trig_sec[0];

  trktree->GetEntry(itrk);

  for(Int_t ibsd=0; ibsd<nbsd; ibsd++){//Loop for Entries in the QSD File
    bsd->GetEntry(ibsd);

    if(good_spill_flag==1){//If spill_flag=1, then
      goodentry++;
      pottot+=ct_np[4][0];
      bentry.push_back(goodentry);
      bdate.push_back(trig_sec[0]);
      bpot.push_back(pottot);

      while(unixtime[0]<=trig_sec[0]){
	if(trig_sec[0]-unixtime[0]<=1){
	  goodtrk++;
	  tentry.push_back(goodtrk);
	  tdate.push_back(unixtime[0]);
	  tpottot += ct_np[4][0];
	  tpot.push_back(tpottot);
	  //cout<<unixtime[0]<<" "<<trig_sec[0]<<endl;
	  break;

	}
	itrk++;
	trktree->GetEntry(itrk);
	while(unixtime[0]>maxunixtime){
	  itrk++;
	  trktree->GetEntry(itrk);
	}
      }
    }//fi spill_flag=1

    if(ibsd%1000==0) {
      ratio = (Float_t)ibsd/nbsd;
      ratio *= 100.;
      fprintf(stderr, "\r%d events in the QSD file are finished!! (%.0f%)", ibsd, ratio);
    }
    //cout<<ibsd<<endl;
  }//End Loop of QSD Entries

  fprintf(stderr, "\nAll events in the QSD file are finished!!\n");
  cout<<unixtime[0]<<" "<<trig_sec[0]<<endl;

  //Show Efficiency in Console
  effentry = (Float_t)goodtrk/goodentry;
  effentry *= 100.;
  effpot = tpottot/pottot;
  effpot *= 100.;
  cout<<goodtrk<<" "<<goodentry<<endl;
  cout<<"Total Delivered POT is "<<pottot<<endl;
  cout<<"Total Recorded POT is "<<tpottot<<endl;
  cout<<"Tracker Data Taking Efficiency is "<<effentry<<"% (Number of Entries)"<<endl;
  cout<<"Tracker Data Taking Efficiency is "<<effpot<<"% (Total POT)"<<endl;

  
  //Create Graph of Entries
  cout<<"Start Creating Graphs..."<<endl;

  TCanvas *c1 = new TCanvas("c1","Status of the NINJA Tracker (# of Events)");

  //Graph of Tracker Entries
  TGraph *tg = new TGraph(goodtrk, &tdate[0], &tentry[0]);
  tg->SetTitle("Number of Entries of the NINJA Tracker;Date;Entries");

  //Convert Unixtime to Date
  gStyle->SetTimeOffset(-788918400);
  tg->GetXaxis()->SetTimeDisplay(1);
  tg->GetXaxis()->SetTimeFormat("%m/%d");
  tg->GetXaxis()->SetTimeOffset(9, "gmt");
  
  tg->Draw("AP");

  //Graph of QSD Entries
  TGraph *bg = new TGraph(goodentry, &bdate[0], &bentry[0]);

  bg->SetMarkerColor(kRed);
  bg->SetLineColor(kRed);

  gStyle->SetTimeOffset(-788918400);
  bg->GetXaxis()->SetTimeDisplay(1);
  bg->GetXaxis()->SetTimeFormat("%m/%d");
  bg->GetXaxis()->SetTimeOffset(9, "gmt");

  bg->Draw("SAME P");

  //Make Legend
  TLegend *leg = new TLegend(0.6, 0.15, 0.85, 0.35);
  leg->AddEntry(tg,  "Tracker Triggered", "l");
  leg->AddEntry(bg, "QSD w/ spill_flag=1", "l");
  leg->SetFillColor(0);
  leg->Draw();

  cout<<"Creating Graph of the Number of Entries is Finished!!"<<endl;

  //Create Graph of POT
  TCanvas *c2 = new TCanvas("c2", "Status of the NINJA Tracker (Accumulated POT)");

  //Graph of Tracker POT
  TGraph *tpotg = new TGraph(goodtrk, &tdate[0], &tpot[0]);
  tpotg->SetTitle("Accumulated POT of the NINJA Tracker;Date;Accumulated POT");

  gStyle->SetTimeOffset(-788918400);
  tpotg->GetXaxis()->SetTimeDisplay(1);
  tpotg->GetXaxis()->SetTimeFormat("%m/%d");
  tpotg->GetXaxis()->SetTimeOffset(9, "gmt");

  tpotg->GetYaxis()->SetMaxDigits(2);
  tpotg->Draw("AP");

  //Graph of QSD POT
  TGraph *bpotg = new TGraph(goodentry, &bdate[0], &bpot[0]);

  bpotg->SetMarkerColor(kRed);
  bpotg->SetLineColor(kRed);

  gStyle->SetTimeOffset(-788918400);
  bpotg->GetXaxis()->SetTimeDisplay(1);
  bpotg->GetXaxis()->SetTimeFormat("%m/%d");
  bpotg->GetXaxis()->SetTimeOffset(9, "gmt");

  bpotg->GetYaxis()->SetMaxDigits(2);
  bpotg->Draw("SAME P");

  TLegend *legpot = new TLegend(0.6, 0.15, 0.85, 0.35);
  legpot->AddEntry(tpotg, "Tracker Triggered", "l");
  legpot->AddEntry(bpotg, "BSD w/ good_spill_flag=1", "l");
  //legpot->AddEntry(bpotg, "QSD w/ spill_flag=1", "l");
  legpot->SetFillColor(0);
  legpot->Draw();

  cout<<"Creating Graph of the Accumulated POT is Finished!!"<<endl;
  
  c1->SaveAs("../fig/ntracker_eve_rate.png");
  c2->SaveAs("../fig/ntracker_pot.png");
}

/*int main(int argc, char *argv[]){
  TApplication app("app", &argc, argv);
  app.Init();

  EventRate();
  app.Run();

  return 0;
  }*/
