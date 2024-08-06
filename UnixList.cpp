void UnixList(){
    TFile *file0 = new TFile("../merge/All_1618018135_conv.root");
    TTree *tree = (TTree*)file0->Get("tree");
    UInt_t DAQ_UT[250];
    tree->SetBranchAddress("UNIXTIME",DAQ_UT);
    tree->SetBranchStatus("LEADTIME",1);
    tree->SetBranchStatus("TRAILTIME",1);
    tree->SetBranchStatus("PE",1);
    tree->SetBranchStatus("ADC", 1);
    tree->SetBranchStatus("VIEW", 1);
    tree->SetBranchStatus("PLN", 1);
    tree->SetBranchStatus("CH", 1);

    TFile *file1 = new TFile("../merge/merge_MR86b.root");
    TTree *bsd = (TTree*)file1->Get("bsd");
    Int_t BSD_UT,flag;
    bsd->SetBranchAddress("trg_sec", &BSD_UT);
    bsd->SetBranchAddress("spill_flag", &flag);
    int N = bsd->GetEntries();

    bsd->GetEntry(0);
    int UT_0 =  BSD_UT;
    cout << UT_0 << endl;
    bsd->GetEntry(N-1);
    int UT_N = BSD_UT;
    cout << UT_N << " " << UT_0 << endl;
    bsd->Draw(">>elist","spill_flag>0");
    TEventList *elist = (TEventList*)gROOT->FindObject("elist");
    int bsdN = elist->GetN();
    bsd->GetEntry(elist->GetEntry(0));
    int start_UT =BSD_UT;
    cout << start_UT << endl;
    bsd->GetEntry(elist->GetEntry(bsdN-1));
    int end_UT =BSD_UT;
    TH1D *h = new TH1D("h", "h ",UT_N-UT_0, UT_0,UT_N);
    cout << UT_N << " " << UT_0 << endl;

    int BSDUT_x[N],BSDUT_y[N],BSDcount=0;

    for (Int_t ibsd=0; ibsd<N; ibsd++) {

        //Int_t eve = elist->GetEntry(ibsd);
        bsd->GetEntry(ibsd);
        //if (flag>0){
        //cout<<flag<<endl;

        h->Fill(BSD_UT,1);
        /*
        BSDcount++;
        BSDUT_x[ibsd]=BSD_UT;
        BSDUT_y[ibsd]=BSDcount;
         */
        //cout<<BSDcount<<endl;
         /*
        if (BSDUT_x[ibsd]-BSDUT_x[ibsd-1]<0) {
            cout<<BSDUT_x[ibsd]-BSDUT_x[ibsd-1]<<endl;
        }
         */
        //}
    }

    tree->Draw(">>elist2",Form("UNIXTIME[%d]<%d&&UNIXTIME[%d]>%d",0,UT_N+10,0,UT_0-10));
    TEventList *elist2 = (TEventList*)gROOT->FindObject("elist2");
    Int_t daqN = elist2->GetN();
    cout << daqN << endl;
    int DAQUT_x[N],DAQUT_y[N],DAQcount=0;

    for (Int_t idaq=0; idaq<daqN; idaq++) {

        Int_t eve = elist2->GetEntry(idaq);
        tree->GetEntry(idaq);
        //cout<<h->GetBinContent(idaq)<<endl;
        h->Fill(DAQ_UT[0]);
        h->Fill(DAQ_UT[0]);
        /*
        int tmp=h->GetBinContent(DAQ_UT[0]-start_UT+1000);

        if(h->GetBinContent(tmp)==3){
            DAQcount++;
            DAQUT_x[idaq]=DAQ_UT[0];
            DAQUT_y[idaq]=DAQcount;
        }
         */
    }

    for (Int_t ibin=0; ibin<N; ibin++) {

        int tmp=h->GetBinContent(ibin);
        int UT = UT_0+ibin;
        DAQUT_x[ibin]=UT;
        DAQUT_y[ibin]=DAQcount;

        BSDUT_x[ibin]=UT;
        BSDUT_y[ibin]=BSDcount;
        if(tmp==3){
            //cout<<tmp<<endl;
            DAQcount++;
            DAQUT_x[ibin]=UT;
            DAQUT_y[ibin]=DAQcount;
            BSDcount++;
            BSDUT_x[ibin]=UT;
            BSDUT_y[ibin]=BSDcount;
        }

        if(tmp==1){
            //cout<<tmp<<endl;
            BSDcount++;
            BSDUT_x[ibin]=UT;
            BSDUT_y[ibin]=BSDcount;
        }
        //cout<<BSDcount<<endl;
        //cout<<BSDUT_x[ibin]<<endl;
    }


    TCanvas *c = new TCanvas( "test" );
    TH1 *frame = c->DrawFrame(UT_0, 0,UT_N+1000, BSDcount+1000 );
    TGraph *g = new TGraph(N,BSDUT_x, BSDUT_y );
    gStyle->SetTimeOffset(-788918400);
    g->GetXaxis()->SetTimeDisplay(1);
    g->GetXaxis()->SetTimeFormat("%m/%d");
    g->GetXaxis()->SetTimeOffset(0, "gmt");
    //g->SetMarkerStyle( 20 );
    //g->SetMarkerSize( 0.5 );

    g->Draw("AP");
    TGraph *gg = new TGraph(N,DAQUT_x, DAQUT_y );

    gg->SetMarkerStyle( 20 );
    gg->SetMarkerSize( 5 );
    gg->SetMarkerColor( 6 );
    gg->Draw("same");
    g->GetXaxis()->SetTitle("month/day");
    g->GetYaxis()->SetTitle("#Event");
    g->SetTitle("#Event");
/*
    gStyle->SetTimeOffset(-788918400);
    h->GetXaxis()->SetTimeDisplay(1);
    h->GetXaxis()->SetTimeFormat("%m/%d");
    h->GetXaxis()->SetTimeOffset(0, "gmt");
    h->Draw();

     */
}
