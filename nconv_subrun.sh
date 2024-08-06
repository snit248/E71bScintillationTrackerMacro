#!/bin/sh

XY=(Y1 Y2 X1 X2) 

#RAWDIR=/gpfs/fs03/t2k/beam/work/odagawa/data/ninja
RAWDIR=../data
calib=1573054450
phys=1573054729
ito=1700903564

# スクリプトの第一引数を開始サブラン、第二引数を終了サブランとして使用
start_subrun=$1
end_subrun=$2

for module in `seq 0 3`
do
    # 結合するファイルのリストを作成
    file_list=""
    for subrun in `seq -f %04g $start_subrun $end_subrun`
    do
        if [ ! -e "../${XY[$module]}_${subrun}_${ito}.dat" ]; then
            cp ${RAWDIR}/${XY[$module]}_${subrun}_${ito}.dat ../
        fi
        ./makerootfile_odagawaq ../data/${XY[$module]}_${subrun}_${ito}.dat
        file_list+=" ../data/${XY[$module]}_${subrun}_${ito}.root"
    done

    # 指定されたサブラン範囲に一致するファイルのみを結合
    hadd -f ../merge/${start_subrun}_${end_subrun}_${XY[$module]}_${ito}_conv.root $file_list
done

rm -f ../*.dat
rm -f ../*.root
