#define DWBUG

void ED_match() { // Entry numberを指定

  const Int_t nindex = 250;
  const Float_t PE_threshold = 2.5;
  const Int_t offsety[4] = {20, 4, 16, 0};
  const Int_t offsetx[4] = {0, 16, 4, 20};
  const Int_t offsetz[4] = {1, 7, 10, 16};
  std::string msg;

  // 読み込むファイルの指定
  TString mergefilename = "../E71b_ALL_ST/E71b_ST_data.root";

  // MultiHitRate.datの読み込み
  const Int_t nline = 10897; // 行数
  std::ifstream fin1("../Analysis/MultiHitRate_x2y2.dat");
  if(!fin1){
    cout << "Cannot open the file."<<std::endl;
  }

  Int_t tmp1;
  Int_t trkentry[nline];

  for (Int_t iline = 0; iline < nline; iline++) {
    fin1 >> trkentry[iline];
    fin1.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //cout << tmp1 << endl;
  }
  fin1.close();

  cout << "Reading：" << mergefilename << endl;
  TFile *mergefile = new TFile(mergefilename);
  TTree *mergetree = (TTree *)mergefile->Get("tree");

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

  mergetree->GetEntries();

  // channel.datの読み込み
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
  for (Int_t index = 0; index < nindex; index++) {
    map >> tmp >> module[index] >> ch_mod[index] >> view[index] >> pln[index] >> ch[index];
  }

  // MatchしたEntry numberのEventDisplay
  for (Int_t ientry = 0; ientry < mergetree->GetEntries(); ientry++) {
    // MultiHitRate.dataから取得したインデックスと一致するエントリのみ表示
    bool match = false;
    for (Int_t iline = 0; iline < nline; iline++) {
      if (ientry == trkentry[iline]) {
        match = true;
        break;
      }
    }
    if (!match) continue;

    mergetree->GetEntry(ientry);

    // 以前に作成されたキャンバスとヒストグラムを削除
    if (gROOT->FindObject("c")) delete gROOT->FindObject("c");
    if (gROOT->FindObject("sidehist")) delete gROOT->FindObject("sidehist");
    if (gROOT->FindObject("tophist")) delete gROOT->FindObject("tophist");

    // キャンバスの準備
    TCanvas *c = new TCanvas("c", "c", 750, 1000);
    c->Divide(2, 1);
    c->cd(1)->DrawFrame(0, -500, 20, 500, "Event Display (Sideview);z [mm];y [mm]");
    c->cd(2)->DrawFrame(20, -500, 40, 500, "Event Display (Topview);z [mm];x [mm]");

    // チャンネルごとの光量を反映させるための2次元ヒストグラム
    TH2F *sidehist = new TH2F("sidehist", "", 20, 0, 20, 250, -500, 500);
    sidehist->SetMinimum(0);
    sidehist->SetMaximum(20);
    TH2F *tophist = new TH2F("tophist", "", 20, 20, 40, 250, -500, 500);
    tophist->SetMinimum(0);
    tophist->SetMaximum(20);

    TBox *scint = new TBox();

    cout << "Entry number：" << ientry << endl;
    cout << "-----Unixtime of each module-----" << endl;

    Int_t tdcflag = 0;

    for (Int_t index = 0; index < 250; index++) {

      // 各モジュールのunixtimeが一致しているかを確認
      if (index == 0 || index == 16 || index == 141 || index == 156) cout << "Module No：" << module[index] << "：" << unixtime[index] << endl;

      // 光量がthresholdを超えているか
      if (pe[index] < PE_threshold) continue; // thresholdより小さかったらループを抜ける

      tdcflag = 1;
      // viewは0がXのチャンネル、1がYのチャンネル
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

    // チャンネルに色を付けていく
    if (tdcflag) {
      for (Int_t index = 0; index < 250; index++) {

        if (index == 89 || index == 122) scint->SetFillColor(kBlack);
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
    gSystem->ProcessEvents();
    std::cout << "Input q to quit." << std::endl;
    getline(std::cin, msg);
    if (msg == "q") {
      break;
    }
  }
}