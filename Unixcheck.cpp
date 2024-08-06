void Unixcheck(){

    TFile *file = new TFile("../merge/All_1618018135_conv.root");

    TTree *tree = (TTree*)file->Get("tree");
    Int_t N = tree->GetEntries();
    Int_t Nch = 250;
    Int_t tmp = 0;
    Int_t LT[250],TT[250];
    UInt_t UT[250];



    //unsigned int trailtime[Nch],UT[Nch];

    tree->SetBranchAddress("LEADTIME",LT);
    tree->SetBranchAddress("TRAILTIME",TT);
    tree->SetBranchAddress("UNIXTIME", UT);


    for(Int_t event=0; event<N; event++){
        tree->GetEntry(event);

        for(Int_t ch=0;ch<Nch;ch++){
          if(LT[ch]>0){
            cout << "event：" << event << "　leadtime[" << ch << "]：" << LT[ch] << endl;
          }
        }
    }

   cout << "complete" << endl;

}
