
//fit histgram with double gaussian
//for auto fit in DAQ program
//hosomi 121208

const char* safename(const char* name){
    TObject* old = gROOT -> FindObject(name);
    if(old) delete old;
    return name;
}

Double_t mygauss(Double_t *x, Double_t *p){
    return p[0]*exp(-(((x[0]-p[1])/p[2])**2)/2)/(sqrt(2*TMath::Pi())*p[2])
    +p[3]*exp(-(((x[0]-p[1]-p[4])/p[5])**2)/2)/(sqrt(2*TMath::Pi())*p[5]);
}

Double_t gauss(Double_t *x, Double_t *p){
    return p[0]*exp(-(((x[0]-p[1])/p[2])**2)/2)/(sqrt(2*TMath::Pi())*p[2]);
}

void fit1(const char* filename="Deneb_0000_0005", const double Vbias=53.94, const int chnum=0){

    gStyle->SetOptFit();
    gStyle->SetTitleSize(0.05,"xy");
    gStyle->SetLabelSize(0.05,"xy");
    gStyle->SetLabelOffset(0.02,"x");
    gStyle->SetNdivisions(505,"xy");
    gStyle->SetTitleOffset(1.1,"x");
    gStyle->SetTitleOffset(1.3,"y");
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetHistLineWidth(2);
    gStyle->SetHistLineColor(kBlue);
    gStyle->SetFuncWidth(3);
    gStyle->SetFuncColor(kRed);

    const int begin=700;
    const int end=1000;
    const int WOLED=10;

    TCanvas* c1=new TCanvas(safename("c1"),"c1");
    const char* image_gif = Form("image_adc_gif/%s_ch%02d.gif",filename,chnum);
    const char* image_eps = Form("image_adc_eps/%s_ch%02d.eps",filename,chnum);
    TFile* file1=new TFile(Form("rootfile/%s.root",filename));
    TH1F* h1=(TH1F*) file1->Get(safename(Form("adc_high_%d",chnum)));
    h1->SetTitle(Form("ch%d Vbias=%.2f V",chnum,Vbias));
    h1->GetXaxis()->SetRangeUser(begin,end);
    h1->SetXTitle("ADC counts");
    h1->SetYTitle("Counts");
    int NEvents=h1->GetEntries();
    h1->Draw();

    TSpectrum* sp1=new TSpectrum(10);
    sp1->Search(h1);
    int NPeaks=sp1->GetNPeaks();
    cout << "NPeaks is " << NPeaks << endl;
    if(NPeaks<2){
        cerr << "Peak search fault." << endl;
        TText* t1 = new TText(.2, .2, "");
        t1->SetTextColor(kRed);
        t1->SetTextSize(.1);
        t1->DrawTextNDC(.2, .2, "Peak seach fault.");
        c1->Print(image_gif);
        c1->Print(image_eps);
        return;
    }

    //else if(NPeaks>10){
    //  cerr << "Too many peaks." << endl;
    //  return;
    //}


    Float_t* px=sp1->GetPositionX();
    Float_t* py=sp1->GetPositionY();
    Float_t mx0=end;
    Float_t mx1=end;
    Float_t my0, my1;
    int mi0, mi1;

    ///// sort: extract two minimum x point /////
    cout << "\n***** sort *****" << endl;
    for(int i=0; i<NPeaks; i++){
        cout << "(x,y)[" << i << "]=(" << px[i] << "," << py[i] << ")\n";
        if(px[i]<mx1){
            mx1=px[i];
            my1=py[i];
            mi1=i;
        }
        if(px[i]<mx0){
            mx1=mx0;
            my1=my0;
            mi1=mi0;
            mx0=px[i];
            my0=py[i];
            mi0=i;
        }
    }

    cout << "(mx0,my0,mi0)=(" << mx0 << "," << my0 << "," << mi0 << ")\n";
    cout << "(mx1,my1,mi1)=(" << mx1 << "," << my1 << "," << mi1 << ")\n";

    TF1* func1=new TF1("func1",mygauss,begin,end,6);
    func1->SetNpx(1000);
    func1->SetParameters(my0*10.0,mx0,4,my1*10.0,mx1-mx0,4);
    func1->SetParNames("area0","mean0","sigma0","area1","gain","sigma1");

    func1->SetParLimits(0,my0*5.0,my0*20.0);
    func1->SetParLimits(1,mx0-8.0,mx0+8.0);
    func1->SetParLimits(2,2,12);
    func1->SetParLimits(3,my1*5.0,my1*20.0);
    func1->SetParLimits(4,mx1-mx0-8.0,mx1-mx0+8.0);
    func1->SetParLimits(5,2,12);

    cout << "\n***** fitting with LED *****" << endl;
    h1->Fit("func1","","",1.5*mx0-0.5*mx1,-0.5*mx0+1.5*mx1);
    Double_t* par=func1->GetParameters();
    Double_t* err=func1->GetParErrors();
    //const Double_t pedestal = h1->Integral(par[1]-begin-0.5*par[4],par[1]-begin+0.5*par[4]);
    const Double_t pedestal = h1->Integral(par[1]-0.5*par[4],par[1]+0.5*par[4]);
    const Double_t one_pe = h1->Integral(par[1]+0.5*par[4],par[1]+1.5*par[4]);

    ofstream ofs1(Form("para/%s_ch%02d.txt",filename,chnum));
    ofs1 << "#bias\tnevents\tarea0\terr\tmean0\terr\tsigma0\terr\t";
    ofs1 << "area1\terr\tgain\terr\tsigma1\terr\tpedestal\t1 p.e.\n";
    ofs1 << Vbias << "\t" << NEvents << "\t";
    for(int i=0; i<6; i++) ofs1 << par[i] << "\t" << err[i] << "\t";
    ofs1 << pedestal << "\t" << one_pe << endl;
    ofs1.close();

    func1->Draw("same");
    c1->Print(image_gif);
    c1->Print(image_eps);

    return;
}
