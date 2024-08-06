
void landau_fit(TString filename = "../merge/All_1573054729_conv"){

  const Int_t maxch = 250;

  TCanvas *c[4];  
  const TString order[4] = {"1st", "2nd", "3rd", "4th"};  
  for(Int_t icanvas = 0; icanvas < 4; icanvas++){
    c[icanvas] = new TCanvas(Form("c%d",icanvas), order[icanvas]+" 16 cannels", 1200, 800);
    c[icanvas]->Divide(4,4);
  }

  TCanvas *cplot = new TCanvas("cplot", "Light Yield for MIP muons");
  TCanvas *ceff = new TCanvas("ceff","Efficiency with a Threshold");

  TString rootfilename = filename + ".root";
  TFile *rootfile = new TFile(rootfilename);
  TTree *tree = (TTree*)rootfile->Get("tree");

  ifstream map("./channel.dat");
  char buf[256];
  map.getline(buf, 100);
  int module[maxch], ch_mod[maxch];
  int tmp;


  TH1F *h1 = new TH1F("h1", "Light Yield for Cosmic Muons (MIP);# of p.e.;Entries",15,5,20);


  TH1F *hist[maxch];
  TF1 *landau[maxch];
  TF1 *l1 = new TF1("landau", "landau");

  for(Int_t index = 0; index < 250; index++){
    map>>tmp>>module[index]>>ch_mod[index]>>tmp>>tmp>>tmp;
    //cout << module[index] << ch_mod[index]<< endl;
    if(module[index]!=2) continue;
    //cout << "fit"<<endl;
    c[ch_mod[index]/16]->cd(ch_mod[index]%16+1);
    gPad->SetLogy(1);
    hist[index] = new TH1F(Form("hist%d",index), Form("hist%d",index),130,2,40);
    tree->Draw(Form("PE[%d]>>hist%d",index,index),"","");
    landau[index] = new TF1(Form("landau%d",index), "landau",3,30);
    hist[index]->Fit(landau[index],"Q","",3,30);
    
    Double_t mpv = landau[index]->GetParameter(1);
    if(mpv>5) h1->Fill(mpv);
  }

  cplot->cd();
  h1->Draw();
  cout << h1->GetMean() << endl;

}
