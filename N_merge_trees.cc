#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <iostream>
#include <sstream>

void N_merge_trees() {
    // 7つのファイル名を指定
    const char* files[] = {
        "../merge/All_1700735626_conv.root",
        "../merge/All_1700903564_conv.root",
        "../merge/All_1701075572_conv.root",
        "../merge/All_1701239897_conv.root",
        "../merge/All_1701336021_conv.root",
        "../merge/All_1701431185_conv.root",
        "../merge/All_1701478689_conv.root",
        "../merge/All_1702446582_conv.root",
        "../merge/All_1702533973_conv.root",
        "../merge/All_1702797691_conv.root",
    };
    const int nFiles = sizeof(files) / sizeof(files[0]);

    // TChainを使用してTTreeを結合
    TChain chain("tree");

    // ファイルをチェーンに追加
    for (int i = 0; i < nFiles; ++i) {
        chain.Add(files[i]);
    }

    // 新しいファイル名の生成
    std::string new_file_name = "../merge/All_";

    TFile f1(files[0]);
    TTree* tree1 = (TTree*)f1.Get("tree");
    Int_t unixtime1[250]; // 仮定として配列サイズを250とする
    tree1->SetBranchAddress("UNIXTIME", unixtime1);
    tree1->GetEntry(0);
    new_file_name += std::to_string(unixtime1[0]) + "_";

    TFile f2(files[nFiles - 1]);
    TTree* tree2 = (TTree*)f2.Get("tree");
    Int_t unixtime2[250]; // 仮定として配列サイズを250とする
    tree2->SetBranchAddress("UNIXTIME", unixtime2);
    tree2->GetEntry(0);
    new_file_name += std::to_string(unixtime2[0]) + "_conv.root";

    // 新しいファイルに結合されたTTreeを保存
    chain.Merge(new_file_name.c_str());

    f1.Close();
    f2.Close();
}
