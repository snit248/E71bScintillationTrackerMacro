void Compare_each_ch(){
    // ROOTファイルを開きます。
    //TFile *file = new TFile("../merge/All_1702446582_conv.root");
    //TFile *file = new TFile("../tmp_st/All_1708219820_conv.root");
    //TFile *file = new TFile("../merge/All_1708835120_conv.root");//過電流下でのラン
    TFile *file = new TFile("../E71b_ALL_ST/E71b_ST_data.root");
    //All_1702446582_conv.root
    //All_1700735651_1701478713_conv.root
    //ito=1701478689 #E71b 7 12/2
    //1702533973 E71b 9
    //1703173071 E71b 11

    // ツリーを取得します。
    TTree *tree = (TTree*)file->Get("tree");

    // ブランチを取得
    const int nChannels = 250;
    Int_t adc[nChannels];
    Int_t leadtime[nChannels];
    Int_t trailtime[nChannels];
    Int_t unixtime[nChannels];
    tree->SetBranchAddress("ADC", adc);
    tree->SetBranchAddress("UNIXTIME", unixtime);
    tree->SetBranchAddress("LEADTIME", leadtime);
    tree->SetBranchAddress("TRAILTIME", trailtime);

    // ヒストグラムの配列を作成します。
    TH1F *hADC[nChannels];
    for (int i = 0; i < nChannels; ++i) {
        // ヒストグラムの名前とタイトルを設定します。
        hADC[i] = new TH1F(Form("hADC_%d", i), Form("ADC Histograms"), 100, 500, 3500); // ここでビンの数と範囲を適宜調整します。
    }

    // ツリーをループし、ヒストグラムにデータを填めます。
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);
        //if(unixtime[0]>1708668000){
            for (int i = 0; i < nChannels; ++i) {
                // adcブランチからデータを読み込みます。
                //hADC[i]->Fill(tree->GetLeaf(Form("adc[%d]", i))->GetValue());
                hADC[i]->Fill(adc[i]);
            }
        //}
    }

    //Long64_t nEntries = tree->GetEntries();
    //for (Long64_t entry = 0; entry < nEntries; ++entry) {
    //    tree->GetEntry(entry);
    //    //Bad channels Y
    //    hADC[32]->Fill(tree->GetLeaf(Form("adc[32]"))->GetValue());
    //    hADC[35]->Fill(tree->GetLeaf(Form("adc[35]"))->GetValue());
    //    hADC[36]->Fill(tree->GetLeaf(Form("adc[36]"))->GetValue());
    //    hADC[49]->Fill(tree->GetLeaf(Form("adc[49]"))->GetValue());
    //    hADC[56]->Fill(tree->GetLeaf(Form("adc[56]"))->GetValue());
//
    //    //Bad channels X
    //    hADC[197]->Fill(tree->GetLeaf(Form("adc[32]"))->GetValue());
//
    //    //Good channel Y
    //    hADC[46]->Fill(tree->GetLeaf(Form("adc[46]"))->GetValue());
    //    //Good channel X
    //    hADC[197]->Fill(tree->GetLeaf(Form("adc[197]"))->GetValue());
//
    //    //Edge channels Y
    //    hADC[0]->Fill(tree->GetLeaf(Form("adc[0]"))->GetValue());
    //    hADC[30]->Fill(tree->GetLeaf(Form("adc[30]"))->GetValue());
    //    hADC[31]->Fill(tree->GetLeaf(Form("adc[31]"))->GetValue());
    //    hADC[61]->Fill(tree->GetLeaf(Form("adc[61]"))->GetValue());
    //    hADC[62]->Fill(tree->GetLeaf(Form("adc[62]"))->GetValue());
    //    hADC[92]->Fill(tree->GetLeaf(Form("adc[92]"))->GetValue());
    //    hADC[93]->Fill(tree->GetLeaf(Form("adc[93]"))->GetValue());
    //    hADC[124]->Fill(tree->GetLeaf(Form("adc[124]"))->GetValue());
