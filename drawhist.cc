//Macro to import histograms, fit first two peaks (pedestal and 1p.e.) and output the means of the peaks to a file

#include "TFile.h"
#include <TH1.h>
#include <string>

using namespace std;
using namespace TMath;

void drawhist(string data = "./data/calib_17101500.dat"){

  TF1 * DGauss = new TF1("DGauss","[0]*Gaus(x,[1],[2])+[3]*Gaus(x,[4],[5])",800,900);

 DGauss->SetParameters(5000,830,20,500,870,20);  //set initial fit parameters

 double normA[64]; 
 double normB[64];
 double meanA[64];
 double meanB[64];
 double stdevA[64];
 double stdevB[64];
 
 
 TFile * f = new TFile(data.c_str(),"READ");
 f->ls();


 for (int i = 0; i < 64; i++){ 
   
   TH1I * h1 = (TH1I*)f->Get(Form("adc_high_%d",i));

   h1->Fit(DGauss);
   h1->Draw();


   normA[i] = h1->GetFunction("DGauss")->GetParameter(0);  //probably don't need these
   normB[i] = h1->GetFunction("DGauss")->GetParameter(3);
   meanA[i] = h1->GetFunction("DGauss")->GetParameter(1);
   meanB[i] = h1->GetFunction("DGauss")->GetParameter(4);
   stdevA[i] = h1->GetFunction("DGauss")->GetParameter(2);
   stdevB[i] = h1->GetFunction("DGauss")->GetParameter(5);

   //DGauss->SetParameters(800,820,1,600,850,1);


}  // end of for loop



 ofstream f2;
 f2.open("fitoutput.dat");
 
 for(int j = 0; j < 64 ; j++){
   
   f2 << meanA[j] << '\t' << meanB[j] << std::endl;
   
 }  // end of for loop
 
 
 f2.close();

}

