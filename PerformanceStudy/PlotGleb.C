#include "VariablesPerformance.h"

void PlotGleb(int indexP = 0)
{
  gStyle->Reset("Plain");
  gStyle->SetOptTitle(0);
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameLineWidth(1);
  gStyle->SetFrameFillColor(kWhite);
  gStyle->SetPadColor(10);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetHistLineWidth(1);
  gStyle->SetHistLineColor(kRed);
  gStyle->SetFuncWidth(2);
  gStyle->SetFuncColor(kGreen);
  gStyle->SetLineWidth(2);
  gStyle->SetLabelSize(0.045,"xyz");
  gStyle->SetLabelOffset(0.01,"y");
  gStyle->SetLabelOffset(0.01,"x");
  gStyle->SetLabelColor(kBlack,"xyz");
  gStyle->SetTitleSize(0.05,"xyz");
  gStyle->SetTitleOffset(1.25,"y");
  gStyle->SetTitleOffset(1.2,"x");
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetTextSizePixels(26);
  gStyle->SetTextFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(kWhite);
  gStyle->SetLegendFont(42);
  TFile *outVisual = new TFile("out_Visual.root");

//----------------------------------------------------------------------------------------
// ct_texp_FT0VSP
TH1D *h1t_texp_FT0VSP;
TCanvas *ct_texp_FT0VSP = new TCanvas("ct_texp_FT0VSP", "ct_texp_FT0VSP");
TH2D *h2d = (TH2D *)outVisual->Get("ht_texp_FT0VSP");
h1t_texp_FT0VSP = h2d->ProjectionY("h1t_texp_FT0VSP", h2d->GetXaxis()->FindBin(PRangeMin+DeltaPRange*indexP), h2d->GetXaxis()->FindBin(PRangeMin+DeltaPRange*(indexP+1)));
h1t_texp_FT0VSP->SetName("h1t_texp_FT0VSP");
h1t_texp_FT0VSP->Fit("gaus","WW");
h1t_texp_FT0VSP->Draw();

}