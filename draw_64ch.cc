void draw_64ch(char* data="../data/1120tu19Y1BV57.root"){
  //../data/1120tu19Y1BV57.root
  //../data/1120ku16Y2BV57.root
  //../data/1120ynu25X1BV57.root
  //../data/1120ynu26X2BV57.root

  TFile* f = new TFile(data,"read");

  TCanvas* c1 = new TCanvas("c1", "c1", 600,800);
  c1->Divide(8,8);
  
  char name[64];
  int i;
  for(i=0; i<64; i++){
    c1->cd(i+1);
    sprintf(name,"ADC_HIGH_%d",i);
    TH1D *h = (TH1D*)gDirectory->Get(name);
    h->GetXaxis()->SetRangeUser(500,3500);
    h->Draw();
    gPad->Update();
    gPad->SetLogy();
  }
  
}
