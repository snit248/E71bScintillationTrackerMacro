void TdcCheck(){
    TFile *file0 = new TFile("../merge/All_1618018135_conv.root");

    TTree *tree = (TTree*)file0->Get("tree");
    Int_t N = tree->GetEntries();
    Int_t Nch=250,maxch,emiteve[N],tmp=0;
    Int_t adc[Nch];
    Int_t LT[250],TT[250];
    UInt_t UT[250];
    Float_t pe[250];


    //unsigned int trailtime[Nch],UT[Nch];

    tree->SetBranchAddress("LEADTIME",LT);
    tree->SetBranchAddress("TRAILTIME",TT);
    tree->SetBranchAddress("UNIXTIME", UT);
    tree->SetBranchAddress("ADC", adc);
    tree->SetBranchAddress("PE", pe);

    TH2F *h = new TH2F("name","title; X; Y", 250, 0, 250, 4600, -100, 4500);
    //TH2F *h = new TH2F("name","title; ch; pe", 250, 0, 250, 10, 0, 100);

    for (Int_t eve =0; eve<N; eve++){
        tree->GetEntry(eve);

        for(Int_t ch = 0;ch<Nch;ch++){
          //if(pe[ch]>2.5){
           // if(TT[ch]>0){
            //h->Fill(ch,pe[ch]);
            h->Fill(ch,TT[ch]);
          }
        //}
        }
    
    h->SetStats(0);
    h->Draw();

}
