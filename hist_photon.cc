#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <TFile.h>
#include <TH1.h>

using namespace std;

unsigned int getBigEndian32(const char* b)
{
    return ((b[0] << 24) & 0xff000000) |
           ((b[1] << 16) & 0x00ff0000) |
           ((b[2] <<  8) & 0x0000ff00) |
           ((b[3] <<  0) & 0x000000ff);
}

bool isAdcHg(unsigned int data)
{
    return (data & 0x00680000) == 0x00000000;
}

void hist_photon(const string& filename)
{
    string::size_type pos = filename.find(".dat");
    if(pos == string::npos) {
        cerr << filename << " is not a dat file" << endl;
        return;
    }
    string rootfile_name(filename);
    rootfile_name.replace(pos, 5, ".root");

    TFile *f = new TFile(rootfile_name.c_str(), "RECREATE");
    TH1I* adcHigh[64];
    TH1I* adcLow[64];

    int nbin = 4096;
    for(int j = 0; j < 64; ++j) {
        adcHigh[j] = new TH1I(Form("ADC_HIGH_%d", j),
                              Form("ADC high gain %d", j),
                              nbin, 0, 4096);
    }

    ifstream datFile(filename.c_str(), ios::in | ios::binary);
    unsigned int events = 0;
    while(datFile) {
        char headerByte[4];
        datFile.read(headerByte, 4);
        unsigned int header = getBigEndian32(headerByte);
        bool isHeader = ((header >> 27) & 0x01) == 0x01;
        if(!isHeader) {
            std::cerr << "Frame Error" << std::endl;
            fprintf(stderr, "    %08X\n", header);
            std::exit(1);
        }
        size_t dataSize = header & 0x0fff;

        unsigned int scalerValues[69];
        char* dataBytes = new char[dataSize * 4];
        datFile.read(dataBytes, dataSize * 4);
	
	//---------- Fill Histogram ----------//

	for(size_t i = 0; i < dataSize; ++i) {
            unsigned int data = getBigEndian32(dataBytes + 4 * i);
            if(isAdcHg(data)) {
                int ch = (data >> 13) & 0x3f;
                bool otr = ((data >> 12) & 0x01) != 0;
                int value = data & 0x0fff;
                if(!otr) {
		  adcHigh[ch]->Fill(value);
                }
	    }
	}

        delete[] dataBytes;
        events++;
    }
    f->Write();
    f->Close();
}


int main(int argc, char** argv)
{
    if(argc != 2) {
        cerr << "hist <dat file>" << endl;
        return -1;
    }
    hist_photon(argv[1]);

    return 0;
}
