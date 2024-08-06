void PlotADC() {
    // ROOTファイルを開きます。
    TFile *file = new TFile("../merge/All_1699259775_conv.root");
    if (file->IsZombie()) {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    // ツリーを取得します。
    TTree *tree = (TTree*)file->Get("tree");
    if (!tree) {
        std::cout << "Error getting tree" << std::endl;
        return;
    }

    // 全てのチャンネルのヒストグラムを作成し、PDFに保存します。
    const int nChannels = 250;
    for (int channel = 0; channel < nChannels; channel++) {
        // ヒストグラムを作成します。
        TH1F *hADC = new TH1F(Form("hADC_%d", channel), Form("ADC Histogram for Channel %d", channel), 100, 0, 3000);

        // ツリーからデータを読み込みます。
        tree->Draw(Form("adc[%d]>>hADC_%d", channel, channel), "", "goff");

        // キャンバスを作成し、ヒストグラムを描画します。
        TCanvas *c = new TCanvas(Form("c%d", channel), Form("ADC Histogram for Channel %d", channel), 800, 600);
        c->SetLogy(); // 対数スケールを設定します。
        hADC->Draw(); // ヒストグラムを描画します。

        // PDFファイルを保存します。
        c->SaveAs(Form("adc_histogram_channel%d.pdf", channel));

        // キャンバスとヒストグラムを削除します。
        delete c;
        delete hADC;
    }

    // ファイルを閉じます。
    file->Close();
}
