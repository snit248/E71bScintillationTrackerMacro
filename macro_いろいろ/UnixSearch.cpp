void UnixSearch(){
    TFile *file0 = new TFile("../merge/All_1618018135_conv.root");

    TTree *tree = (TTree*)file0->Get("tree");
    Int_t N = tree->GetEntries();
    Int_t Nch=250,maxch,emiteve[N],tmp=0;
    Int_t adc[Nch];
    Int_t LT[250],TT[250];
    UInt_t UT[250];


    //unsigned int trailtime[Nch],UT[Nch];

    tree->SetBranchAddress("LEADTIME",LT);
    tree->SetBranchAddress("TRAILTIME",TT);
    tree->SetBranchAddress("UNIXTIME", UT);
    tree->SetBranchAddress("ADC", adc);


    TH1D *h[N];

    for(Int_t i=0; i<N; i++){

        h[i] = new TH1D(Form("h%d",i), Form("h%d",i), 4500, 0,4500);

    }

    for (Int_t eve=0; eve<N; eve++){
        tree->GetEntry(eve);
        //cout<<" "<<LT[0]<<endl;
        //cout<<UT[0]<<endl;
        for(Int_t ch=0; ch<Nch; ch++){
            if(LT[ch]>0&&abs(LT[ch]-TT[ch])>200&&abs(LT[ch]-TT[ch])<4500){
                //if(LT[ch]-TT[ch]>200){
                //cout<<" "<<LT[ch]<<endl;

                    for(Int_t Time=min(LT[ch],TT[ch]); Time<max(LT[ch],TT[ch]); Time++){
                    //cout<<TT[ch]<<" "<<LT[ch]<<endl;

                        h[eve]->Fill(Time);
                    }
               // }
            }
        }
        maxch=h[eve]->GetMaximum();
        if(maxch>3){
            //cout<<eve<<" "<<maxch<<endl;
            emiteve[tmp] = eve;
            tmp++;
        }
    }

    for(Int_t j=0 ; j<tmp; j++){
        tree->GetEntry(emiteve[j]);
        for(Int_t ch=0; ch<Nch; ch++){
            if(LT[ch]>0&&abs(LT[ch]-TT[ch])>200&&abs(LT[ch]-TT[ch])<4500){
                cout << ch << ", ";
            }
        }
        cout << endl;

    }
    TCanvas *c = new TCanvas("c","c" , 600, 800);

    //gPad->SetLogy();
    h[0]->Draw();

}
