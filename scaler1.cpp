
//draw scaler histogram, extract mean and rms

void scaler1(const char* filename="temp", const int dac=800, const int chnum=0, 
             const int trig=10000, const int counter=100){
	
	gStyle->SetOptFit();
	gStyle->SetTitleSize(0.05,"xy");
	gStyle->SetLabelSize(0.05,"xy");
	gStyle->SetLabelOffset(0.02,"x");
	gStyle->SetNdivisions(505,"xy");
	gStyle->SetTitleOffset(1.1,"x");
	gStyle->SetTitleOffset(1.3,"y");
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetHistLineWidth(2);
	gStyle->SetHistLineColor(kBlue);
	gStyle->SetFuncWidth(3);
	gStyle->SetFuncColor(kRed);
	
	const double countrate=1.0*trig/counter;
	
	TCanvas* c1=new TCanvas("c1","c1");
	TFile* file1=new TFile(Form("rootfile/%s.root",filename));
	TH1F* h1=(TH1F*) file1->Get(Form("scaler_%d",chnum));
	h1->SetTitle(Form("ch%d DAC=%d", chnum, dac));
	//h1->GetXaxis()->SetRangeUser(begin,end);
	h1->SetXTitle(Form("Scaler counts (#times%.0f Hz)", countrate));
	h1->SetYTitle("Counts");
	double xmin=h1->GetXaxis()->GetBinLowEdge(h1->FindFirstBinAbove());
	double xmax=h1->GetXaxis()->GetBinUpEdge( h1->FindLastBinAbove());
	double range=xmax-xmin;
	h1->GetXaxis()->SetRangeUser(xmin-0.1*range,xmax+0.1*range);
	int NEvents=h1->GetEntries();
	double mean = countrate * h1->GetMean();
	double rms  = countrate * h1->GetRMS();
	
	ofstream ofs1(Form("para/%s_ch%02d.txt",filename,chnum));
	ofs1 << "#dac\tnevents\tmean\trms\n";
	ofs1 << dac << "\t" << NEvents << "\t" << mean << "\t" << rms;
	ofs1.close();
	
	h1->Draw();
	c1->Print(Form("image_scaler_gif/%s_ch%02d.gif",filename,chnum));
	c1->Print(Form("image_scaler_eps/%s_ch%02d.eps",filename,chnum));
	
	
	return;
}
