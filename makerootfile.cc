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

void makerootfile(const string& filename)
{
    string::size_type pos = filename.find(".dat");
    if(pos == string::npos) {
        cerr << filename << " is not a dat file" << endl;
        return;
    }
    string rootfile_name(filename);
    rootfile_name.replace(pos, 10, "calib.root");
    //rootfile_name.replace(pos, 5, ".root");
    
    int ADC[64];
    double PE[64];
    
    TFile *f = new TFile(rootfile_name.c_str(), "RECREATE");
    TTree*tree = new TTree("tree", "tree");
    tree->Branch( "ADC",      ADC,     "ADC[64]/I");
    tree->Branch( "PE",       PE,     "PE[64]/D");

    ifstream datFile(filename.c_str(), ios::in | ios::binary);
    unsigned int events = 0;

    //read file with mean peak values (fitoutput)
    
    double meanA[64];
    double meanB[64];
    double meanC[64];
    double gain[64];
    int k=0;
    /*
    std::ifstream f1;
    f1.open("test.dat");

    while(!f1.eof()){ f1 >> meanA[k] >> meanB[k] >> meanC[k]; 

    gain[k] = meanB[k] - meanA[k];

    //cout << gain[k] << '\t' << std::endl;

    k++;

    } //end of while
    */

    for(int k=0;k<64;k++){
      meanA[k]=820;
      gain[k]=50;
    }

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

	cout<<header<<endl;
	
	char timeByte[4];
        datFile.read(timeByte, 4);
        unsigned int time = getBigEndian32(timeByte);
	cout<<time<<endl;
		    
        char* dataBytes = new char[dataSize * 4];
        datFile.read(dataBytes, dataSize * 4);

	
	//---------- Fill tree ----------//

	for(size_t i = 0; i < dataSize; ++i) {
            unsigned int data = getBigEndian32(dataBytes + 4 * i);
            if(isAdcHg(data)) {
                int ch = (data >> 13) & 0x3f;
                bool otr = ((data >> 12) & 0x01) != 0;
                int value = data & 0x0fff;
                double pe = double((value-meanA[ch])/gain[ch]);
                if(!otr) {
		  ADC[ch]=value;
		  PE[ch]=pe;
		}
		if(ch==63){tree->Fill();}
	    }
	}

        delete[] dataBytes;
        events++;
    }
    tree->Write();
    f->Write();
    f->Close();
}


int main(int argc, char** argv)
{
    if(argc != 2) {
        cerr << "hist <dat file>" << endl;
        return -1;
    }
    makerootfile(argv[1]);

    return 0;
}
