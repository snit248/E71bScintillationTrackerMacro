#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <iostream>
#include <sstream>

void merge_trees() {
    // ファイル名を指定
    const char* file1 = "../merge/All_1700735651_1701075597_conv.root";
    const char* file2 = "../merge/All_1701239897_conv.root";

    // TChainを使用してTTreeを結合
    TChain chain("tree");

    // 二つのファイルをチェーンに追加
    chain.Add(file1);
    chain.Add(file2);

    // 新しいファイル名の生成（../mergeディレクトリに保存）
    std::string new_file_name = "../merge/All_";

    TFile f1(file1);
    TTree* tree1 = (TTree*)f1.Get("tree");
    Int_t unixtime1[250]; // 仮定として配列サイズを250とする
    tree1->SetBranchAddress("UNIXTIME", unixtime1);
    tree1->GetEntry(0);
    new_file_name += std::to_string(unixtime1[0]) + "_";

    TFile f2(file2);
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
