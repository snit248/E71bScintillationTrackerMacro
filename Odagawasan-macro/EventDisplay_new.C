//#define DEBUG 1


void EventDisplay_new(Int_t ientry = 23180){

  const Float_t offset[4] = {25*5/6., 25/6., 25*2/3., 0.};

  const Float_t ntoffset[2] = {-5., -30.};
  const Float_t offsetz[4]  = { 1.,  7., 10., 16.};
  const Float_t scioffset = 44.8;
  const Float_t zgap[2] = {1.3284, 3.3284};//cm

  TString filename = "../../merge/All_1707637325_conv.root";
  TFile *file = new TFile(filename);
  TTree *tree = (TTree*)file->Get("tree");

  Int_t adc[250];
  Int_t leadt[250], trailt[250];
  UInt_t unixt[250];
  Float_t pe[250];
  Int_t view[250], pln[250], ch[250];
  
  tree->SetBranchAddress("ADC", adc);
  tree->SetBranchAddress("LEADTIME", leadt);
  tree->SetBranchAddress("TRAILTIME", trailt);
  tree->SetBranchAddress("UNIXTIME", unixt);
  tree->SetBranchAddress("PE", pe);
  tree->SetBranchAddress("VIEW", view);
  tree->SetBranchAddress("PLN", pln);
  tree->SetBranchAddress("CH", ch);
  
  tree->GetEntry(ientry);

  TCanvas *c_all = new TCanvas("c1", "Event Display (Overall)", 750, 1000);

  c_all->Divide(2,1);

  TH2F *sideview = new TH2F("sideview", "Event Display (Side View);z [mm];y [mm]", 20, 0, 20, 7800, -650, 650);
  sideview->SetStats(0);
  sideview->SetMinimum(0);
  sideview->SetMaximum(50);
  TH2F *topview  = new TH2F("topview",  "Event Display (Top View);z [mm];x [mm]",  20, 20, 40, 7800, -650, 650);
  topview->SetStats(0);
  topview->SetMinimum(0);
  topview->SetMaximum(50);

  for(Int_t ihits = 0; ihits < 250; ihits++){

    if(view[ihits]==0&&pe[ihits]>2.5){
      for(int z=0; z<3; z++){
	for(Int_t j = 0; j < 144; j++){
	  sideview->Fill(offsetz[pln[ihits]]+1.*z,-448.+offset[pln[ihits]]+25.*4/3*ch[ihits]+j/6.+1/12.,pe[ihits]);
	}
      }
    }else if(view[ihits]==1&&pe[ihits]>2.5){
      for(int z=0; z<3; z++){
	for(Int_t j = 0; j < 144; j++){
	  topview->Fill(20+offsetz[pln[ihits]]+1.*z,448.-offset[3-pln[ihits]]-25.*4/3*ch[ihits]-j/6.-1/12.,pe[ihits]);
	}
      }
    }
  }
  
  c_all->cd(1)->DrawFrame(0,-650,20,650,"Event Display (Sideview);z [mm];y [mm]");

  TBox *scint = new TBox();
  scint->SetFillColor(19);
  for(Int_t ipln = 0; ipln < 4; ipln++){
    for(Int_t ich = 0; ich < 31; ich++){
      scint->DrawBox(offsetz[ipln], -448. + 25.*4/3*ich + offset[ipln], offsetz[ipln]+3, -448. + 25.*4/3*ich + 24 + offset[ipln]);
    }
  }

  sideview->Draw("colz same");

  /*
  for(int itrk=0; itrk<ntrk; itrk++){
    TLine *ytrack = new TLine(0,10*(postrue[itrk][0]-1.*angtrue[itrk][0]),20,10*(postrue[itrk][0]+1.*angtrue[itrk][0]));
    if(pid[itrk]==13) ytrack->SetLineColor(kRed);
    ytrack->Draw("same");
    
    TGraph *yreconpoint = new TGraph();
    yreconpoint->SetPoint(0, 10, posrecon[itrk][0]*10.);
    yreconpoint->SetMarkerStyle(4);
    yreconpoint->SetMarkerSize(1);
    yreconpoint->Draw("same P");
    
    TGraph *yextpoint = new TGraph();
    yextpoint->SetPoint(0, 10, postrue[itrk][0]*10.);
    yextpoint->SetMarkerStyle(2);
    yextpoint->SetMarkerSize(1);
    yextpoint->Draw("same P");
  }
  */

  c_all->cd(2)->DrawFrame(20,-650,40,650,"Event Display (Topview);z [mm];x [mm]");
  
  for(Int_t ipln = 0; ipln < 4; ipln++){
    for(Int_t ich = 0; ich < 31; ich++){
      scint->DrawBox(20 +offsetz[ipln], 448 - 25.*4/3*ich - offset[3-ipln], 20+offsetz[ipln]+3, 448 - 25.*4/3*ich - 24 - offset[3-ipln]);
    }
  }

  
  topview->Draw("same colz");
  /*
  for(int itrk=0; itrk<ntrk; itrk++){
    TLine *xtrack = new TLine(20,10*(postrue[itrk][1]-1.*angtrue[itrk][1]),40,10*(postrue[itrk][1]+1.*angtrue[itrk][1]));
    if(pid[itrk]==13) xtrack->SetLineColor(kRed);
    xtrack->Draw("same");
    
    TGraph *xreconpoint = new TGraph();
    xreconpoint->SetPoint(0, 30, posrecon[itrk][1]*10);
    xreconpoint->SetMarkerStyle(4);
    xreconpoint->SetMarkerSize(1);
    xreconpoint->Draw("same P");
    
    TGraph *xextpoint = new TGraph();
    xextpoint->SetPoint(0, 30, postrue[itrk][1]*10);
    xextpoint->SetMarkerStyle(2);
    xextpoint->SetMarkerSize(1);
    xextpoint->Draw("same P");
    
  }
*/
  
  TLine *frame = new TLine();
  c_all->cd(1);
  frame->SetLineColor(kRed);
  frame->DrawLine(0,610,20,610);
  frame->DrawLine(0,-610,20,-610);
  c_all->cd(2);
  frame->DrawLine(20,610,40,610);
  frame->DrawLine(20,-610,40,-610);

}
