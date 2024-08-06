#define DWBUG

void EventDisplay_peAuto() { // Entry numberを指定

  const Int_t nindex = 250;

  const Float_t PE_threshold = 2.5;
  const Float_t ADC_threshold = 900; 
  const Float_t ToT_threshold = 40; 

  const Int_t offsety[4]  = {20,  4, 16,  0};
  const Int_t offsetx[4]  = { 0, 16,  4, 20};
  const Int_t offsetz[4]  = { 1,  7, 10, 16};

  TString mergefilename = "../E71b_ALL_ST/E71b_ST_data.root";

  cout << "Reading：" << mergefilename << endl;
  TFile *mergefile = new TFile(mergefilename);
  TTree *mergetree = (TTree*)mergefile->Get("tree");

  Int_t adc[nindex];
  Int_t unixtime[nindex];
  Int_t leadtime[nindex];
  Int_t trailtime[nindex];
  Float_t pe[nindex];

  mergetree->SetBranchAddress("ADC", adc);
  mergetree->SetBranchAddress("UNIXTIME", unixtime);
  mergetree->SetBranchAddress("LEADTIME", leadtime);
  mergetree->SetBranchAddress("TRAILTIME", trailtime);
  mergetree->SetBranchAddress("PE", pe);

  Int_t nEntries = mergetree->GetEntries();

  Char_t buffer[256];
  TString mapname = "../src/channel_new.dat";
  std::ifstream map(mapname);
  if (!map.is_open()) {
      std::cerr << "Failed to open the file." << std::endl;
      return;
  }
  map.getline(buffer, 100);
  Int_t tmp, module[nindex], ch_mod[nindex];
  Int_t view[nindex], pln[nindex], ch[nindex];
  for(Int_t index = 0; index < nindex; index++) {
      map >> tmp >> module[index] >> ch_mod[index] >> view[index] >> pln[index] >> ch[index];
  }

  TDatime now;
  Int_t currentUnixTime = now.Convert();
  Int_t unixTime24HoursAgo = currentUnixTime - 24 * 3600;

  Int_t counter = 0;

  for (Int_t ientry = 0; ientry < nEntries; ientry++) {
    mergetree->GetEntry(ientry);
    Int_t countOverThreshold = 0;
    for (Int_t index = 0; index < nindex; index++) {
      if (pe[index] > PE_threshold && unixtime[index] > 1708152470) {
        countOverThreshold++;
      }
    }

    if (countOverThreshold >= 3) {
      counter++;
      if (counter == 2) break;

      TCanvas *c = new TCanvas("c", "c", 750, 1000);
      c->Divide(2, 1);
      c->cd(1)->DrawFrame(0 , -500, 20, 500, "Event Display (Sideview);z [mm];y [mm]");
      c->cd(2)->DrawFrame(20, -500, 40, 500, "Event Display (Topview);z [mm];x [mm]");

      TH2F *sidehist = new TH2F("sidehist", "", 20, 0, 20, 250, -500, 500);
      sidehist->SetMinimum(0);
      sidehist->SetMaximum(20);
      TH2F *tophist  = new TH2F("tophist",  "", 20, 20, 40, 250, -500, 500);
      tophist->SetMinimum(0);
      tophist->SetMaximum(20);

      TBox *scint = new TBox();

      cout << "Entry number：" << ientry << endl;
      cout << "-----Unixtime of each module-----" << endl;

      Int_t tdcflag = 0;

      for (Int_t index = 0; index < 250; index++) {
        if (index == 0 || index == 16 || index == 141 || index == 156) cout << "Module No：" << module[index] << "：" << unixtime[index] << endl;
        if (pe[index] < PE_threshold) continue;

        tdcflag = 1;
        if (view[index] == 0) {
          for (Int_t z = 0; z < 3; z++) {
            for (Int_t y = 0; y < 6; y++) {
              sidehist->Fill(offsetz[pln[index]] + 1. * z, -500 + 32 * ch[index] + offsety[pln[index]] + 4. * y, pe[index]);
            }
          }
        } else if (view[index] == 1) {
          for (Int_t z = 0; z < 3; z++) {
            for (Int_t x = 0; x < 6; x++) {
              tophist->Fill(20 + offsetz[pln[index]] + 1. * z, 500 - 32 * ch[index] - offsetx[pln[index]] - 4. * (x + 1), pe[index]);
            }
          }
        }
      }

      if (tdcflag) {
        for (Int_t index = 0; index < 250; index++) {
          scint->SetFillColor(19);
          if (view[index] == 0) {
            c->cd(1);
            scint->DrawBox(offsetz[pln[index]], -500 + 32 * ch[index] + offsety[pln[index]], offsetz[pln[index]] + 3, -500 + 32 * ch[index] + 24 + offsety[pln[index]]);
          } else if (view[index] == 1) {
            c->cd(2);
            scint->DrawBox(offsetz[pln[index]] + 20, 500 - 32 * ch[index] - offsetx[pln[index]], offsetz[pln[index]] + 23, 500 - 32 * ch[index] - 24 - offsetx[pln[index]]);
          }
        }
      } else {
        cerr << "No ADC data and Multi hit TDC data has no meaning..." << endl;
      }
      c->cd(1);
      sidehist->Draw("same colz");
      c->cd(2);
      tophist->Draw("same colz");
      c->Update();

      cout << "Press Enter to proceed to the next event or close the canvas window..." << endl;

      bool canvasClosed = false;
      while (!canvasClosed) {
        gSystem->ProcessEvents();
        if (!gROOT->FindObject("c")) {
          canvasClosed = true;
        }
        else if (kbhit()) {  // Check if a key has been pressed
          getchar();  // Consume the key press
          break;
        }
      }

      delete c; // Clear the canvas to prepare for the next event
    }
  }
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
