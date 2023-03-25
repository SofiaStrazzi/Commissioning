#include "VariablesPerformance.h"
void SetStyle();
void EtaPhimacro(int indexP = 0)
{
  TFile *out_Plots = new TFile("out_Plots.root");
  const TString colors[11] = {"#a6cee3", "#1f78b4", "#b2df8a", "#33a02c", "#fb9a99", "#e31a1c", "#fdbf6f", "#ff7f00", "#cab2d6", "#6a3d9a", "#ffff99"};

//----------------------------------------------------------------------------------------
// Sigma

  TCanvas *canSigma = new TCanvas();
  TMultiGraph *mgSigma = (TMultiGraph *)out_Plots->Get(Form("EtaPhiP%d", indexP));
  auto *frameSigma = canSigma->DrawFrame(0,-1000, 6, 1000, ";#varphi (rad);#sigma_{TOF} (ps)");
  for (Int_t i = 0; i < mgSigma->GetListOfGraphs()->GetEntries(); i++)
  {
    auto *gSigma = (TGraphErrors *)mgSigma->GetListOfGraphs()->At(i);
    //cout <<"Io sono eta numero "<< i<<endl;
    int col = TColor::GetColor(colors[i]);
    gSigma->SetLineWidth(2);
    gSigma->SetMarkerSize(0.8);
    gSigma->SetLineColor(col);
    gSigma->SetMarkerColor(col);
    gSigma->SetTitle(gSigma->GetName());
    mgSigma->GetListOfGraphs()->At(i)->Draw("LP same");
  }
  canSigma->BuildLegend();

//----------------------------------------------------------------------------------------
// Mean

  TCanvas *canMean = new TCanvas();
  TMultiGraph *mgMean = (TMultiGraph *)out_Plots->Get(Form("EtaPhiPMean%d", indexP));
  auto *frameMean = canMean->DrawFrame(0, -1000, 6, 1000, ";#varphi (rad);Mean Value (ps)");
  for (Int_t i = 0; i < mgMean->GetListOfGraphs()->GetEntries(); i++)
  {
    auto *gMean = (TGraphErrors *)mgMean->GetListOfGraphs()->At(i);
    int col = TColor::GetColor(colors[i]);
    gMean->SetLineWidth(2);
    gMean->SetMarkerSize(0.8);
    gMean->SetLineColor(col);
    gMean->SetMarkerColor(col);
    gMean->SetTitle(gMean->GetName());
    mgMean->GetListOfGraphs()->At(i)->Draw("LP");
  }
  canMean->BuildLegend();

//----------------------------------------------------------------------------------------
// Shift

TCanvas *canMean2 = new TCanvas();
TH1F *hshift = new TH1F("hshift", "Shift (ps)", 100, -150, 150);
  for (Int_t i = 0; i < mgMean->GetListOfGraphs()->GetEntries(); i++)
  {
    auto *gMean2 = (TGraphErrors *)mgMean->GetListOfGraphs()->At(i);
    for (Int_t j = 0; j < gMean2->GetN(); j++)
    {
      hshift->Fill(gMean2->GetPointY(j));
    }
  }
  hshift->Fit("gaus");
  hshift->Draw();
  hshift->Write();

}