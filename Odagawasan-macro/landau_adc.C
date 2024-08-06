
void landau_adc(TString filename = "../merge/All_1573054729_conv"){

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

  //ofstream gainfile("./gain_1573054729_X2.txt");
  ofstream gainfile("./gain_1578809458_X2.txt");

  TH1F *h1 = new TH1F("h1", "Light Yield for Cosmic Muons (MIP);# of p.e.;Entries",60,900,1500);

  TH1F *histpeak[maxch];
  TH1F *hist[maxch];
  TF1 *landau[maxch];
  TF1 *l1 = new TF1("landau", "landau");

  TSpectrum *s = new TSpectrum();
  Double_t *xpeaks;
  Double_t pedestal[maxch], gain[maxch];
  for(Int_t index = 0; index < 250; index++){
    map>>tmp>>module[index]>>ch_mod[index]>>tmp>>tmp>>tmp;
    //cout << module[index] << ch_mod[index]<< endl;
    if(module[index]!=3) continue;
    //cout << "fit"<<endl;
    c[ch_mod[index]/16]->cd(ch_mod[index]%16+1);
    gPad->SetLogy(1);
    histpeak[index] = new TH1F(Form("histpeak%d",index), Form("histpeak%d",index),1024,0,4096);
    tree->Draw(Form("ADC[%d]>>histpeak%d",index,index),"","");
    s->Search(histpeak[index]);
    xpeaks = s->GetPositionX();
    pedestal[index] = xpeaks[0];

    hist[index] = new TH1F(Form("hist%d",index), Form("hist%d",index),500,pedestal[index]+80,pedestal[index]+2150);
    tree->Draw(Form("ADC[%d]>>hist%d",index,index),"","");
    landau[index] = new TF1(Form("landau%d",index), "landau",pedestal[index]+100,2700);
    hist[index]->Fit(landau[index],"Q","",pedestal[index]+100,2700);
    
    Double_t mpv = landau[index]->GetParameter(1);
    h1->Fill(mpv);
    //gain[index] = (mpv-pedestal[index])/12.33;//run a-1
    gain[index] = (mpv-pedestal[index])/12.31;
    cout << pedestal[index] << " " << mpv << " " << gain[index] << endl;
    gainfile << index << " " << pedestal[index] << " " << gain[index] << endl;

  }

  cplot->cd();
  h1->Draw();
  cout << h1->GetMean() << endl;

  gainfile.close();
}
