//
//  multihistqq.cpp
//  
//
//  Created by 金島遼太 on 2021/01/26.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>


#include <stdlib.h>

void multihistqq(){

    TFile *file0 = new TFile("../merge/All_1700398844_conv.root");
    TTree *tree = (TTree*)file0->Get("tree");
    
    Int_t ch_N=250;
    Int_t  canvas_N=16;
    
   
    
    Double_t  m;
    Double_t  l;
 
    
    TH1D *h[ch_N];
   
    
    TCanvas *cc[16];
    TF1 *dg1[ch_N];
    TF1 *dg2[ch_N];
    
    
   
    
    for(Int_t icanvas = 0;icanvas < canvas_N ; icanvas++){
        cc[icanvas] = new TCanvas(Form("cc%d",icanvas), Form("cc%d",icanvas) , 1200, 800);
        cc[icanvas]->Divide(4,4);
    }
     
    
    
        for(Int_t i = 0 ; i<ch_N; i++){
            
            cout<<i<<endl;
                
                h[i] = new TH1D(Form("h%d",i), Form("h%d",i), 600, 650, 1250);
          
                tree->Project(Form("h%d",i), Form("ADC[%d]",i));
            /*
                dg1[i] = new TF1(Form("dg1%d",i) , "gaus(0)" , 0, 1100);
                dg2[i] = new TF1(Form("dg2%d",i) , "gaus(0)" , 0, 1100);
                
                m = int(h[i]->GetMean());
                l = h[i]->GetRMS();
                
                dg1[i]->SetParameters(100,m-1,100);
                dg1[i]->SetParLimits(0,0,100000000);
                dg1[i]->SetParLimits(1,m-l,m+l);
                dg1[i]->SetParLimits(2,-10000,10000);
                
                dg2[i]->SetParameters(10,m+l*5,100);
                dg2[i]->SetParLimits(0,0,10000000);
                dg2[i]->SetParLimits(1,m+l,m+l*10);
                dg2[i]->SetParLimits(2,-10000,10000);
            */
                 cc[i/16]->cd(i%16+1);
           
            //    h[i]->Fit(Form("dg1%d",i),"","",m-l*3,m+l*3);
              //  h[i]->Fit(Form("dg2%d",i),"+","",m+l*3,m+l*7);
                //h[i]->Fit(Form("dg1%d",i));
               
               
                
                gPad->SetLogy();
                h[i]->Draw();
                
                
        
    }
    //return 0;
}
   


