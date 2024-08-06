void NEntries(){

  TString filename = "../merge/All_1573054729_conv.root";
  TFile *file = new TFile(filename);
  TTree *tree = (TTree*)file->Get("tree");

  Float_t pe[250];
  tree->SetBranchAddress("PE", pe);

  TH1F *hist[2][4];
  TCanvas *canvas[2];
  for(int ilayer=0; ilayer<2; ilayer++){
    canvas[ilayer] = new TCanvas(Form("c%d",ilayer), Form("c%d",ilayer));
    for(int ipln=0; ipln<4; ipln++){
      hist[ilayer][ipln] = new TH1F(Form("hist%d%d",ilayer,ipln),"Number of Entries in Each Channel;Channel No.;Entries",31,0,31);
    }
  }

  hist[0][0]->SetStats(0);
  hist[0][0]->GetYaxis()->SetRangeUser(4000,6000);
  hist[0][0]->GetYaxis()->SetTitleOffset(1.2);
  hist[1][0]->SetStats(0);
  hist[1][0]->GetYaxis()->SetRangeUser(4000,7000);
  hist[1][0]->GetYaxis()->SetTitleOffset(1.2);

  ifstream map("../src/channel.dat");
  char buffer[256];
  map.getline(buffer, 100);
  int tmp;
  int view[250], pln[250], ch[250];
  for(int id=0; id<250; id++){
    map>>tmp>>tmp>>tmp>>view[id]>>pln[id]>>ch[id];
  }

  Int_t events[2]={};

  for(int ientry=0; ientry<tree->GetEntries(); ientry++){
    tree->GetEntry(ientry);
    for(int id=0; id<250; id++){
      if(view[id]==-1) continue;
      if(pe[id]>2.5) events[view[id]]++;
    }
      
    if(events[0]>1&&events[1]>1){
      for(int id=0; id<250; id++){
	if(pe[id]>2.5) hist[view[id]][pln[id]]->Fill(ch[id]);
      }
    }
    events[0]=0; events[1]=0;
  }

  TLegend *leg[2];
  for(int ilayer=0; ilayer<2; ilayer++) leg[ilayer] = new TLegend(0.8,0.9,0.8,0.9);
  TString viewname[2] = {"y", "x"};

  for(int ilayer=0; ilayer<2; ilayer++){
    canvas[ilayer]->cd();
    for(int ipln=0; ipln<4; ipln++){
      if(ipln==0) hist[ilayer][0]->Draw("hist");
      else hist[ilayer][ipln]->Draw("hist same");
      hist[ilayer][ipln]->SetLineColor(2+ipln);
      leg[ilayer]->AddEntry(hist[ilayer][ipln],viewname[ilayer]+Form("-%d",ipln), "lp");
    }
  leg[ilayer]->Draw("");
  }

  canvas[0]->SaveAs("../fig/yhitnumsame.pdf");
  canvas[1]->SaveAs("../fig/xhitnumsame.pdf");

}

