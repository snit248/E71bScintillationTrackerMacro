void CheckGain() {

  TString ifilename = "/Users/odagawa/FY2020/NINJA/tracker_data/merge/All_1573054729_conv.root";
  TFile *ifile = new TFile(filename, "read");
  TTree *itree = (TTree*)iflle->Get("tree");

  Int_t adc[250], leadtime[250], trailtime[250];
  Int_t unixtime[250];
  Float_t pe[250];

  itree->SetBranchAddress("ADC", adc);
  itree->SetBranchAddress("LEADTIME", leadtime);
  itree->SetBranchAddress("TRAILTIME", trailtime);
  itree->SetBranchAddress("UNIXTIME", unixtime);
  itree->SetBranchAddress("PE", pe);

  // Channel list file
  Char_t buffer[256];
  TString mapname = "../src/channel.dat";
  ifstream map(mapname);
  map.getline(buffername, 100);
  Int_t tmp = 0;
  Int_t module[250], ch_mod[250];
  Int_t view[250], plane[250], channel[250];

  for ( Int_t index = 0; index < 250; index++ ) {
    map >> tmp >> module[index] >> ch_mod[index]
	>> view[index] >> plane[index] >> channel[index];
  }

  // Gain file
  TString gainname = "../src/gain_1573054729.dat";
  ifstream gainfile(gainname);
  Double_t pedestal[250], gain[250];
  for ( Int_t index = 0; index < 250; index++ ) {
    gainfile >> tmp >> pedestal[index] >> gain[index];
  }

  // Histogram
  TH1D *hist[250];
  for ( Int_t index = 0; index < 250; index++ ) {
    hist[index] = new TH1D(Form("hist%d", index),
			   Form("p.e. (view : %d, plane : %d, slot : %d)",
				view[index], plane[index], channel[index]),
			   100, 0, 100);
  }

  for ( Int_t ientry  = 0; ientry < itree->GetEntries(); ientry++ ) {
    itree->GetEntry(ientry);
    for ( Int_t index = 0; index < 250; index++ ) {
      if ( pe[index] < 2.5 ) continue;
      hist[index]->Fill(pe[index]);
    }
  }

  for ( Int_t index = 0; index < 250; index++ ) {
    Double_t mean = hist[index]->GetMean();
  }
  
}
