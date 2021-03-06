// Original Author - Doug Berry

#include "TH1F.h"
#include "TList.h"
#include "TString.h"
#include "TFile.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include <iostream>
#include <sstream>
#include <string>

#include "th1fmorph.C"
#include "Normalization.C"
//#include "Normalization_ff.C"

using namespace std;
using namespace RooFit;

string dtoa(double value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

void dofit(double fitmass, vector <TString> InterpolationList, TFile* SourceFile, TFile* OutputFile, RooWorkspace* WorkSpace, int debug=1) {

  if (fitmass>=150 || fitmass<=105) {
    cout << "Warning!!!!!!!!!!! You must have an input mass between 105 and 140 GeV!" << endl << "Exiting Program!!!!" << endl;
    return;
  }

  if (floor(fitmass)-fitmass<0.00001 && floor(fitmass)-fitmass>0) fitmass=floor(fitmass);
  if (fitmass-ceil(fitmass)>-0.00001 && fitmass-ceil(fitmass)<0) fitmass=ceil(fitmass);
  
<<<<<<< InterpolateMass.C
  double Masses[9] = {105.0, 110.0, 115.0, 120.0,125.0, 130.0, 135.0,140.0,150.0};
=======
  double Masses[12] = {105.0, 110.0, 115.0, 120.0,125.0, 130.0, 135.0,140.0,150.0};
>>>>>>> 1.9.4.1
  double lowerbound = 0;
  double upperbound = 0;
  for (unsigned int i=0; i<10; i++) {
    if (fitmass>Masses[i] && fitmass<Masses[i+1]) {
      lowerbound = Masses[i];
      upperbound = Masses[i+1];
    } else if (fitmass==Masses[i]) {
      lowerbound = Masses[i-1];
      upperbound = Masses[i+1];
    }
  }

  TString MassString = dtoa(fitmass);
  //MassString.ReplaceAll(".","_");
  TString LowerBoundString = dtoa(lowerbound);
  LowerBoundString.ReplaceAll(".0","");
  TString UpperBoundString = dtoa(upperbound);
  UpperBoundString.ReplaceAll(".0","");
  RooRealVar RooRealMass = *(WorkSpace->var("CMS_hgg_mass"));
  
  for (unsigned int k=0; k < InterpolationList.size(); k++) {

    TString LowerHistName = InterpolationList[k];
    LowerHistName.ReplaceAll("115",LowerBoundString);
    TString UpperHistName = InterpolationList[k];
    UpperHistName.ReplaceAll("115",UpperBoundString);
    TString HistName = InterpolationList[k];
    HistName.ReplaceAll("115",MassString);
    
    TString HistTitle = "Interpolated Mass at ";
    HistTitle += dtoa(fitmass);
    HistTitle += "GeV";

    TH1F* LowerHist = (TH1F*) SourceFile->Get(LowerHistName.Data());
    TH1F* UpperHist = (TH1F*) SourceFile->Get(UpperHistName.Data());
    if (debug>=1) cout << "Calculating mass point at " << fitmass << "GeV with histograms " << LowerHistName << " and " << UpperHistName << endl;

    double Normalization = GetNorm(lowerbound, LowerHist, upperbound, UpperHist, fitmass);

    TH1F* MCHist = (TH1F*) SourceFile->Get(HistName.Data());
    TH1F* InterpolatedHist = (TH1F*) th1fmorph((Char_t*) HistName.Data(),(Char_t*) HistTitle.Data(),LowerHist,UpperHist,lowerbound,upperbound,fitmass,Normalization,0);

    if (MCHist!=NULL) {
      TString ResidualHistName = HistName;
      ResidualHistName += "_Residual";
      TH1F* ResidualHist = (TH1F*) InterpolatedHist->Clone(ResidualHistName.Data());
      ResidualHist->Add(MCHist,-1);
      OutputFile->WriteTObject(ResidualHist);
      ResidualHistName.ReplaceAll("th1f_","");
      RooDataHist RooDataResidual(Form("roohist_%s",ResidualHistName.Data()),ResidualHistName.Data(),RooRealMass,ResidualHist);
      WorkSpace->import(RooDataResidual);
    }

    if (MCHist==NULL) {
      OutputFile->WriteTObject(InterpolatedHist,InterpolatedHist->GetName());
      HistName.ReplaceAll("th1f_","");
      RooDataHist RooDataInterpolated(Form("roohist_%s",HistName.Data()),HistName.Data(),RooRealMass,InterpolatedHist);
      WorkSpace->import(RooDataInterpolated);
    } else {
      HistName += "_Interpolated";
      InterpolatedHist->SetName(HistName.Data());
      OutputFile->WriteTObject(InterpolatedHist);
      HistName.ReplaceAll("th1f_","");
      RooDataHist RooDataInterpolated(Form("roohist_%s",HistName.Data()),HistName.Data(),RooRealMass,InterpolatedHist);
      WorkSpace->import(RooDataInterpolated);
    }
  }

}

void InterpolateMass(double fitmass) {

  TString FileName = "CMS-HGG_";
  FileName += dtoa(fitmass);
  FileName.ReplaceAll(".","_");
  FileName += ".root";
  
  if (fitmass>=140 || fitmass<=105) {
    cout << "Warning!!!!!!!!!!! You must have an input mass between 105 and 140 GeV!" << endl << "Exiting Program!!!!" << endl;
    exit(1);
  }

  TFile* SourceFile = new TFile("CMS-HGG.root");
  TList* HistList = SourceFile->GetListOfKeys();
  RooWorkspace * WorkSpace = (RooWorkspace*) SourceFile->Get("cms_hgg_workspace");
  TFile* OutputFile = new TFile(FileName.Data(),"RECREATE");
  OutputFile->cd();
  vector<TString> InterpolationList;

  for (Int_t j=0; j<HistList->GetSize(); ++j) {

    TString HistName(HistList->At(j)->GetName());
    if (HistName.Contains("115")) InterpolationList.push_back(HistName);
    if (HistName.Contains("th1f")) {
      TH1F* temphist = (TH1F*) SourceFile->Get(HistName.Data());
      OutputFile->WriteTObject(temphist);
    }
    if (HistName.Contains("plot_data_pol_model")) {
      TCanvas* tempcan = (TCanvas*) SourceFile->Get(HistName.Data());
      OutputFile->WriteTObject(tempcan);
    }
  }

  dofit(fitmass, InterpolationList, SourceFile, OutputFile, WorkSpace);

  WorkSpace->Write();
  OutputFile->Close();
  delete WorkSpace;
  delete SourceFile;
  delete OutputFile;
  cout << "Done!" << endl;
}

void InterpolateMassRange(double Min, double Max, double Step, TString SourceFileName="CMS-HGG") {

  TString FileName = SourceFileName+"_";
  FileName += dtoa(Min);
  FileName += "_";
  FileName += dtoa(Max);
  FileName += "_";
  FileName += dtoa(Step);
  FileName.ReplaceAll(".","_");
  FileName += ".root";
  
  SourceFileName += ".root";
  TFile* SourceFile = new TFile(SourceFileName);
  TList* HistList = SourceFile->GetListOfKeys();
  RooWorkspace * WorkSpace = (RooWorkspace*) SourceFile->Get("cms_hgg_workspace");
  TFile* OutputFile = new TFile(FileName.Data(),"RECREATE");
  OutputFile->cd();
  vector<TString> InterpolationList;

  for (Int_t j=0; j<HistList->GetSize(); ++j) {

    TString HistName(HistList->At(j)->GetName());
    if (HistName.Contains("115")) InterpolationList.push_back(HistName);
    if (HistName.Contains("th1f")) {
      TH1F* temphist = (TH1F*) SourceFile->Get(HistName.Data());
      OutputFile->WriteTObject(temphist);
    }
    if (HistName.Contains("plot_data_pol_model")) {
      TCanvas* tempcan = (TCanvas*) SourceFile->Get(HistName.Data());
      OutputFile->WriteTObject(tempcan);
    }
    
  }
    
  for (double fitmass=Min; fitmass<Max; fitmass+=Step) {
    dofit(fitmass, InterpolationList, SourceFile, OutputFile, WorkSpace);
  }

  WorkSpace->Write();
  OutputFile->Close();
  delete WorkSpace;
  delete SourceFile;
  delete OutputFile;
  cout << "Done!" << endl;

}
