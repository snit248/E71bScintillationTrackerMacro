void NPeakSearch_plot(){

    const TString unixtime = "1618018135";  //解析したいunixtime
    TString ifilename = "../merge/All_"+unixtime+"_conv.root";  //読み込むファイル名(All)
    //TString ifilename = "../merge/X1_"+unixtime+"_conv.root";  //読み込むファイル名(module)

    TFile *file = new TFile(ifilename); //ファイルを読み込む
    TTree *tree = (TTree*)file->Get("tree");  //TTreeの作成

    Int_t readadc[250];
    tree->SetBranchAddress("ADC", readadc);


    Int_t ch_N = 250; //チャンネル総数(All)
    //Int_t ch_N = 64; //チャンネル総数(module)
    Int_t  canvas_N = 16; //キャンバスの数(All)
    //Int_t  canvas_N = 4; //キャンバスの数(module)

    TCanvas *c[canvas_N];

    TH1F *h[ch_N], *histzoom[ch_N] ,*histzzoom[ch_N];;
    //TH1F *histzoom[ch_N] ,*histzzoom[ch_N];


    ////キャンバスの準備////
    for(Int_t icanvas=0; icanvas<canvas_N; icanvas++){
        c[icanvas] = new TCanvas(Form("c%d",icanvas), Form("c%d",icanvas) , 1200, 800);
        c[icanvas]->Divide(4,4);  //1枚のキャンバスを4行4列に分割
    }

    const Int_t maxpeaks = 1; //ピークの個数の最大数

    TSpectrum *s = new TSpectrum(maxpeaks);

    Double_t *xpeaks = {};  //xpeaksのメモリを確保
    Double_t pedestal, one, two;
    Int_t corrangemin, corrangemax; //ピークを探す範囲の最小・最大

    Double_t *entry = {};
    Double_t entry_ped, entry_one, entry_two;

    TGraph *g[250];

    Double_t peaks[3];
    Double_t entries[3];


    ////Npeak_new.datに書き込んで出力////
    ofstream fout("Npeak_new.dat");  //All
    //ofstream fout("Npeak_X1.dat"); //module


    ////チャンネル数でループする////
    for(Int_t i=0; i<ch_N; i++){
    //for(Int_t i=192; i<208; i++){
        cout << i << endl;

        ////hの定義////
        //h[i] = new TH1D(Form("h%d",i), Form("h%d",i), 400, 0, 4000);
        h[i] = new TH1F(Form("h%d",i), Form("h%d",i), 200, 650, 1050);

        //Project("ヒストグラムの名前", "詰めたいBranch変数")
        //tree->Project(Form("h%d",i), Form("ADC[%d]",i));

        c[i/16]->cd(i%16+1); //キャンバス内の区画移動
        gPad->SetLogy(); //Logスケールにする
        tree->Draw(Form("ADC[%d]>>h%d", i, i));

        //Search(ヒストグラム名, sigma, "オプション", thresholdの設定)
        //goffはオプション、graphics off
        //goffは表示させないだけでDrawはしている
        s->Search(h[i], 1, "", 1e-1);
        xpeaks = s->GetPositionX(); //これでpeakの時のxの値すなわちxpeaksの値を取ってくる
        pedestal = xpeaks[0];  //ぺデスタルはxpeakの0番目の要素
        peaks[0] = pedestal;

        entry = s->GetPositionY();
        entry_ped = entry[0];
        entries[0] = entry_ped;


        ////histzoomの範囲の設定////
        corrangemin = (Int_t)xpeaks[0]+35;  //範囲の最小値をpedestal+35に設定
        corrangemax = (Int_t)xpeaks[0]+85;  //範囲の最大値をpedestal+65に設定

        ////histzoomの定義////
        ////1p.e.を探す////
        histzoom[i] = new TH1F(Form("histzoom%d",i),Form("histzoom%d",i),30,corrangemin,corrangemax);
        //tree->Project(Form("histzoom%d",i), Form("ADC[%d]",i));
        c[i/16]->cd(i%16+1); //キャンバス内の区画移動
        gPad->SetLogy(); //Logスケールにする
        tree->Draw(Form("ADC[%d]>>histzoom%d", i, i)); //各チャンネルのADCのグラフを描く
        //Int_t nfound = s->Search(histzoom[i], 1, "", 1e-2); //nfoundはこの範囲でのピークの数
        Int_t nfound = s->Search(histzoom[i], 1., "", 1e-2);
        one = xpeaks[0]; //1p.e.の値をxpeakの0番目の要素とする
        peaks[1] = one;

        entry = s->GetPositionY();
        entry_one = entry[0];
        entries[1] = entry_one;


        ////histzzoomの範囲の設定////
        corrangemin = (Int_t)xpeaks[0]+40; //範囲の最小値を1p.e.+40に設定
        corrangemax = (Int_t)xpeaks[0]+90; //範囲の最大値を1p.e.+70に設定

        ////histzzoomの定義////
        ////2p.e.を探す////
        histzzoom[i] = new TH1F(Form("histzzoom%d",i),Form("histzzoom%d",i),30,corrangemin,corrangemax);
        //tree->Project(Form("histzzoom%d",i), Form("ADC[%d]",i));
        c[i/16]->cd(i%16+1);  //キャンバス内の区画移動
        gPad->SetLogy();  //Logスケールにする
        tree->Draw(Form("ADC[%d]>>histzzoom%d", i, i)); //各チャンネルのADCのグラフを描く
        Int_t nnfound = s->Search(histzzoom[i], 1.75, "", 1e-2); //nnfoundはこの範囲でのピークの数
        two = xpeaks[0]; //2p.e.の値をxpeakの0番目の要素とする
        peaks[2] = two;

        entry = s->GetPositionY();
        entry_two = entry[0];
        entries[2] = entry_two;

        //cout << pedestal << " " << one << " " << two << endl;
        //cout << "entry_ped=" << entry_ped << " " << "entry_one=" << entry_one << " " << "entry_two=" << entry_two << endl;

        //チャンネル番号、ピークの数、ぺデスタル、1p.e.、2p.e.の順に書き込み
        fout << i << " " << nfound+nnfound+1 << " " << pedestal << " " << one << " " << two << endl;


        g[i] = new TGraph(3, peaks, entries);
        g[i]->SetMarkerStyle(23);  //プロットの形
        g[i]->SetMarkerColor(2);  //プロットの色
        g[i]->SetMarkerSize(1.0); //プロットのサイズ


        //c[i/16]->cd(i%16+1);
        //gPad->SetLogy();


        h[i]->Draw("");
        //histzoom[i]->Draw("same P");
        //histzzoom[i]->Draw("same P");
        g[i]->Draw("same P");
    }
}
