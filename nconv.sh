
#!/bin/sh

XY=(Y1 Y2 X1 X2) 

#RAWDIR=/gpfs/fs03/t2k/beam/work/odagawa/data/ninja
RAWDIR=../data
calib=1573054450
phys=1573054729 #run a-1
#ito=1700735626 #E71b 1 11/23
#ito=1700903564 #E71 2 11/25
#ito=1701075572 #E71 3 11/27
#ito=1701239897 #E71 4 11/29
#ito=1700561694 #E71b physics test until subrun 5
#ito=1701336021  #E71b 5 11/30
#ito=1701431185 #E71b 6 12/1
#ito=1701478689 #E71b 7 12/2
#ito=1702446582 #E71b 8 12/13
#ito=1702533973 #E71b 9 12/14
#ito=1702797691 #E71b 10 12/17
#ito=1703173071 #E71b 11 12/22
#ito=1705039280 #E71b test 1/12
#ito=1705045718 #E71b testOC 1/12
#ito=1705561018 #E71b testLowEntries16 1/18
#ito=1705563849 #E71b testLowEntries16FlatCableReverse 1/18
#ito=1705566581 #E71b testLowEntries16new 1/18
#ito=1705643026 #E71b testLowEntries16FlatCableReverse 1/19
#ito=1706672531 #E71b test 1/31
ito=1708219820 #E71b 14

#1614760282
#1614834557
#1614760282
#1614739566
#1614695376
#1614608670
#1612319561
#1612249626
#1612241472
#1612230984
#1611305744
#1611798935
#1611794913
#1611716727
#1611714731
#1611298437
#1699346608
#1700398844
#1700561694 #no beam just a test subrun 5 までが本番条件
#All_1700735651_1701239921_conv.root 11/30までの全てのデータ
#1701431185 #E71b 6
#1705039280
#1705045718
#1705561018


#phys= #run a-2
for module in `seq 0 3`
    do
    for subrun in `seq -f %04g 0 $1` #a-1:137 a-2:$1には変数
        do
        if [ ! -e "../${XY[$module]}_${subrun}_${ito}.dat" ]; then
                #if [ ! -e "../${XY[$module]}_${subrun}_${ito}.dat" ]; then
        cp ${RAWDIR}/${XY[$module]}_${subrun}_${ito}.dat ../
        fi
            ./makerootfile_odagawaq ../data/${XY[$module]}_${subrun}_${ito}.dat
    done
    #hadd -f ../merge/${XY[$module]}_${phys}_calib.root ../${XY[$module]}_000*_${phys}_conv.root
    hadd -f ../merge/${XY[$module]}_${ito}_conv.root ../data/${XY[$module]}_*_${ito}.root
done
rm -f ../*.dat
rm -f ../*.root
