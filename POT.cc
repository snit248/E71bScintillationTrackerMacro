void POT() {
    // データの数
    const int n = 16; // 例として10個のデータ点を設定

    // 横軸データ (日付) と縦軸データ (POT)
    double days[n];
    double POT[n] = {0, 0.23, 8.7748, 18.4380, 25.5449, 33.5240, 38.95, 41.4843, 49.7075, 56.3998, 65.3390, 74.1435, 80.4167, 86.0932, 86.0932, 93.7373, }; // 例のデータ

    // 基準日付 (2023年11月23日)
    int year = 2023;
    int month = 11;
    int day = 23;

    // 日付データの設定
    for (int i = 0; i < n; i++) {
        TTimeStamp currentDate(year, month, day + i, 0, 0, 0); // i日後の日付を設定
        days[i] = currentDate.AsDouble(); // ROOTの内部形式に変換
    }

    // グラフの作成
    TGraph *gr = new TGraph(n, days, POT);

    // グラフの設定
    gr->SetTitle("Date vs POT;Date;POT x10^18"); // タイトルと軸ラベルの設定
    gr->SetMarkerStyle(20); // マーカーのスタイル
    gr->SetLineColor(kBlue); // 線の色
    gr->SetLineWidth(2); // 線の太さ

    /// キャンバスの作成
    TCanvas *c1 = new TCanvas("c1", "Date vs POT", 700, 500);

    // グラフの描画
    gr->Draw("ALP");

    // 時間軸の設定
    c1->SetGrid();
    gr->GetXaxis()->SetTimeDisplay(1);
    gr->GetXaxis()->SetTimeFormat("#splitline{%m/%d}{%Y}");
    gr->GetXaxis()->SetTimeOffset(0, "gmt");

    // X軸のラベル表示間隔の設定 (毎日表示)
    //gr->GetXaxis()->SetNdivisions(-506); // 負の数で日付の表示間隔を制御
}


