for module in `seq 910002 910329`
    do
    for subrun in `seq 0 1000` #a-1:137 a-2:$1には変数
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