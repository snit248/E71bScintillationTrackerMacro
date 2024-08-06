#include <TCanvas.h>
#include <TH2F.h>
#include <TBox.h>
#include <TFile.h>
#include <TTree.h>

void EventDisplayAll() {
  const Int_t nindex = 250;
  const Float_t PE_threshold = 2.5;
  const Int_t offsety[4]  = {20,  4, 16,  0};
  const Int_t offsetx[4]  = { 0, 16,  4, 20};
  const Int_t offsetz[4]  = { 1,  7, 10, 16};

  TString dirname = "../merge/";
  TString mergefilename = dirname + "All_1702446582_conv.root";
  cout << "Reading: " << mergefilename << endl;
  TFile *mergefile = new TFile(mergefilename);
  TTree *mergetree = (TTree*)mergefile->Get("tree");

  Int_t adc[nindex], unixtime[nindex], leadtime[nindex], trailtime[nindex];
  Float_t pe[nindex];
  mergetree->SetBranchAddress("ADC", adc);
  mergetree->SetBranchAddress("UNIXTIME", unixtime);
  mergetree->SetBranchAddress("LEADTIME", leadtime);
  mergetree->SetBranchAddress("TRAILTIME", trailtime);
  mergetree->SetBranchAddress("PE", pe);

  Char_t buffer[256];
  TString mapname = "../src/channel_new.dat";
  std::ifstream map(mapname);
  if (!map.is_open()) {
    std::cerr << "Failed to open the file." << std::endl;
    return;
  }

  map.getline(buffer, 100);
  Int_t tmp, module[nindex], ch_mod[nindex], view[nindex], pln[nindex], ch[nindex];
  for (Int_t index = 0; index < nindex; index++) {
    map >> tmp >> module[index] >> ch_mod[index] >> view[index] >> pln[index] >> ch[index];
  }

  // Event display loop
  for (Int_t ientry = 0; ientry < mergetree->GetEntries(); ientry++) {
    mergetree->GetEntry(ientry);
    Int_t countOverThreshold = 0;
    for (Int_t index = 0; index < nindex; index++) {
      if (pe[index] > PE_threshold && unixtime[index] > 1701939967) {
        countOverThreshold++;
      }
    }

    if (countOverThreshold >= 3) {
      // Create and draw the event display
      TCanvas *c = new TCanvas("c", "c", 750, 1000);
      c->Divide(2, 1);
      c->cd(1)->DrawFrame(0, -500, 20, 500, "Event Display (Sideview);z [mm];y [mm]");
      c->cd(2)->DrawFrame(20, -500, 40, 500, "Event Display (Topview);z [mm];x [mm]");

      // ヒストグラムを作成する前に以前のものを削除
        if (gROOT->FindObject("sidehist")) delete gROOT->FindObject("sidehist");
        if (gROOT->FindObject("tophist")) delete gROOT->FindObject("tophist");


      TH2F *sidehist = new TH2F("sidehist", "", 20, 0, 20, 250, -500, 500);
      sidehist->SetMinimum(0);
      sidehist->SetMaximum(20);
      TH2F *tophist = new TH2F("tophist", "", 20, 20, 40, 250, -500, 500);
      tophist->SetMinimum(0);
      tophist->SetMaximum(20);

      TBox *scint = new TBox();
      cout << "Entry number: " << ientry << endl;
      Int_t tdcflag = 0;

      // Process event data
      // ... [イベントデータ処理のコード] ...

      // Fill and draw histograms
      // ... [ヒストグラムを埋めるためのコード] ...
      c->cd(1);
      sidehist->Draw("same colz");
      c->cd(2);
      tophist->Draw("same colz");

      // Wait for the user to close the canvas
      c->WaitPrimitive();

      // Close the canvas and clean up
      c->Close();
      delete c;
    }
  }
}
