void GainCalib(){

    ifstream fin("./Npeak_new_bymyeyes.dat");  //Npeak_new_new.datを読み込む

    ofstream fout("./Gainlist_new_bymyeyes.dat");  //Gainlsit_new.datを出力


    const Int_t ch = 250; //チャンネル総数
    //const Int_t nch = 64; //各モジュールのチャンネル数
    double tmp,a;
    //double Y[3]={0,1,2},X[3]={0,1,2}; //YとXという配列を用意
    double Y[3] = {}; //読み込んだピークを入れる配列
    double X[3] = {0,1,2};  //X軸 0,1,2

    //TF1 *function = new TF1("関数の名前","関数の定義");
    TF1 *function = new TF1("function","[0]*x+[1]");

    ////TGraphの一般的な使い方////
    //プロットしたいx,y座標をいれた配列を用意して、そのアドレスをTGraphに渡す
    TGraph *g[ch];  //TGraphクラスのポインタを作成


    ////TCanvasの準備////
    Int_t canvas_N = 16;  //キャンバスの数
    TCanvas *c[canvas_N]; //キャンバスの作成


    for(Int_t icanvas = 0;icanvas < canvas_N ; icanvas++){
      c[icanvas] = new TCanvas(Form("c%d",icanvas), Form("c%d",icanvas), 1200, 800);
      //TCanvas *c = new TCanvas("name", "title", 横, 縦);
      c[icanvas]->Divide(4,4);    //4行4列に分割
    }


    for (Int_t i=0; i<ch; i++){

        fin >> tmp >> tmp >> Y[0] >> Y[1] >> Y[2]; //3つのピークを読み込んでY[3]の配列に入れる
        //cout << Y[0] << " " << Y[1] << " " << Y[2]; //読み込めているかの確認

        ////Y[1]>Y[2]のとき、Y[1]とY[2]を入れ替え////
        ////3つのピークを小さい順にする////
        if(Y[1]>Y[2]){  //もし2つ目のピークが3つ目より大きかったら
            a = Y[1];     //aに2つ目を代入
            Y[1] = Y[2];  //2つ目に3つ目を代入
            Y[2] = a;     //3つ目に2つ目を代入
        }


        g[i] = new TGraph( 3, X, Y ); //new TGraph( プロットの数, 配列名, 配列名 );

        g[i]->Fit(function);  //functionをFitさせる

        g[i]->SetTitle(Form("h%d",i));  //グラフにタイトルをつける
        g[i]->GetXaxis()->SetTitle("p.e."); //X軸の名前
        g[i]->GetYaxis()->SetTitle("ADC");  //Y軸の名前
        g[i]->SetMarkerStyle(8);  //プロットの形
        g[i]->SetMarkerColor(1);  //プロットの色
        g[i]->SetMarkerSize(0.5); //プロットのサイズ

        c[i/16]->cd(i%16+1);  //キャンパスの区画移動
        g[i]->Draw("AP"); //APはDrawオプション、A:グラフにまわりに軸表示、P:マーカーでプロット


        double gain = function->GetParameter(0);  //0番目のパラメータ[0]の値を得る、[0]はグラフの傾き=ゲイン
        fout << i << " " << Y[0] << " " << gain << endl;  //i、Y[0]、gainの順に書き込み
        //iはチャンネル番号、Y[0]はぺデスタル、gainはゲイン
    }
}
