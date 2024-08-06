void EventDisplay(Int_t ientry = 23180){

  const Int_t nmodule = 4;

  const Int_t offset[4] = {20, 4, 16, 0};
  const Int_t slide[4] = {1, 0, 0, 1};

  TString filename;
  TString modulename[nmodule] = {"Y1", "Y2", "X1", "X2"};
  TString dirname = "../merge/";

  TFile *file[nmodule];
  TTree *tree[nmodule];

  Int_t adc[64], leadtime[64], trailtime[64];
  Int_t view[64], pln[64], ch[64];
  //Float_t pe[64];

  /*
  tree->SetBranchAddress("ihit", &ihit);
  tree->SetBranchAddress("view", view);
  tree->SetBranchAddress("pln", pln);
  tree->SetBranchAddress("ch", ch);
  tree->SetBranchAddress("pe", pe);
  */

  TH2F *sideview = new TH2F("sideview", "Event Display (Side View);z [mm];y [mm]", 4, 0, 12, 250, -500, 500);
  sideview->SetStats(0);
  TH2F *topview  = new TH2F("topview",  "Event Display (Top View);z [mm];x [mm]",  4, 12, 24, 250, -500, 500);
  topview->SetStats(0);


  TCanvas *c = new TCanvas("c", "c", 750, 1000);
  c->Divide(2,1);

  TBox *scint = new TBox();
  scint->SetFillColor(19);

  Bool_t flag = 0;

  for(Int_t imodule = 0; imodule < nmodule; imodule++){
    filename= dirname+modulename[imodule]+"_1573054729_conv.root";
    file[imodule] = new TFile(filename);
    tree[imodule] = (TTree*)file[imodule]->Get("tree");


    tree[imodule]->SetBranchAddress("ADC", adc);
    tree[imodule]->SetBranchAddress("LEADTIME", leadtime);
    tree[imodule]->SetBranchAddress("TRAILTIME", trailtime);


    tree[imodule]->GetEntry(ientry);
    
    for(Int_t i = 0; i < 64; i++){
      view[i] = imodule/2;
      pln[i] = (i%32)/8;
      ch[i] = i%8 + (i/32)*8 + 16*slide[imodule];
      if(view[i]==-1&&(leadtime[i]==-1||leadtime-trailtime<40)) continue;
      else if(view[i]==0){
	for(Int_t j = 0; j < 6; j++){
	  sideview->Fill(3*pln[i],-500+offset[pln[i]]+32*ch[i]+j*4,leadtime[i]);
	}
      }else if(view[i]==1){
	for(Int_t j = 0; j < 6; j++){
	  topview->Fill(12+3*pln[i],-500+offset[pln[i]]+32*ch[i]+j*4,leadtime[i]);
	}
      }
    }

    c->cd(1)->DrawFrame(0,-500,12,500, "Event Display (Sideview);z [mm];y [mm]");
    for(Int_t ipln = 0; ipln < 4; ipln++){
      for(Int_t ich = 0; ich < 31; ich++){
	scint->DrawBox(3*ipln, -500 + 32*ich + offset[ipln], 3*(ipln+1), -500 + 32*ich + 24 + offset[ipln]);
      }
    }
    sideview->Draw("colz same");
    
    c->cd(2)->DrawFrame(12,-500,24,500,"Event Display (Topview);z [mm];x [mm]");
    for(Int_t ipln = 0; ipln < 4; ipln++){
      for(Int_t ich = 0; ich < 31; ich++){
	scint->DrawBox(12 +3*ipln, -500 + 32*ich + offset[ipln], 12 + 3*(ipln+1), -500 + 32*ich + 24 + offset[ipln]);
      }
    }
    topview->Draw("colz same");
  }

  //TGraph *sidegraph = new TGraph(n, x, y);
  //TGraph *topgraph  = new TGraph(n, x, y); 

}
