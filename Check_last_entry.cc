void Check_last_entry(){
    // TFile と TTree を開く
    TString mergefilename = "../merge/All_1707011441_conv.root";
    //All_1700735626_conv.root
    //All_1700903564_conv.root
    //All_1701075572_conv.root
    //All_1701239897_conv.root
    //All_1701336021_conv.root
    //All_1701431185_conv.root
    //All_1701478689_conv.root
    //All_1702446582_conv.root
    //All_1702533973_conv.root
    //All_1702797691_conv.root
    //All_1703173071_conv.root
    //All_1707637325_conv.root

    TFile *mergefile = new TFile(mergefilename);
    TTree *tree = (TTree*)mergefile->Get("tree");

    // TTree の総エントリ数を取得
    Int_t nentries = tree->GetEntries();
    // UNIXTIME ブランチの値を読み込むための変数
    Int_t unixtime=0; // 単一の整数として宣言
    tree->SetBranchAddress("UNIXTIME", &unixtime);
    //tree->GetEntry(80007);
    //printf("%d\n",unixtime);

    // 最後のエントリを読み込む
    if (nentries > 0) {
        tree->GetEntry(nentries - 1); // 最後のエントリ
        //std::cout << "Last entry UNIXTIME: " << unixtime << std::endl;
        printf("%d\n",nentries-1);
        printf("%d\n",unixtime);
    }
}
