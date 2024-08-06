#!/bin/bash

# コマンドライン引数を確認
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <subrun> <runtimename>"
    exit 1
fi

# コマンドラインから引数を取得
SUBRUN=$1
RUNTIMENAME=$2

XY=(Y1 Y2 X1 X2) 
RAWDIR=../data

#rsyncで松尾さんサーバーからデータ取得
rsync -avz ninja@MATSUOSAN:/home/ninja/public_html/ST_data/ /Users/shunito/Scinti/E71b/Controller/data/

# データファイルのコピーとrootファイルの生成
for module in `seq 0 3`
do
    for subrun in `seq -f %04g 0 $SUBRUN`
    do
        #if [ ! -e "../${XY[$module]}_${subrun}_${RUNTIMENAME}.dat" ]; then
        #    cp ${RAWDIR}/${XY[$module]}_${subrun}_${RUNTIMENAME}.dat ../
        #fi
        ./makerootfile_odagawaq ../data/${XY[$module]}_${subrun}_${RUNTIMENAME}.dat
    done
    # 各モジュールのrootファイルをマージ
    hadd -f ../merge/${XY[$module]}_${RUNTIMENAME}_conv.root ../data/${XY[$module]}_*_${RUNTIMENAME}.root
done

# 一時ファイルの削除
rm -f ../*.dat
rm -f ../*.root

# ROOT マクロを引数付きで実行
root -q "ModuleMergeAuto.cc(\"$RUNTIMENAME\")"
root "count_entries_xyAuto.cc(\"$RUNTIMENAME\")"
root "EventDisplay_peAuto.cc(\"$RUNTIMENAME\")"

#作成したグラフを松尾さんサーバへアップロード
#rsync -avz /Users/shunito/Scinti/E71b/graphs/ ninja@MATSUOSAN:/home/ninja/public_html/graphs/
