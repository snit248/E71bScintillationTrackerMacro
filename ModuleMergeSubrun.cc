#define DEBUG

void ModuleMergeSubrun(){

  const Int_t nmodule = 4;
  const Int_t nch = 64;
  Int_t start_subrun = 6;
  Int_t end_subrun = 11;

  TString rootdirname = "../merge/";
  TString modulename[nmodule] = {"Y1", "Y2", "X1", "X2"};
  //TString runtimename = "1573054729";//run a-1
  //TString runtimename = "1578809458";//run a-2
  //TString runtimename = "1699259775";//kane1
  //TString runtimename = "1700735626";//E71b 1 11/23
  TString runtimename = "1700903564";//E71b 2 11/25
  TString rootextname = ".root";

  TString filename[nmodule];
  TFile *file[nmodule];
  TTree *tree[nmodule];

  Int_t nentry[nmodule];

  Int_t adc[nmodule][nch], leadtime[nmodule][nch], trailtime[nmodule][nch];
  Int_t unixtime[nmodule];

  Char_t buffer[256];
  TString mapname = "../src/channel_new.dat";
  ifstream map(mapname);
  map.getline(buffer, 100);
  Int_t tmp;
  Int_t module[250], ch_mod[250];
  Int_t view[250], pln[250], ch[250];

  for(Int_t index=0; index<250; index++){
    map>>tmp>>module[index]>>ch_mod[index]>>view[index]>>pln[index]>>ch[index];
#ifdef DEBUG
    cout<<module[index]<<" "<<ch_mod[index]<<endl;
#endif
  }

  TString gainname = "../src/gain_"+runtimename+".dat";
  //TString gainname = "../src/gain_1573054729.dat";
  ifstream gainfile(gainname);
  Double_t pedestal[250], gain[250];
  for(Int_t index=0; index<250; index++){
    gainfile>>tmp>>pedestal[index]>>gain[index];
#ifdef DEBUG
    cout<<pedestal[index]<<" "<<gain[index]<<endl;
#endif
  }

  for(Int_t imodule=0; imodule<4; imodule++){
    // ファイル名の生成にサブランの範囲を使用
    filename[imodule] = rootdirname+Form("%d_%d_", start_subrun, end_subrun)+modulename[imodule]+"_"+runtimename+"_conv"+rootextname;
    file[imodule] = new TFile(filename[imodule]);
    tree[imodule] = (TTree*)file[imodule]->Get("tree");

    file[imodule] = new TFile(filename[imodule]);
    tree[imodule] = (TTree*)file[imodule]->Get("tree");
    tree[imodule]->SetBranchAddress("ADC", adc[imodule]);
    tree[imodule]->SetBranchAddress("LEADTIME", leadtime[imodule]);
    tree[imodule]->SetBranchAddress("TRAILTIME", trailtime[imodule]);
    tree[imodule]->SetBranchAddress("UNIXTIME", &unixtime[imodule]);

    nentry[imodule] = tree[imodule]->GetEntries();
#ifdef DEBUG
    cout<<nentry[imodule]<<endl;
#endif
    if(imodule>0&&nentry[imodule]!=nentry[imodule-1]){
      cerr<<"Number of Entries in each module is not same!!"<<endl;
      return;
    }
  }

  //TString mergefilename = rootdirname+"All"+"_"+runtimename+"_convcheck"+rootextname;
  TString mergefilename = rootdirname+Form("%d_%d_", start_subrun, end_subrun)+"All"+"_"+runtimename+"_conv"+rootextname;
  TFile *mergefile = new TFile(mergefilename, "recreate");
  TTree *mergetree = new TTree("tree", "tree");

  Int_t mergeadc[250], mergeleadtime[250], mergetrailtime[250];
  Int_t mergeunixtime[250];
  Float_t mergepe[250];

  mergetree->Branch("ADC", mergeadc, "adc[250]/I");
  mergetree->Branch("LEADTIME", mergeleadtime, "leadtime[250]/I");
  mergetree->Branch("TRAILTIME", mergetrailtime, "trailtime[250]/I");
  mergetree->Branch("UNIXTIME", mergeunixtime, "unixtime[250]/I");
  mergetree->Branch("PE", mergepe, "pe[250]/F");
  mergetree->Branch("VIEW", view, "view[250]/I");
  mergetree->Branch("PLN", pln, "pln[250]/I");
  mergetree->Branch("CH", ch, "ch[250]/I");

  for(Int_t ientry=0; ientry<nentry[0]; ientry++){
    //if(ientry==638355) continue;
      cout<<"DEBUG1"<<endl;
      for(Int_t imodule=0; imodule<nmodule; imodule++) tree[imodule]->GetEntry(ientry);
      cout<<"DEBUG2"<<endl;
      for(Int_t index=0; index<250; index++){
      mergeadc[index]=adc[module[index]][ch_mod[index]];
      mergeleadtime[index]=leadtime[module[index]][ch_mod[index]];
      mergetrailtime[index]=trailtime[module[index]][ch_mod[index]];
      mergeunixtime[index]=unixtime[module[index]];
      mergepe[index]=(mergeadc[index]-pedestal[index])/gain[index];
          cout<<"DEBUG3"<<endl;
      }
    mergetree->Fill();
    if(ientry%100000==0) fprintf(stderr, "\r[%d/%d]", ientry, nentry[0]);
  }
  fprintf(stderr, "\r[%d/%d]\n", nentry[0], nentry[0]);

  mergetree->Write();
  mergefile->Close();
}
