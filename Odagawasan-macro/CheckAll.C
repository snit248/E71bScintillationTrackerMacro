void CheckAll(){

  const Int_t NumId = 250;

  /*BSD (QSD) File Preparation*/
  TString qsdfilename = "../bsd/merge/bsd_merge.root";
  //TString qsdfilename = "../bsd/merge/bsd_merge_2.root";
  //TString qsdfilename = "../qsd/merge/qsd_merge.root";
  TFile *qsdfile = new TFile(qsdfilename);
  TTree *bsd = (TTree*)qsdfile->Get("bsd");

  Int_t trig_sec[3];
  Int_t spill_flag;
  Int_t good_spill_flag;
  Double_t ct_np[5][9];
  bsd->SetBranchAddress("trg_sec",     trig_sec);
  bsd->SetBranchAddress("spill_flag", &spill_flag);
  bsd->SetBranchAddress("good_spill_flag", &good_spill_flag);
  bsd->SetBranchAddress("ct_np",       ct_np);

  TString filename = "../merge/All_1573054729_conv.root";
  //TString filename = "../merge/All_1578809458_conv.root";
  TFile *file = new TFile(filename);
  TTree *tree = (TTree*)file->Get("tree");

  const Int_t nentry = tree->GetEntries();

  Int_t adc[NumId] = {};
  UInt_t unixtime[NumId] = {};
  Int_t tl[NumId] = {};
  Int_t tt[NumId] = {};
  Float_t pe[NumId] = {};

  tree->SetBranchAddress("ADC", adc);
  tree->SetBranchAddress("UNIXTIME", unixtime);
  tree->SetBranchAddress("LEADTIME", tl);
  tree->SetBranchAddress("TRAILTIME", tt);
  tree->SetBranchAddress("PE", pe);


  ifstream map("channel.dat");
  char buf[256];
  map.getline(buf, 100);
  int tmp;
  int view[NumId], ch[NumId];
  for(int id=0; id<NumId; id++){
    map>>tmp>>tmp>>tmp>>view[id]>>tmp>>ch[id];
  }

  TString viewname[2] = {"Y", "X"};
  TH1I *hithist[2];
  TCanvas *hitc[2];
  for(int i=0; i<2; i++){
    hithist[i] = new TH1I(Form("hithist%d",i),"Number of hits in "+viewname[i]+";# of planes;Entries",10,0,10);
    hitc[i] = new TCanvas(Form("hitc%d",i),Form("hitc%d",i));
  }
  TH1I *tothist = new TH1I("tothist", "Number of hits in the tracker;# of hits;Entries",10,0,10);
  TCanvas *totc = new TCanvas("totc","totc");

  TH1I *leadhist = new TH1I("leadhist","Leadtime Distribution;Leading time [ns];Entries", 1024,0,4096);
  TCanvas *leadc = new TCanvas("leadc", "leadc");


  Int_t nhitstot=0;
  Int_t nhitstmp=0;
  Int_t bunbo, bunsi;
  bunbo=0; bunsi=0;

  Int_t nhits[2] = {};
  Int_t tdcflag[NumId] = {};
  Int_t tdchits[8] = {};

  Double_t x[6], y[6], xerr[6], yerr[6];
  for(int i=0; i<6; i++){
    x[i]=i+1;
    xerr[i]=0.;
    y[i]=0.;
    yerr[i]=0.;
  }

  int nbsd = bsd->GetEntries();
  bsd->GetEntry(nbsd-1);
  const int maxbsdtime = trig_sec[0];
  tree->GetEntry(nentry-1);
  const int maxunixtime = unixtime[0];
  int ientry = 0;
  tree->GetEntry(ientry);

  for(int ibsd=0; ibsd<nbsd; ibsd++){
    bsd->GetEntry(ibsd);
    //if(unixtime[0] > 1576681200||unixtime[0] < 1573052400) continue;
    //if(unixtime[0] > 1581519600||unixtime[0] < 1578927600) continue;
    if(good_spill_flag==1){

      while(unixtime[0]<=trig_sec[0]){

	if(trig_sec[0]-unixtime[0]<=1){
	  bunbo++;
	  for(int id=0; id<NumId; id++){

	    if(view[id]==-1) continue;
	    if(pe[id]>2.5){
	      nhits[view[id]]++; 
	      nhitstot++;
	      nhitstmp++;
	    }

	    tdcflag[id]=-1;
	    if(nhitstmp>0){
	      for(int itdc=0; itdc<8; itdc++){
		if(fabs(4080-580*itdc-tl[id])<60&&tl[id]>0){
		  tdchits[itdc]++;
		  tdcflag[id] = itdc;
		}
	      }//itdc
	    }
	    
	  }//id

	  for(int iview=0; iview<2; iview++){
	    hithist[iview]->Fill(nhits[iview]);
	    nhits[iview] = 0;
	  }

	  tothist->Fill(nhitstot);
	  nhitstot=0;
	  if(nhitstmp>1) bunsi++;

      	  for(int itdc=0; itdc<8; itdc++){
	    if(tdchits[itdc]>3){
	      if(itdc>0&&itdc<7) y[itdc-1]++;
	      for(int id=0; id<250; id++){
		if(tdcflag[id]==itdc){
		  leadhist->Fill(tl[id]);
		}
	      }
	    }
	    tdchits[itdc]=0;
	  }

	  nhitstmp=0;
	  break;
	}//fi trig-unix<1

	ientry++;
	tree->GetEntry(ientry);
	while(unixtime[0]>maxunixtime){
	  ientry++;
	  tree->GetEntry(ientry);
	}

      }//end while unix<trig


    }//fi spill flag

    for(int j=0; j<6; j++) cout << y[j]<< " ";
    cout << endl;
  }//ibsd
   
  cout << ientry << " " <<bunsi << " " << bunbo <<" " << (Double_t)bunsi/bunbo << endl;


  for(int iview=0; iview<2; iview++){
    hitc[iview]->cd();
    hithist[iview]->Draw();
  }

  totc->cd();
  tothist->Draw();
  
  leadc->cd();
  leadhist->Draw();
  gPad->SetLogy();
  
  for(int i=0; i<6; i++){
    yerr[i] = sqrt(y[i]);
  }
  
  TGraphErrors *g = new TGraphErrors(6,x,y,xerr,yerr);
  TCanvas *cg = new TCanvas("cg", "cg");
  cg->cd();
  //g->SetMarkerStyle(20);
  g->GetXaxis()->SetRangeUser(0,10);
  g->GetYaxis()->SetRangeUser(0,3.e3);
  g->GetYaxis()->SetTitleOffset(1.2);
  g->SetTitle("Mulit hit TDC check with linear fit;Bunch difference;Entries");
  g->Draw("AP");

  TF1 *line = new TF1("line","[0]*(x-[1])",0,9);
  line->SetParameter(1,8.);
  g->Fit(line, "","",.5,7.5);

}


