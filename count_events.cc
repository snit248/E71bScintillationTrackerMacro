//あるEvent numberにおけるEvent Display
//チャンネルごとの光量を反映

#define DWBUG

void count_events(){ //Entry numberを指定

  const Int_t nindex = 250;

  const Float_t PE_threshold = 2.5;
  const Float_t ADC_threshold = 900; 
  const Float_t ToT_threshold = 40; 

  const Int_t offsety[4]  = {20,  4, 16,  0};
  const Int_t offsetx[4]  = { 0, 16,  4, 20};
  const Int_t offsetz[4]  = { 1,  7, 10, 16};


  ////読み込むファイルの指定////
  TString dirname="../merge/";
  TString mergefilename = dirname+"All_1700735651_1701478713_conv.root";
  //All_1700735651_1701239921_conv.root
  //All_1700735651_1701478713_conv.root
  //ninja_rawdata_2021_4_15.root
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

  mergetree->GetEntries();

  ////channel.datの読み込み////
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

  Int_t counter=0;

  ///MatchしたEntry numberのEventDisplay////
  for(Int_t ientry=0; ientry<mergetree->GetEntries(); ientry++){
    mergetree->GetEntry(ientry);
    Int_t countOverThreshold = 0;
        for(Int_t index = 0; index < nindex; index++) {
            //if(leadtime[index]-trailtime[index] > ToT_threshold && unixtime[index] > 1701075572) {
            //if(pe[index] > PE_threshold && unixtime[index] > 1701478689) {
            if(pe[index] > PE_threshold) {
                countOverThreshold++;
            }
        }

        //カウントする条件
        if(countOverThreshold >= 2 && 1701445800 < unixtime[0] && unixtime[0] < 1701476100) {
          counter++;
        }
    }
    std::cout<<counter<<std::endl;
}
