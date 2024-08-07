void testDoubleAxis(){
    int n[10]={1,2,3,4,5,6,7,8,9,10};
    int m[10];
    for(int i=0;i<10;i++){
        m[i]=n[i]*10;
    }

    TGraph *g1 = new TGraph(10,n,n);
    TGraph *g2 = new TGraph(10,n,m);

    g1->SetMarkerStyle(20);
    g1->SetMarkerColor(kRed);

    g2->SetMarkerStyle(20);
    g2->SetMarkerColor(kBlue);

    TCanvas *c1 =new TCanvas("c1","c1",1000,1000);
    TPad *pad = new TPad("pad","pad",0,0,1,1);
    pad->SetMargin(0.05,0.05,0.05,0.05);
    pad->Draw();
    pad->cd();

    g1->Draw("AP");
    g2->Draw("Psame");

    // 右側のY軸を追加
    Double_t xmin = g1->GetXaxis()->GetXmin();
    Double_t xmax = g1->GetXaxis()->GetXmax();
    Double_t ymin = g2->GetYaxis()->GetXmin();
    Double_t ymax = g2->GetYaxis()->GetXmax();

    TGaxis *axis = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,510,"+L");
    axis->SetLineColor(kBlue);
    axis->SetLabelColor(kBlue);
    axis->Draw();

    // 凡例を追加
    TLegend *legend = new TLegend(0.7,0.7,0.9,0.9);
    legend->AddEntry(g1,"g1","p");
    legend->AddEntry(g2,"g2","p");
    legend->Draw();

}