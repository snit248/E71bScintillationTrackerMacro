#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

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

bool isAdcLg(unsigned int data)
{
    return (data & 0x00680000) == 0x00080000;
}

bool isTdcLeading(unsigned int data)
{
    return (data & 0x00601000) == 0x00201000;
}

bool isTdcTrailing(unsigned int data)
{
    return (data & 0x00601000) == 0x00200000;
}

bool isScaler(unsigned int data)
{
    return (data & 0x00600000) == 0x00400000;
}

void hist(const string& filename)
{
    string::size_type pos = filename.find(".dat");
    if(pos == string::npos) {
        cerr << filename << " is not a dat file" << endl;
        return;
    }
	string datafile_dir = "./";
	string datafile_name = datafile_dir + filename;
	string rootfile_dir = "./";
    string rootfile_name = filename;
    rootfile_name.replace(pos, 5, ".root");
	rootfile_name = rootfile_dir + rootfile_name;

	const int SCALERBIN = 100000;
    TFile *f = new TFile(rootfile_name.c_str(), "RECREATE");
    TH1I* adcHigh[64];
    TH1I* adcLow[64];
    TH1I* tdcLeading[64];
    TH1I* tdcTrailing[64];
    TH1F* rate[67];
    TH1F* scaler[67];
	TH2F* adcHighAll = new TH2F("adc_high_all", "ADC high gain all channel", 64, 0, 64, 4096, 0, 4096);
	TH2F* adcLowAll = new TH2F("adc_low_all", "ADC low gain all channel", 64, 0, 64, 4096, 0, 4096);
	TH2F* tdcLeadingAll = new TH2F("tdc_leading_all", "TDC leading all channel", 64, 0, 64, 4096, 0, 4096);
	TH2F* tdcTrailingAll = new TH2F("tdc_trailing_all", "TDC trailing all channel", 64, 0, 64, 4096, 0, 4096);
	TH2F* scalerAll = new TH2F("scaler_all", "Scaler all channel", 64, 0, 64, SCALERBIN, 0, SCALERBIN);

    for(int i = 0; i < 64; ++i) {
        adcHigh[i] = new TH1I(Form("adc_high_%d", i),
                              Form("ADC high gain %d", i),
                              4096, 0, 4096);
        adcLow[i] = new TH1I(Form("adc_low_%d", i),
                             Form("ADC low gain %d", i),
                             4096, 0, 4096);
        tdcLeading[i] = new TH1I(Form("tdc_leading_%d", i),
                                 Form("TDC leading %d", i),
                                 4096, 0, 4096);
        tdcTrailing[i] = new TH1I(Form("tdc_trailing_%d", i),
                                  Form("TDC trailing %d", i),
                                  4096, 0, 4096);
        rate[i] = new TH1F(Form("rate_%d", i),
                             Form("Rate %d", i),
                             4096, 0, 5.0);
        scaler[i] = new TH1F(Form("scaler_%d", i),
                             Form("Scaler %d", i),
                             SCALERBIN, 0, SCALERBIN);
    }
    rate[64] = new TH1F("rate_or32u", "Rate OR32U",
                          4096, 0, 200);
    rate[65] = new TH1F("rate_or32l", "Rate OR32L",
                          4096, 0, 200);
    rate[66] = new TH1F("rate_or64", "Rate OR64",
                          4096, 0, 200);

    ifstream datFile(datafile_name.c_str(), ios::in | ios::binary);
    unsigned int scalerValuesArray[100][69];
    unsigned int events = 0;
	bool flag=true;
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
	
	//cout<<header<<endl;

        size_t dataSize = header & 0x0fff;
	
	char timeByte[4];
	datFile.read(timeByte, 4);
	unsigned int time = getBigEndian32(timeByte);
	cout << time <<endl;
	//cout << "!!!!!!" << endl;
	
        unsigned int scalerValues[69];
        char* dataBytes = new char[dataSize * 4];
        datFile.read(dataBytes, dataSize * 4);
	for(size_t i = 0; i < dataSize; ++i) {
            unsigned int data = getBigEndian32(dataBytes + 4 * i);
            if(isAdcHg(data)) {
                int ch = (data >> 13) & 0x3f;
                bool otr = ((data >> 12) & 0x01) != 0;
                int value = data & 0x0fff;
                if(!otr) {
                    adcHigh[ch]->Fill(value);
		    adcHighAll->Fill(ch,value);
		    //cout << ch << " " << value << endl;
                }
            }else if(isAdcLg(data)) {
                int ch = (data >> 13) & 0x3f;
                bool otr = ((data >> 12) & 0x01) != 0;
                int value = data & 0x0fff;
                if(!otr) {
                    adcLow[ch]->Fill(value);
		    adcLowAll->Fill(ch,value);
		    //cout << ch << " " << value << endl;
                }
            }else if(isTdcLeading(data)) {
                int ch = (data >> 13) & 0x3f;
                int value = data & 0x0fff;
                tdcLeading[ch]->Fill(value);
            	tdcLeadingAll->Fill(ch,value);
		cout << ch << " " << value << endl;
            }else if(isTdcTrailing(data)) {
                int ch = (data >> 13) & 0x3f;
                int value = data & 0x0fff;
                tdcTrailing[ch]->Fill(value);
            	tdcTrailingAll->Fill(ch,value);
		cout << ch << " " << value << endl;
            }else if(isScaler(data)) {
                int ch = (data >> 14) & 0x7f;
                int value = data & 0x3fff;
                scalerValues[ch] = value;
#if 1
                if(ch == 68) {
                    int scalerValuesArrayIndex = events % 100;
                    memcpy(scalerValuesArray[scalerValuesArrayIndex], scalerValues,
                           sizeof(scalerValues));
                }
#else

                if(ch == 68) {
                    int counterCount1MHz = scalerValues[67] & 0x1fff;
                    int counterCount1KHz = scalerValues[68] & 0x1fff;

                    // 1count = 1.0ms
                    double counterCount = (double)counterCount1KHz + counterCount1MHz / 1000.0;
                    // TODO
                    // Firmwareのバグを直したら消す
                    counterCount *= 2.0;
                    //cout << "counterCount: " << counterCount << endl;
                    for(size_t j = 0; j < 67; ++j) {
                        bool ovf = ((scalerValues[j] >> 13) & 0x01) != 0;
                        ovf = false;
                        double scalerCount = scalerValues[j] & 0x1fff;
                        //cout << "scalerCount: " << j << " " << scalerCount << endl;
                        if(!ovf && scalerCount != 0) {
                            double rate = scalerCount / counterCount; // kHz
                            //cout << "rate: " << rate << endl;
                            scaler[j]->Fill(rate);
                        }
                    }
                    //cout << endl;
                    //cout << endl;
                }
#endif
            }else {
	      //std::cerr << "Unknown data type" << std::endl;
            }
        }

        delete[] dataBytes;
        events++;
#if 1
        if(events % 100 == 0) {
            unsigned int scalerValuesSum[69];
            for(int i = 0; i < 69; ++i) {
                scalerValuesSum[i] = 0;
            }
            for(int i = 0; i < 100; ++i) {
                for(int j = 0; j < 69; ++j) {
                    scalerValuesSum[j] += scalerValuesArray[i][j];
                }
            }

            int counterCount1MHz = scalerValuesSum[67];
            int counterCount1KHz = scalerValuesSum[68];

            // 1count = 1.0ms
            double counterCount = (double)counterCount1KHz + counterCount1MHz / 1000.0;
            // TODO
            // Firmwareのバグを直したら消す
            counterCount /= 2.0;

            //cout << "counterCount: " << counterCount << endl;
            for(size_t j = 0; j < 64; ++j) {
				scaler[j]->Fill(scalerValuesSum[j]);
				scalerAll->Fill(j,scalerValuesSum[j]);
                bool ovf = ((scalerValuesSum[j] >> 13) & 0x01) != 0;
                ovf = false;
                double scalerCount = scalerValuesSum[j] & 0x1fff;
                //cout << "scalerCount: " << j << " " << scalerCount << endl;
                if(!ovf && scalerCount != 0) {
                    double ratebuf = scalerCount / counterCount; // kHz
                    //cout << "rate: " << rate << endl;
                    rate[j]->Fill(ratebuf);
                }
            }
            //cout << endl;
            //cout << endl;
        }
#endif
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
    hist(argv[1]);

    return 0;
}
