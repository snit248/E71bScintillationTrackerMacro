void TOTFit(){

  const Int_t maxch = 250;
  const Int_t nmodule = 4;
  const Int_t ncanvas = 4;

  //canvases
  TCanvas *c[nmodule][ncanvas];
  const TString modulename[nmodule] = {"Y1",  "Y2",  "X1",  "X2"};
  const TString order[ncanvas] = {"1st", "2nd", "3rd", "4th"};
  for(Int_t imodule=0; imodule<nmodule; imodule++){
    for(Int_t icanvas=0; icanvas<ncanvas; icanvas++){
      c[imodule][icanvas] = new TCanvas(Form("c%d%d",imodule,icanvas), modulename[imodule]+" "+order[icanvas]+" 16 channels", 1200, 800);
      c[imodule][icanvas]->Divide(4, 4);
    }
  }

  TCanvas *ccut = new TCanvas("ccut","ToT corresponding to 0 p.e.");
  TCanvas *cthr = new TCanvas("cthr", "ToT corresponding to 2.5 p.e.");
  
 
  //input file (PRA-2)
  TString dirname = "../merge/";
  TString filename = "All_1578809458_conv.root";
  TFile *file = new TFile(dirname+filename);
  TTree *tree = (TTree*)file->Get("tree");

  //channel map file
  ifstream map("./channel.dat");
  char buf[256];
  map.getline(buf, 100);
  int module[maxch], ch_mod[maxch];
  int tmp;

  //output file
  ofstream thrfile("./threshold.txt");
  TString pdfname = "./threshold.pdf";
  
  //Fit function
  TF1 *func = new TF1("func","sqrt([2]/[0])*log(x-[1])+sqrt([0]*[2])",-.5,25);
  func->SetParameter(0, 1);
  func->SetParameter(1, 2);
  func->SetParameter(2, 200);

  //Histogram
  TH2F *hist[maxch];
  TH1F *hcut = new TH1F("hcut","P.E. corresponding to 0 ToT;P.E.;Entries",15,1.5,3.0);
  TH1F *hthr = new TH1F("hthr","ToT corresponding to 2.5 P.E.;ToT [um];Entries",20,10,30);
  for(int ich=0; ich<maxch; ich++){
    //for(int ich=0; ich<2; ich++){
    //cout << "\rich: " << ich << " ";
    cout << "ich:" << ich << endl;

    map >> tmp >> module[ich] >> ch_mod[ich] >> tmp >> tmp >> tmp;
    if(module[ich]==-1) continue;
#ifdef DEBUG
    cout << endl << module[ich] << " " << ch_mod[ich] << endl;
#endif

    c[module[ich]][ch_mod[ich]/16]->cd(ch_mod[ich]%16+1);
    hist[ich] = new TH2F(Form("hist%d",ich),Form("CH%d;P.E.;Time Over Threshold [us]",ich),100,2,22,19,5,100);
    hist[ich]->SetStats(0);

    tree->Draw(Form("LEADTIME[%d]-TRAILTIME[%d]:PE[%d]>>hist%d",ich,ich,ich,ich),"","colz");
    hist[ich]->Fit(func,"Q","",3,15);
    
    float p0 = func->GetParameter(0), p1 = func->GetParameter(1), p2 = func->GetParameter(2);
    float e0 = func->GetParError(0), e1 = func->GetParError(1), e2 = func->GetParError(2);
    float value0 = exp(-p0)+p1;
    float value25 = sqrt(p2/p0)*log(2.5-p1)+sqrt(p0*p2);
    float err0 = sqrt(exp(-2*p0)*e0*e0+e1*e1);
#ifdef DEBUG
    cout << "When ToT = 0, P.E. Value: " << value0 << " +/- " << err0 << endl;
    cout << "When P.E. = 2.5, ToT Value: " << value25 << endl;
#endif

    hcut->Fill(value0);
    hthr->Fill(value25);
    thrfile << ich << " " << value25 << endl;
    
  }

  cout << endl;

  thrfile.close();

  for(Int_t imodule=0; imodule<nmodule; imodule++){
    for(Int_t icanvas=0; icanvas<ncanvas; icanvas++){
      if(imodule==0&&icanvas==0) c[imodule][icanvas]->Print(pdfname+"[","pdf");
      c[imodule][icanvas]->Print(pdfname,"pdf");
      delete c[imodule][icanvas];
    }
  }

  ccut->cd();
  hcut->Draw();
  ccut->Print(pdfname,"pdf");
  delete ccut;
  cthr->cd();
  hthr->Draw();
  cthr->Print(pdfname,"pdf");
  cthr->Print(pdfname+"]","pdf");
  delete cthr;
 
  return;
  
}