//
    //    //Edge channels X
    //    hADC[125]->Fill(tree->GetLeaf(Form("adc[125]"))->GetValue());
    //    hADC[155]->Fill(tree->GetLeaf(Form("adc[155]"))->GetValue());
    //    hADC[156]->Fill(tree->GetLeaf(Form("adc[156]"))->GetValue());
    //    hADC[186]->Fill(tree->GetLeaf(Form("adc[186]"))->GetValue());
    //    hADC[187]->Fill(tree->GetLeaf(Form("adc[187]"))->GetValue());
    //    hADC[217]->Fill(tree->GetLeaf(Form("adc[217]"))->GetValue());
    //    hADC[218]->Fill(tree->GetLeaf(Form("adc[218]"))->GetValue());
    //    hADC[248]->Fill(tree->GetLeaf(Form("adc[248]"))->GetValue());

    // Y比較
    //TCanvas *c1 = new TCanvas("c1", "ADC Histograms", 800, 600);
    //hADC[32]->Draw();
    //hADC[33]->Draw("same");
    //hADC[35]->Draw("same");
    //hADC[36]->Draw("same");
    //hADC[32]->SetLineColor(kRed);
    //hADC[35]->SetLineColor(kBlue);
    //hADC[36]->SetLineColor(kGreen);
//
    //// レジェンドの作成
    //TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9); // レジェンドの位置
    //legend->AddEntry(hADC[32], "Channel 32 Bad", "l");
    //legend->AddEntry(hADC[33], "Channel 33 Good", "l");
    //legend->AddEntry(hADC[35], "Channel 35 Bad", "l");
    //legend->AddEntry(hADC[36], "Channel 36 Bad", "l");
//
    ////統計ボックス非表示
    //hADC[32]->SetStats(0);
    //hADC[33]->SetStats(0);
    //hADC[35]->SetStats(0);
    //hADC[36]->SetStats(0);

    // X比較
    //TCanvas *c1 = new TCanvas("c1", "ADC Histograms", 800, 600);
    //hADC[186]->Draw();
    //hADC[185]->Draw("same");
    //hADC[197]->SetLineColor(kRed);
//
    //// レジェンドの作成
    //TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9); // レジェンドの位置
    //legend->AddEntry(hADC[197], "Channel 197 Bad", "l");
    //legend->AddEntry(hADC[198], "Channel 198 Good", "l");
//
    ////統計ボックス非表示
    //hADC[197]->SetStats(0);
    //hADC[198]->SetStats(0);

    // edge比較
    //TCanvas *c1 = new TCanvas("c1", "ADC Histograms", 800, 600);
    //hADC[186]->Draw();
    //hADC[185]->Draw("same");
    //hADC[186]->SetLineColor(kRed);
//
    //// レジェンドの作成
    //TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9); // レジェンドの位置
    //legend->AddEntry(hADC[186], "Channel 186 Bad", "l");
    //legend->AddEntry(hADC[185], "Channel 185 Good", "l");
//
    ////統計ボックス非表示
    //hADC[186]->SetStats(0);
    //hADC[185]->SetStats(0);

    //4つのチャンネルの比較
    int n1=195, n2=196, n3=197, n4=198;
    TCanvas *c1 = new TCanvas("c1", "ADC Histograms", 800, 600);
    hADC[n1]->Draw();
    hADC[n2]->Draw("same");
    hADC[n3]->Draw("same");
    hADC[n4]->Draw("same");
    hADC[n2]->SetLineColor(kRed);
    hADC[n3]->SetLineColor(kGreen);
    hADC[n4]->SetLineColor(kOrange);

    // レジェンドの作成
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9); // レジェンドの位置
    legend->AddEntry(hADC[n1], ("Channel " + std::to_string(n1)).c_str(), "l");
    legend->AddEntry(hADC[n2], ("Channel " + std::to_string(n2)).c_str(), "l");
    legend->AddEntry(hADC[n3], ("Channel " + std::to_string(n3)).c_str(), "l");
    legend->AddEntry(hADC[n4], ("Channel " + std::to_string(n4)).c_str(), "l");

    //統計ボックス非表示
    hADC[n1]->SetStats(0);
    hADC[n2]->SetStats(0);
    hADC[n3]->SetStats(0);
    hADC[n4]->SetStats(0);

    // レジェンドの描画
    legend->Draw();

    gPad->Update(); 
    gPad->SetLogy(); 
}