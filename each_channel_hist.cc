void each_channel_hist(){

// ROOTファイルを開きます。
TFile *file = new TFile("../merge/All_1700735651_1701478713_conv.root");

// ツリーを取得します。
TTree *tree = (TTree*)file->Get("tree");

Int_t nindex=250;

Int_t adc[nindex];
Int_t unixtime[nindex];
Int_t leadtime[nindex];
Int_t trailtime[nindex];
Float_t pe[nindex];
tree->SetBranchAddress("ADC", adc);
tree->SetBranchAddress("UNIXTIME", unixtime);
tree->SetBranchAddress("LEADTIME", leadtime);
tree->SetBranchAddress("TRAILTIME", trailtime);
tree->SetBranchAddress("PE", pe);

// ヒストグラムの配列を作成します。
const int nChannels = 250;
TH1F *hADC[nChannels];
for (int i = 0; i < nChannels; ++i) {
    // ヒストグラムの名前とタイトルを設定します。
    hADC[i] = new TH1F(Form("hADC_%d", i), Form("ADC Histogram for Channel %d", i), 600, 750, 3500); // ここでビンの数と範囲を適宜調整します。
}

// ツリーをループし、ヒストグラムにデータを填めます。
Long64_t nEntries = tree->GetEntries();
for (Long64_t entry = 0; entry < nEntries; ++entry) {
    tree->GetEntry(entry);
        if(unixtime[0]>1701939967){
        for (int i = 0; i < nChannels; ++i) {
            // adcブランチからデータを読み込みます。
            hADC[i]->Fill(adc[i]);
        }
    }
}

// キャンバスを作成し、最初のヒストグラムを描画します。
TCanvas *c1 = new TCanvas("c1", "ADC Histograms", 800, 600);
hADC[21]->Draw();
gPad->Update();
gPad->SetLogy();

// 残りのヒストグラムを描画する場合、新しいキャンバスを作成するか、
// 現在のキャンバスをクリアして次のヒストグラムを描画します。
}