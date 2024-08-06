
void draw(const int filenum=0){

	TString rootfile_dir = "rootfile/";
	TString dataname = "temp";
	TFile* f1 = new TFile(rootfile_dir + dataname + Form("%d.root", filenum));
	TH2F* h1 = (TH2F*) f1->Get("adc_high_all");
	h1->GetYaxis()->SetRangeUser(700,1100);
	h1->Draw("colz");
}
