void Check(){

  const Int_t nmodule = 4;

  TString filename[nmodule];
  TString modulename[nmodule] = {"Y1", "Y2", "X1", "X2"};
  TString dirname = "../merge/";

  TFile *file[nmodule];
  TTree *tree[nmodule];

  Int_t adc[64] = {};
  UInt_t unixtime = 0;
  Int_t tl[64] = {};
  Int_t tt[64] = {};

  TH1I *histy = new TH1I("histy","Number of hits in Y modules",10,0,10);
  TH1I *histx = new TH1I("histx","Number of hits in X modules",10,0,10);
  TH1I *hist  = new TH1I("hist", "Number of hits in all modules",10,0,10);
  TH1I *hlead = new TH1I("hlead","Leading time distribution (ToT>~3p.e.)",512,0,4096);

  TCanvas *cy   = new TCanvas("cy",   "Hits in Y modules");
  TCanvas *cx   = new TCanvas("cx",   "Hits in X modules");
  TCanvas *ctot = new TCanvas("ctot", "Hits in all modules");

  TCanvas *ctdc = new TCanvas("ctdc", "TDC Distribution of one module");
  TCanvas *cplt = new TCanvas("cplt", "Check of Multi-hit TDC reliability");

  //Int_t ihits = 0;
  //Int_t yhits[10], xhits[10], nhits[10];
  Int_t nhits = 0;

  for(Int_t imodule=0; imodule<nmodule; imodule++){
    filename[imodule] = dirname+modulename[imodule]+"_1573054729_conv"+".root";
    file[imodule] = new TFile(filename[imodule]);
    tree[imodule] = (TTree*)file[imodule]->Get("tree");

    tree[imodule]->SetBranchAddress("ADC",       adc);
    tree[imodule]->SetBranchAddress("UNIXTIME",  &unixtime);
    tree[imodule]->SetBranchAddress("LEADTIME",  tl);
    tree[imodule]->SetBranchAddress("TRAILTIME", tt);

    Int_t nentry = tree[imodule]->GetEntries();

    for(Int_t i = 20000; i < nentry; i++){
      tree[imodule]->GetEntry(i);
      for(Int_t j = 0; j < 64; j++){
	if(adc[j]>950) nhits++;
      }
      if(imodule/2==0) histy->Fill(nhits);
      else histx->Fill(nhits);
      hist->Fill(nhits);
      nhits=0;
    }

    if(imodule==0){
      ctdc->cd();
      tree[imodule]->Draw("LEADTIME>>hlead","LEADTIME-TRAILTIME>40");
      gPad->SetLogy();
      const Int_t npoints = 6;
      Double_t x[npoints], y[npoints];
      Double_t xerr[npoints], yerr[npoints];
      for(Int_t i = 0; i < npoints; i++){
	x[i] = i+1;
	xerr[i] = 0;
	y[i] = tree[imodule]->GetEntries(Form("fabs(4080-%d*580-LEADTIME)<100&&LEADTIME-TRAILTIME>40",i+1));
	yerr[i] = sqrt(y[i]);
      }

      TGraphErrors *g = new TGraphErrors(npoints,x,y,xerr,yerr);
      cplt->cd();
      g->SetMarkerStyle(2);
      g->Draw("AP");
      
      TF1 *func = new TF1("func", "[0]*(x-[1])");
      func->SetParameter(1,8.);
      g->Fit(func);

    }
    
  }
  gPad->SetLogy(0);
  cy->cd();
  histy->Draw();
  gPad->SetLogy();
  cx->cd();
  histx->Draw();
  ctot->cd();
  hist->Draw();

}
