#include "TMath.h"
#include <iomanip>

// standard includes
#include <cmath>
#include <map>
#include <set>
#include <vector>

void massBias(int mass_in=120) {

  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  gStyle->SetCanvasColor(0);
  gStyle->SetFrameBorderMode(0);

  bool plot_chi2=false;
  bool saveGifs=false;
  bool fastsim=false;
  bool correctShape=false;
  int shift=0;
  float sig_sf=0.;

  float sidebandWidth=0.02;
  float signalRegionWidth=0.07;

  TString mass_str;
  mass_str+=mass_in;
  TString mass_str2= mass_str;
  if (mass_in==150) mass_str2="145";

  float basemass[5];
  if (shift==0) {
    basemass[0] = 118.5;
    basemass[1] = 133.5;
    basemass[2] = 116.;
    basemass[3] = 131.;
    basemass[4] = 148.;
  } else if (shift==1) {
    basemass[0] = 121.;
    basemass[1] = 136.5;
    basemass[2] = 118.5;
    basemass[3] = 133.5;
    basemass[4] = 150.;
  } else {
    basemass[0] = 115.;
    basemass[1] = 130.;
    basemass[2] = 115.;
    basemass[3] = 130.;
    basemass[4] = 150.;
  }
  TString basemass_str[5];
  if (shift==0) {
    basemass_str[0] = "118.5";
    basemass_str[1] = "133.5";
    basemass_str[2] = "116.0";
    basemass_str[3] = "131.0";
    basemass_str[4] = "148.0";
  } else if (shift==1) {
    basemass_str[0] = "121.0";
    basemass_str[1] = "136.5";
    basemass_str[2] = "118.5";
    basemass_str[3] = "133.5";
    basemass_str[4] = "150.0";
  } else {
    basemass_str[0] = "115.0";
    basemass_str[1] = "130.0";
    basemass_str[2] = "115.0";
    basemass_str[3] = "130.0";
    basemass_str[4] = "150.0";
  }

  float mass[15];
  mass[2] = basemass[0]*(1-signalRegionWidth)/(1+sidebandWidth);
  mass[1] = mass[2]*(1-sidebandWidth)/(1+sidebandWidth);
  mass[0] = mass[1]*(1-sidebandWidth)/(1+sidebandWidth);
  mass[5] = basemass[1]*(1-signalRegionWidth)/(1+sidebandWidth);
  mass[4] = mass[5]*(1-sidebandWidth)/(1+sidebandWidth);
  mass[3] = mass[4]*(1-sidebandWidth)/(1+sidebandWidth);
  mass[6] = basemass[2]*(1+signalRegionWidth)/(1-sidebandWidth);
  mass[7] = mass[6]*(1+sidebandWidth)/(1-sidebandWidth);
  mass[8] = mass[7]*(1+sidebandWidth)/(1-sidebandWidth);
  mass[9] = basemass[3]*(1+signalRegionWidth)/(1-sidebandWidth);
  mass[10] = mass[9]*(1+sidebandWidth)/(1-sidebandWidth);
  mass[11] = mass[10]*(1+sidebandWidth)/(1-sidebandWidth);
  mass[12] = basemass[4]*(1+signalRegionWidth)/(1-sidebandWidth);
  mass[13] = mass[12]*(1+sidebandWidth)/(1-sidebandWidth);
  mass[14] = mass[13]*(1+sidebandWidth)/(1-sidebandWidth);

  int nbins[2];
  TString boost_str[2] = {"grad","ada"};
  Double_t xbins_nominal[20][2];

  TH1* hist_nominal[2];

  TFile *workspace = TFile::Open("/afs/cern.ch/user/f/futyand/scratch1/mva_ucsd/CMS-HGG_mit_2var_07_01_12_v2.root");
  TFile *workspace_mc;
  if (fastsim) {
    workspace_mc = TFile::Open("/afs/cern.ch/user/f/futyand/scratch1/mva_ucsd/CMS-HGG_mva_fastsim_12Jan.root");
  } else {
    workspace_mc = workspace;
  }
  for (int j=0; j<2; j++) {
    hist_nominal[j] = (TH1*)(workspace->Get("th1f_data_"+boost_str[j]+"_"+mass_str+".0_cat0"))->Clone();
    nbins[j] = hist_nominal[j]->GetNbinsX();
    for (int ibin=0; ibin<nbins[j]+1; ibin++) {
      xbins_nominal[ibin][j] = hist_nominal[j]->GetBinLowEdge(ibin+1);
    }
  }

  TH1* hist_data[15][2];
  TH1* hist_bkg[15][2];
  TH1* hist_data_fine[15][2];
  TH1* hist_bkg_fine[15][2];
  TH1* hist_data_corrected[15][2];
  TH1* hist_bkg_corrected[15][2];
  TH1* hist_sig_fine[2];
  TH1* hist_sig[2];

  for (int j=0; j<2; j++) {

    hist_sig_fine[j] = (TH1*)(workspace->Get("th1f_sig_BDT_"+boost_str[j]+"_ggh_140.0_cat0"))->Clone();

    hist_data_fine[0][j] = (TH1*)(workspace->Get("th1f_data_3low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data_fine[1][j] = (TH1*)(workspace->Get("th1f_data_2low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data_fine[2][j] = (TH1*)(workspace->Get("th1f_data_1low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data_fine[3][j] = (TH1*)(workspace->Get("th1f_data_3low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data_fine[4][j] = (TH1*)(workspace->Get("th1f_data_2low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data_fine[5][j] = (TH1*)(workspace->Get("th1f_data_1low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data_fine[6][j] = (TH1*)(workspace->Get("th1f_data_1high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data_fine[7][j] = (TH1*)(workspace->Get("th1f_data_2high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data_fine[8][j] = (TH1*)(workspace->Get("th1f_data_3high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data_fine[9][j] = (TH1*)(workspace->Get("th1f_data_1high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data_fine[10][j] = (TH1*)(workspace->Get("th1f_data_2high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data_fine[11][j] = (TH1*)(workspace->Get("th1f_data_3high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data_fine[12][j] = (TH1*)(workspace->Get("th1f_data_1high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_data_fine[13][j] = (TH1*)(workspace->Get("th1f_data_2high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_data_fine[14][j] = (TH1*)(workspace->Get("th1f_data_3high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();

    hist_bkg_fine[0][j] = (TH1*)(workspace_mc->Get("th1f_bkg_3low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg_fine[1][j] = (TH1*)(workspace_mc->Get("th1f_bkg_2low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg_fine[2][j] = (TH1*)(workspace_mc->Get("th1f_bkg_1low_BDT_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg_fine[3][j] = (TH1*)(workspace_mc->Get("th1f_bkg_3low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg_fine[4][j] = (TH1*)(workspace_mc->Get("th1f_bkg_2low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg_fine[5][j] = (TH1*)(workspace_mc->Get("th1f_bkg_1low_BDT_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg_fine[6][j] = (TH1*)(workspace_mc->Get("th1f_bkg_1high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg_fine[7][j] = (TH1*)(workspace_mc->Get("th1f_bkg_2high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg_fine[8][j] = (TH1*)(workspace_mc->Get("th1f_bkg_3high_BDT_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg_fine[9][j] = (TH1*)(workspace_mc->Get("th1f_bkg_1high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg_fine[10][j] = (TH1*)(workspace_mc->Get("th1f_bkg_2high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg_fine[11][j] = (TH1*)(workspace_mc->Get("th1f_bkg_3high_BDT_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg_fine[12][j] = (TH1*)(workspace_mc->Get("th1f_bkg_1high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_bkg_fine[13][j] = (TH1*)(workspace_mc->Get("th1f_bkg_2high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_bkg_fine[14][j] = (TH1*)(workspace_mc->Get("th1f_bkg_3high_BDT_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();

    Double_t xbins[20];
    for (int i=0; i<nbins[j]+1; i++) xbins[i]= xbins_nominal[i][j];
    workspace->cd();
    hist_data_fine[0][j]->Rebin(nbins[0],"th1f_nominal_bkg_3low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_data_fine[1][j]->Rebin(nbins[0],"th1f_nominal_bkg_2low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_data_fine[2][j]->Rebin(nbins[0],"th1f_nominal_bkg_1low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_data_fine[3][j]->Rebin(nbins[0],"th1f_nominal_bkg_3low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_data_fine[4][j]->Rebin(nbins[0],"th1f_nominal_bkg_2low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_data_fine[5][j]->Rebin(nbins[0],"th1f_nominal_bkg_1low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_data_fine[6][j]->Rebin(nbins[0],"th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_data_fine[7][j]->Rebin(nbins[0],"th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_data_fine[8][j]->Rebin(nbins[0],"th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_data_fine[9][j]->Rebin(nbins[0],"th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_data_fine[10][j]->Rebin(nbins[0],"th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_data_fine[11][j]->Rebin(nbins[0],"th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_data_fine[12][j]->Rebin(nbins[0],"th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);
    hist_data_fine[13][j]->Rebin(nbins[0],"th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);
    hist_data_fine[14][j]->Rebin(nbins[0],"th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);
    hist_sig_fine[j]->Rebin(nbins[0],"th1f_nominal_sig_"+boost_str[j]+"_ggh_140.0_cat0",xbins);

    workspace_mc->cd();
    hist_bkg_fine[0][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_3low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_bkg_fine[1][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_2low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_bkg_fine[2][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_1low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0",xbins);
    hist_bkg_fine[3][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_3low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_bkg_fine[4][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_2low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_bkg_fine[5][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_1low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0",xbins);
    hist_bkg_fine[6][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_bkg_fine[7][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_bkg_fine[8][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0",xbins);
    hist_bkg_fine[9][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_bkg_fine[10][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_bkg_fine[11][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0",xbins);
    hist_bkg_fine[12][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);
    hist_bkg_fine[13][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);
    hist_bkg_fine[14][j]->Rebin(nbins[0],"th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0",xbins);

    hist_data[0][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_3low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data[1][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_2low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data[2][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_1low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_data[3][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_3low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data[4][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_2low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data[5][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_1low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_data[6][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data[7][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data[8][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_data[9][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data[10][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data[11][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_data[12][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_1high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_data[13][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_2high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_data[14][j] = (TH1*)(workspace->Get("th1f_nominal_bkg_3high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_sig[j] = (TH1*)(workspace->Get("th1f_nominal_sig_"+boost_str[j]+"_ggh_140.0_cat0"))->Clone();
    hist_data[8][j]->Add(hist_sig[j],sig_sf);

    hist_bkg[0][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_3low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg[1][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_2low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg[2][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_1low_"+boost_str[j]+"_"+basemass_str[0]+"_cat0"))->Clone();
    hist_bkg[3][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_3low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg[4][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_2low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg[5][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_1low_"+boost_str[j]+"_"+basemass_str[1]+"_cat0"))->Clone();
    hist_bkg[6][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg[7][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg[8][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[2]+"_cat0"))->Clone();
    hist_bkg[9][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg[10][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg[11][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[3]+"_cat0"))->Clone();
    hist_bkg[12][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_1high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_bkg[13][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_2high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_bkg[14][j] = (TH1*)(workspace_mc->Get("th1f_nominal_bkg_mc_3high_"+boost_str[j]+"_"+basemass_str[4]+"_cat0"))->Clone();
    hist_bkg[8][j]->Add(hist_sig[j],sig_sf);

  }

  if (correctShape) {

    TString fastsim_str="";
    if (fastsim) fastsim_str = "_fastsim";
    TFile *f_bias = TFile::Open("/afs/cern.ch/user/f/futyand/scratch1/mva_ucsd/newBkgBias/BkgBias_mit_2var_07_01_12_v2"+fastsim_str+".root");
    float slope_data_in[100][2];
    float slope_mc_in[100][2];
    for (int j=0; j<2; j++) {
      for (int ibin=1; ibin<hist_data[0][j]->GetNbinsX()+1; ibin++) {
	graph_data = (TGraphErrors*)(f_bias->Get("tgraph_biasslopes_data_"+boost_str[j]+"_"+mass_str))->Clone();
	graph_mc = (TGraphErrors*)(f_bias->Get("tgraph_biasslopes_mc_"+boost_str[j]+"_"+mass_str))->Clone();
	slope_data_in[ibin][j] = graph_data->Eval(float(ibin)-0.5);
	slope_mc_in[ibin][j] = graph_mc->Eval(float(ibin)-0.5);
      }
    }

    for (int j=0; j<2; j++) {
      for(int isb=0; isb<15; isb++) {
	hist_data_corrected[isb][j] = (TH1*)hist_data[isb][j]->Clone();
	hist_bkg_corrected[isb][j] = (TH1*)hist_bkg[isb][j]->Clone();
	for (int ibin=1; ibin<hist_data[isb][j]->GetNbinsX()+1; ibin++) {
	  float deltam = mass[isb]-float(mass_in);
	  float corrfac_data = 1./(1.+(slope_data_in[ibin][j]*deltam));
	  float corrfac_mc = 1./(1.+(slope_mc_in[ibin][j]*deltam));
	  if (ibin==6) cout << j << " " << isb << " "  << deltam << " " << ibin << " " << hist_data[isb][j]->GetBinContent(ibin) << " " << slope_data_in[ibin][j] << " " << corrfac_data << endl; 
	  hist_data_corrected[isb][j]->SetBinContent(ibin,hist_data[isb][j]->GetBinContent(ibin)*corrfac_data);
	  hist_bkg_corrected[isb][j]->SetBinContent(ibin,hist_bkg[isb][j]->GetBinContent(ibin)*corrfac_mc);
	}
	hist_data_corrected[isb][j]->Scale(hist_data[isb][j]->Integral()/hist_data_corrected[isb][j]->Integral());
	hist_bkg_corrected[isb][j]->Scale(hist_bkg[isb][j]->Integral()/hist_bkg_corrected[isb][j]->Integral());
	hist_data[isb][j] = hist_data_corrected[isb][j];
	hist_bkg[isb][j] = hist_bkg_corrected[isb][j];
      }
    }

  }

  TGraphErrors *G_bdtout_data[10][2];
  TGraphErrors *G_bdtout_mc[10][2];
  TGraphErrors *G_slope_data[2];
  TGraphErrors *G_slope_mc[2];
  TGraphErrors *G_chi2pol1_data[2];
  TGraphErrors *G_chi2pol1_mc[2];
  TGraphErrors *G_chi2pol2_data[2];
  TGraphErrors *G_chi2pol2_mc[2];
  TMultiGraph *mG_slope[2];
  TMultiGraph *mG_chi2pol1[2];
  TMultiGraph *mG_chi2pol2[2];
  float slope_data[10][2];
  float slope_mc[10][2];
  float slope_data_err[10][2];
  float slope_mc_err[10][2];
  float slope_data_norm[10][2];
  float slope_mc_norm[10][2];
  float slope_data_temp[10];
  float slope_mc_temp[10];
  float slope_data_err_temp[10];
  float slope_mc_err_temp[10];
  float chi2pol1_data_temp[10];
  float chi2pol1_mc_temp[10];
  float chi2pol2_data_temp[10];
  float chi2pol2_mc_temp[10];
  float chi2pol1_data[10][2];
  float chi2pol1_mc[10][2];
  float chi2pol2_data[10][2];
  float chi2pol2_mc[10][2];

  float mass_err[15];
  for(int i=0; i<15; i++) mass_err[i]=0.;
  float bdtout_data[15];
  float bdtout_data_err[15];
  float bdtout_mc[15];
  float bdtout_mc_err[15];
  float bin[10];
  float bin_err[10];
  for(int i=0; i<10; i++) {
    bin[i]=0.5+float(i);
    bin_err[i]=0.;
  }
  float bin_shifted[10];
  float binshift[3]={-0.2,0.,0.2};

  TCanvas *c_sidebands = new TCanvas("c_mass","Sidebands used for slope determination",1000,700);
  c_sidebands->SetFillColor(0);

  workspace->cd();
  th1f_data_mass_cat0->SetMarkerStyle(20);
  th1f_data_mass_cat0->GetXaxis()->SetRangeUser(80.,190.);
  th1f_data_mass_cat0->Draw("e");

  float max = th1f_data_mass_cat0->GetMaximum();
  float max = th1f_data_mass_cat0->SetMaximum(max*1.1);

  float sideband_boundaries[2][15];
  TBox* sideband_box[15];
  for(int i=0; i<15; i++) {
    //cout << mass[i] << endl;
    sideband_boundaries[0][i] = mass[i]*(1-sidebandWidth);
    sideband_boundaries[1][i] = mass[i]*(1+sidebandWidth);
    sideband_box[i] = new TBox(sideband_boundaries[0][i],0.,sideband_boundaries[1][i],max*1.1);
    sideband_box[i]->SetFillColor(38);
    sideband_box[i]->SetFillStyle(3002);
    sideband_box[i]->Draw("same");
  }

  th1f_data_mass_cat0->Draw("same");
  gPad->RedrawAxis();

  TLine* line_sb[2][15];
  for (int i=0; i<15; i++) {
    for (int j=0; j<2; j++) {
      line_sb[j][i] = new TLine(sideband_boundaries[j][i],0.,sideband_boundaries[j][i],max*1.1);
      line_sb[j][i]->SetLineColor(38);
      line_sb[j][i]->SetLineWidth(3);
      line_sb[j][i]->SetLineStyle(9);
      line_sb[j][i]->Draw();
    }
  }

  if (saveGifs) c_sidebands->SaveAs("sidebands.gif");

  TH1* hist_bkg_scaled[15][2];
  TH1* hist_data_scaled[15][2];
  for(int j=0; j<2; j++) {
    for(int i=0; i<15; i++) {
      hist_data_scaled[i][j]=(TH1*)hist_data[i][j]->Clone();
      hist_data_scaled[i][j]->Scale(1./hist_data[i][j]->Integral());
      hist_bkg_scaled[i][j]=(TH1*)hist_bkg[i][j]->Clone();
      hist_bkg_scaled[i][j]->Scale(1./hist_bkg[i][j]->Integral());
    }
  }

  TCanvas *canvas[2];
  TLine *line[2];

  for(int j=0; j<2; j++) {

    line[j] = new TLine(0.,0.,float(nbins[j]),0.);
    line[j]->SetLineColor(4);
    line[j]->SetLineWidth(2);

    canvas[j] = new TCanvas("canvas_"+boost_str[j],"canvas_"+boost_str[j],1600,300);
    canvas[j]->Divide(nbins[j],2);
    canvas[j]->SetFillColor(0);

    TF1* func;

    float fitval_msig[15];
    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) fitval_msig[bdtbin]=0.;

    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) {
      for(int i=0; i<15; i++) {
	bdtout_data[i] = hist_data_scaled[i][j]->GetBinContent(bdtbin+1);
	bdtout_data_err[i] = hist_data_scaled[i][j]->GetBinError(bdtbin+1);
	bdtout_mc[i] = hist_bkg_scaled[i][j]->GetBinContent(bdtbin+1);
	bdtout_mc_err[i] = hist_bkg_scaled[i][j]->GetBinError(bdtbin+1);
      }

      G_bdtout_data[bdtbin][j] = new TGraphErrors(15,mass,bdtout_data,mass_err,bdtout_data_err);
      for(int fitItr=0; fitItr<3; fitItr++) {
	G_bdtout_data[bdtbin][j]->Fit("pol1","Q");
	func = G_bdtout_data[bdtbin][j]->GetFunction("pol1");
	for(int i=0; i<15; i++) {
	  float error_scalefac = sqrt(func->Eval(mass[i])/bdtout_data[i]);
	  //if (i==11 && j==1 && bdtbin==5) cout << fitItr << " " << error_scalefac << " " << G_bdtout_data[bdtbin][j]->GetErrorY(i) << " ";
	  G_bdtout_data[bdtbin][j]->SetPointError(i,0.,bdtout_data_err[i]*error_scalefac);
	  //if (i==11 && j==1 && bdtbin==5) cout << G_bdtout_data[bdtbin][j]->GetErrorY(i) << endl;
	}
      }

      canvas[j]->cd(bdtbin+1);
      G_bdtout_data[bdtbin][j]->SetMarkerSize(.8);
      G_bdtout_data[bdtbin][j]->SetMarkerColor(1);
      G_bdtout_data[bdtbin][j]->SetMarkerStyle(20);
      G_bdtout_data[bdtbin][j]->Draw("AP");
      G_bdtout_data[bdtbin][j]->GetXaxis()->SetLabelSize(0.07);
      G_bdtout_data[bdtbin][j]->GetYaxis()->SetLabelSize(0.07);
      G_bdtout_data[bdtbin][j]->GetXaxis()->SetTitleSize(0.07);
      G_bdtout_data[bdtbin][j]->GetXaxis()->SetNdivisions(5);
      G_bdtout_data[bdtbin][j]->Fit("pol2","Q");
      func = G_bdtout_data[bdtbin][j]->GetFunction("pol2");
      chi2pol2_data[bdtbin][j] = func->GetChisquare()/func->GetNDF();
      G_bdtout_data[bdtbin][j]->Fit("pol1","Q");
      func = G_bdtout_data[bdtbin][j]->GetFunction("pol1");
      slope_data[bdtbin][j] = func->GetParameter(1);
      slope_data_err[bdtbin][j] = func->GetParError(1);
      if (fitval_msig[bdtbin]==0.) fitval_msig[bdtbin] = func->Eval(float(mass_in));
      slope_data_norm[bdtbin][j] = func->Eval(float(mass_in));
      chi2pol1_data[bdtbin][j] = func->GetChisquare()/func->GetNDF();
      G_bdtout_data[bdtbin][j]->GetYaxis()->SetRangeUser(fitval_msig[bdtbin]*0.,fitval_msig[bdtbin]*2.);

      G_bdtout_mc[bdtbin][j] = new TGraphErrors(15,mass,bdtout_mc,mass_err,bdtout_mc_err);
      for(int fitItr=0; fitItr<3; fitItr++) {
	G_bdtout_mc[bdtbin][j]->Fit("pol1","Q");
	func = G_bdtout_mc[bdtbin][j]->GetFunction("pol1");
	for(int i=0; i<15; i++) {
	  float error_scalefac = sqrt(func->Eval(mass[i])/bdtout_mc[i]);
	  //if (i==12 && j==1 && bdtbin==5) cout << fitItr << " " << error_scalefac << " " << G_bdtout_mc[bdtbin][j]->GetErrorY(i) << " ";
	  G_bdtout_mc[bdtbin][j]->SetPointError(i,0.,bdtout_mc_err[i]*error_scalefac);
	  //if (i==12 && j==1 && bdtbin==5) cout << G_bdtout_mc[bdtbin][j]->GetErrorY(i) << endl;
	}
      }

      canvas[j]->cd(bdtbin+1+nbins[j]);
      G_bdtout_mc[bdtbin][j]->SetMarkerSize(.8);
      G_bdtout_mc[bdtbin][j]->SetMarkerColor(4);
      G_bdtout_mc[bdtbin][j]->SetMarkerStyle(20);
      G_bdtout_mc[bdtbin][j]->Draw("AP");
      G_bdtout_mc[bdtbin][j]->GetXaxis()->SetLabelSize(0.07);
      G_bdtout_mc[bdtbin][j]->GetYaxis()->SetLabelSize(0.07);
      G_bdtout_mc[bdtbin][j]->GetXaxis()->SetTitleSize(0.07);
      G_bdtout_mc[bdtbin][j]->GetXaxis()->SetNdivisions(5);
      G_bdtout_mc[bdtbin][j]->Fit("pol2","Q");
      func = G_bdtout_mc[bdtbin][j]->GetFunction("pol2");
      chi2pol2_mc[bdtbin][j] = func->GetChisquare()/func->GetNDF();
      G_bdtout_mc[bdtbin][j]->Fit("pol1","Q");
      func = G_bdtout_mc[bdtbin][j]->GetFunction("pol1");
      slope_mc[bdtbin][j] = func->GetParameter(1);
      slope_mc_err[bdtbin][j] = func->GetParError(1);
      slope_mc_norm[bdtbin][j] = func->Eval(float(mass_in));
      chi2pol1_mc[bdtbin][j] = func->GetChisquare()/func->GetNDF();
      G_bdtout_mc[bdtbin][j]->GetYaxis()->SetRangeUser(fitval_msig[bdtbin]*0.,fitval_msig[bdtbin]*2.);

    }

    Double_t nentries_bin;

    mG_slope[j] = new TMultiGraph();
    mG_chi2pol1[j] = new TMultiGraph();
    mG_chi2pol2[j] = new TMultiGraph();
    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) {
      slope_data_temp[bdtbin]=slope_data[bdtbin][j]/slope_data_norm[bdtbin][j];
      slope_mc_temp[bdtbin]=slope_mc[bdtbin][j]/slope_mc_norm[bdtbin][j];
      slope_data_err_temp[bdtbin]=slope_data_err[bdtbin][j]/slope_data_norm[bdtbin][j];
      slope_mc_err_temp[bdtbin]=slope_mc_err[bdtbin][j]/slope_mc_norm[bdtbin][j];
      chi2pol1_data_temp[bdtbin]=chi2pol1_data[bdtbin][j];
      chi2pol2_data_temp[bdtbin]=chi2pol2_data[bdtbin][j]-chi2pol1_data[bdtbin][j];
      chi2pol1_mc_temp[bdtbin]=chi2pol1_mc[bdtbin][j];
      chi2pol2_mc_temp[bdtbin]=chi2pol2_mc[bdtbin][j]-chi2pol1_mc[bdtbin][j];
    }
    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) bin_shifted[bdtbin]=bin[bdtbin]+binshift[1];
    G_slope_data[j] = new TGraphErrors(nbins[j],bin_shifted,slope_data_temp,bin_err,slope_data_err_temp);
    G_slope_data[j]->SetMarkerSize(1.);
    G_slope_data[j]->SetMarkerColor(1);
    G_slope_data[j]->SetMarkerStyle(20);
    G_chi2pol1_data[j] = new TGraphErrors(nbins[j],bin_shifted,chi2pol1_data_temp);
    G_chi2pol1_data[j]->SetMarkerSize(1.);
    G_chi2pol1_data[j]->SetMarkerColor(1);
    G_chi2pol1_data[j]->SetMarkerStyle(20);
    G_chi2pol2_data[j] = new TGraphErrors(nbins[j],bin_shifted,chi2pol2_data_temp);
    G_chi2pol2_data[j]->SetMarkerSize(1.);
    G_chi2pol2_data[j]->SetMarkerColor(1);
    G_chi2pol2_data[j]->SetMarkerStyle(20);
    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) bin_shifted[bdtbin]=bin[bdtbin]+binshift[2];
    G_slope_mc[j] = new TGraphErrors(nbins[j],bin_shifted,slope_mc_temp,bin_err,slope_mc_err_temp);
    G_slope_mc[j]->SetMarkerSize(1.);
    G_slope_mc[j]->SetMarkerColor(4);
    G_slope_mc[j]->SetMarkerStyle(20);
    G_chi2pol1_mc[j] = new TGraphErrors(nbins[j],bin_shifted,chi2pol1_mc_temp);
    G_chi2pol1_mc[j]->SetMarkerSize(1.);
    G_chi2pol1_mc[j]->SetMarkerColor(4);
    G_chi2pol1_mc[j]->SetMarkerStyle(20);
    G_chi2pol2_mc[j] = new TGraphErrors(nbins[j],bin_shifted,chi2pol2_mc_temp);
    G_chi2pol2_mc[j]->SetMarkerSize(1.);
    G_chi2pol2_mc[j]->SetMarkerColor(4);
    G_chi2pol2_mc[j]->SetMarkerStyle(20);
    mG_slope[j]->Add(G_slope_data[j]);
    mG_slope[j]->Add(G_slope_mc[j]);
    mG_chi2pol1[j]->Add(G_chi2pol1_data[j]);
    mG_chi2pol1[j]->Add(G_chi2pol1_mc[j]);
    mG_chi2pol2[j]->Add(G_chi2pol2_data[j]);
    mG_chi2pol2[j]->Add(G_chi2pol2_mc[j]);
  }

  TCanvas *canvas_summary[2];

  txt = new TLatex();
  txt->SetNDC();
  txt->SetTextSize(0.07);

  TLegend *leg = new TLegend(.15,.68,.35,.88);
  leg->SetBorderSize(0);
  leg->SetTextSize(.05);
  leg->AddEntry(G_slope_data[0],"Data","LP");
  leg->AddEntry(G_slope_mc[0],"MC","LP");

  for(int j=0; j<2; j++) {

    canvas_summary[j] = new TCanvas("canvas_summary_"+boost_str[j],"canvas_summary_"+boost_str[j],800,400);
    canvas_summary[j]->SetFillColor(0);
    canvas_summary[j]->Divide(2,1);

    gPad->SetGrid();
    mG_slope[j]->Draw("AP");
    mG_slope[j]->GetXaxis()->SetLimits(0.,float(nbins[j]));
    mG_slope[j]->GetXaxis()->SetLabelSize(0.05);
    mG_slope[j]->GetYaxis()->SetLabelSize(0.05);
    mG_slope[j]->GetXaxis()->SetTitleSize(0.05);
    mG_slope[j]->GetYaxis()->SetTitleSize(0.05);
    mG_slope[j]->GetXaxis()->SetTitle("BDT output bin number");
    mG_slope[j]->GetYaxis()->SetTitle("Fractional change in bin content / GeV");
    mG_slope[j]->GetYaxis()->SetRangeUser(-0.02,0.02);
    if (j==0) {
      txt->DrawLatex(0.4,0.8,"Gradient");
    } else {
      txt->DrawLatex(0.4,0.8,"Adaptive");
    }
    txt->Draw();
    line[j]->Draw();    
    leg->Draw();
  }

  TCanvas *canvas_chi2pol1;
  TCanvas *canvas_chi2pol2;

  if (plot_chi2) {

    canvas_chi2pol1 = new TCanvas("canvas_chi2pol1","canvas_chi2pol1",1400,300);
    canvas_chi2pol1->SetFillColor(0);
    canvas_chi2pol1->Divide(2,1);

    txt = new TLatex();
    txt->SetNDC();
    txt->SetTextSize(0.07);

    for(int j=0; j<2; j++) {

      canvas_chi2pol1->cd(1+j);
      gPad->SetGrid();
      mG_chi2pol1[j]->Draw("AP");
      mG_chi2pol1[j]->GetXaxis()->SetLimits(0.,float(nbins[j]));
      mG_chi2pol1[j]->GetXaxis()->SetLabelSize(0.05);
      mG_chi2pol1[j]->GetYaxis()->SetLabelSize(0.05);
      mG_chi2pol1[j]->GetXaxis()->SetTitleSize(0.05);
      mG_chi2pol1[j]->GetYaxis()->SetTitleSize(0.05);
      mG_chi2pol1[j]->GetXaxis()->SetTitle("BDT output bin number");
      mG_chi2pol1[j]->GetYaxis()->SetTitle("chi2/ndof pol1");
      mG_chi2pol1[j]->GetYaxis()->SetRangeUser(0.,2.);
      if (j==0) {
	txt->DrawLatex(0.4,0.8,"Gradient");
      } else {
	txt->DrawLatex(0.4,0.8,"Adaptive");
      }
      txt->Draw();
      leg->Draw();

    }

    canvas_chi2pol2 = new TCanvas("canvas_chi2pol2","canvas_chi2pol2",1400,300);
    canvas_chi2pol2->SetFillColor(0);
    canvas_chi2pol2->Divide(2,1);

    txt = new TLatex();
    txt->SetNDC();
    txt->SetTextSize(0.07);

    for(int j=0; j<2; j++) {

      canvas_chi2pol2->cd(1+j);
      gPad->SetGrid();
      mG_chi2pol2[j]->Draw("AP");
      mG_chi2pol2[j]->GetXaxis()->SetLimits(0.,float(nbins[j]));
      mG_chi2pol2[j]->GetXaxis()->SetLabelSize(0.05);
      mG_chi2pol2[j]->GetYaxis()->SetLabelSize(0.05);
      mG_chi2pol2[j]->GetXaxis()->SetTitleSize(0.05);
      mG_chi2pol2[j]->GetYaxis()->SetTitleSize(0.05);
      mG_chi2pol2[j]->GetXaxis()->SetTitle("BDT output bin number");
      mG_chi2pol2[j]->GetYaxis()->SetTitle("chi2/ndof(pol2) - chi2/ndof(pol1)");
      mG_chi2pol2[j]->GetYaxis()->SetRangeUser(-1.,1.);
      if (j==0) {
	txt->DrawLatex(0.4,0.8,"Gradient");
      } else {
	txt->DrawLatex(0.4,0.8,"Adaptive");
      }
      txt->Draw();
      line[j]->Draw();    
      leg->Draw();

      if (saveGifs) {
	canvas_chi2pol1->SaveAs("chi2pol1_"+mass_str+".gif");
	canvas_chi2pol2->SaveAs("chi2pol2_"+mass_str+".gif");
      }
    }

  }

  TCanvas *canvas_bdtout[2];
  TH1* hist_bdtout_scaled_binnum[15][2];
  TString i_str[15] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14"};

  for(int j=0; j<2; j++) {

    canvas_bdtout[j] = new TCanvas("canvas_bdtout_"+boost_str[j],"canvas_bdtout_"+boost_str[j]);
    canvas_bdtout[j]->SetFillColor(0);
    canvas_bdtout[j]->SetLogy();

    float max=0.;

    for(int i=0; i<15; i++) {
      hist_bdtout_scaled_binnum[i][j] = new TH1F("hist_bdtout_scaled_binnum_"+boost_str[j]+i_str[i],"hist_bdtout_scaled_binnum_"+boost_str[j]+i_str[i],nbins[0],0,float(nbins[0]));
      for (int ibin=0; ibin<nbins[0]+1; ibin++) {
	hist_bdtout_scaled_binnum[i][j]->SetBinContent(ibin,hist_data_scaled[i][j]->GetBinContent(ibin));
	hist_bdtout_scaled_binnum[i][j]->SetBinError(ibin,hist_data_scaled[i][j]->GetBinError(ibin));
      }
    }
    for(int i=0; i<7; i++) {
      hist_bdtout_scaled_binnum[i][j]->SetLineColor(kRed-i);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerColor(kRed-i);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerStyle(20);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerSize(.8);
      if (hist_bdtout_scaled_binnum[i][j]->GetMaximum()>max) max = hist_bdtout_scaled_binnum[i][j]->GetMaximum();
    }
    for(int i=14; i>6; i--) {
      hist_bdtout_scaled_binnum[i][j]->SetLineColor(kGreen-i);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerColor(kGreen-i);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerStyle(20);
      hist_bdtout_scaled_binnum[i][j]->SetMarkerSize(.8);
      if (hist_bdtout_scaled_binnum[i][j]->GetMaximum()>max) max = hist_bdtout_scaled_binnum[i][j]->GetMaximum();
    }

    hist_bdtout_scaled_binnum[0][j]->SetMaximum(max*1.1);
    hist_bdtout_scaled_binnum[0][j]->SetMinimum(0.);
    hist_bdtout_scaled_binnum[0][j]->Draw("e");
    hist_bdtout_scaled_binnum[0][j]->GetXaxis()->SetTitle("BDT output bin number");
    hist_bdtout_scaled_binnum[0][j]->GetYaxis()->UnZoom();
    for(int i=0; i<15; i++) {
      hist_bdtout_scaled_binnum[i][j]->Draw("e,same");
    }

  }

  if (saveGifs) {
    for(int j=0; j<2; j++) {
      canvas[j]->SaveAs("slope_"+boost_str[j]+"_"+mass_str+".gif");
      canvas_summary[j]->SaveAs("slope_"+boost_str[j]+"_summary_"+mass_str+".gif");
      canvas_bdtout[j]->SaveAs("bdtout_"+boost_str[j]+"_"+mass_str+".gif");
    }
  }

  TFile *fout_slopes = new TFile("BkgBias.root","update");
  fout_slopes->cd();
  TGraphErrors *tgraph_biasslopes_data_grad =  (TGraphErrors*)G_slope_data[0]->Clone("tgraph_biasslopes_data_grad_"+mass_str);
  TGraphErrors *tgraph_biasslopes_data_ada =  (TGraphErrors*)G_slope_data[1]->Clone("tgraph_biasslopes_data_ada_"+mass_str);
  TGraphErrors *tgraph_biasslopes_mc_grad =  (TGraphErrors*)G_slope_mc[0]->Clone("tgraph_biasslopes_mc_grad_"+mass_str);
  TGraphErrors *tgraph_biasslopes_mc_ada =  (TGraphErrors*)G_slope_mc[1]->Clone("tgraph_biasslopes_mc_ada_"+mass_str);
  tgraph_biasslopes_data_grad->Write();
  tgraph_biasslopes_data_ada->Write();
  tgraph_biasslopes_mc_grad->Write();
  tgraph_biasslopes_mc_ada->Write();

  /*
  //For Paul
  for(int j=0; j<2; j++) {
    for(int bdtbin=0; bdtbin<nbins[j]; bdtbin++) {
      TString bin_str;
      bin_str+=bdtbin;
      TGraphErrors *tgraph_bdtoutvsmass_data =  (TGraphErrors*)G_bdtout_data[bdtbin][j]->Clone("tgraph_bdtoutvsmass_data_"+boost_str[j]+"_"+mass_str+"_bin"+bin_str);
      TGraphErrors *tgraph_bdtoutvsmass_mc =  (TGraphErrors*)G_bdtout_mc[bdtbin][j]->Clone("tgraph_bdtoutvsmass_mc_"+boost_str[j]+"_"+mass_str+"_bin"+bin_str);
      tgraph_bdtoutvsmass_data->Write();
      tgraph_bdtoutvsmass_mc->Write();
    }
    for(int i=0; i<15; i++) {
      TString sb_str;
      sb_str+=(i+1);
      TH1 *hist_bdtout_data =  (TH1*)hist_data[i][j]->Clone("hist_bdtout_data_"+boost_str[j]+"_"+mass_str+"_sideband"+sb_str);
      TH1 *hist_bdtout_mc =  (TH1*)hist_data[i][j]->Clone("hist_bdtout_mc_"+boost_str[j]+"_"+mass_str+"_sideband"+sb_str);
      hist_bdtout_data->Write();
      hist_bdtout_mc->Write();
    }
  }
  */

}
