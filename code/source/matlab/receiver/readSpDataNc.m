function data = readSpDataNc(filepath, Tx)
    nTx = length(Tx);
    TxSat = Tx;
    for iTx = 1 : nTx
        if strcmp(Tx(iTx), "UFO")
            TxSat(iTx) = "MUOS";
        end
        data.(Tx(iTx)) = table;
        data.(Tx(iTx)).rangeTR = ncread(filepath, strcat("rangeTR_", TxSat(iTx)));
        data.(Tx(iTx)).rangeTS = ncread(filepath, strcat("rangeTS_", TxSat(iTx)));
        data.(Tx(iTx)).rangeSR = ncread(filepath, strcat("rangeSR_", TxSat(iTx)));
        
        data.(Tx(iTx)).lookAngAzD = ncread(filepath, strcat("lookAngAzD_", TxSat(iTx)));
        data.(Tx(iTx)).lookAngElD = ncread(filepath, strcat("lookAngElD_", TxSat(iTx)));
        data.(Tx(iTx)).lookAngAzR = ncread(filepath, strcat("lookAngAzR_", TxSat(iTx)));
        data.(Tx(iTx)).lookAngElR = ncread(filepath, strcat("lookAngElR_", TxSat(iTx)));
    end

    data.flagSpotbeam = ncread(filepath, "flagSpotbeam");
end