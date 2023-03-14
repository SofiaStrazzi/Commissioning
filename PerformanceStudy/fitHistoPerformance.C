#include "VariablesPerformance.h"

void fitHistoPerformance()
{
  TList *listOfOutput = new TList();
  gStyle->SetOptFit(0111);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetMarkerColor(kAzure);
  gStyle->SetLineColor(kAzure);

  TFile *outVisual = new TFile("out_Visual.root");

  //-------------------------------------------------------------------------------------------------------------
  // Definisco le variabili per i fit

  // TF1 *fDelta0 = new TF1("fDelta0", "gaus", -2000, 2000);
  TF1 *fDelta = new TF1("fDelta", "gaus", -2000, 2000);
  Float_t RefSigmaDelta[nPtRanges][nEtaRanges][nPhiRanges], RefSigmaErrDelta[nPtRanges][nEtaRanges][nPhiRanges];
  Float_t MeanSigmaDelta[nPtRanges][nEtaRanges][nPhiRanges], MeanSigmaErrDelta[nPtRanges][nEtaRanges][nPhiRanges];
  Float_t RefSigmaDelta0[nPtRanges], RefSigmaErrDelta0[nPtRanges], MeanSigmaDelta0[nPtRanges], MeanSigmaErrDelta0[nPtRanges]; // in tutti gli angoli
  Float_t PtMiddle[nPtRanges];
  Float_t PhiMiddle[nPhiRanges];

  TH1F *h0;
  TH1D *h;
  TH3D *h3d;

  //-------------------------------------------------------------------------------------------------------------
  // Fitto ed estraggo la sigma della reference

  h0 = (TH1F *)outVisual->Get("hReference");
  TF1 *fRefg = new TF1("fRefg", "gaus", -2000, 2000);
  h0->Fit(fRefg, "R,WW");

  RefSigma = fRefg->GetParameter(2) / sqrt(2);
  RefSigmaErr = fRefg->GetParError(2) / sqrt(2);

  //-------------------------------------------------------------------------------------------------------------
  // Estraggo tutti i parametri

  for (Int_t i = 0; i < nPtRanges; i++)
  {
    iPt = PtRangeMin + i * DeltaPtRange;
    PtMiddle[i] = iPt + 1 / 2 * DeltaPtRange;
    h0 = (TH1F *)outVisual->Get(Form("hDelta_%.1f_%.1f", iPt, iPt + DeltaPtRange));
    h0->Fit(fDelta, "R,WW");
    RefSigmaDelta0[i] = sqrt(fDelta->GetParameter(2) * fDelta->GetParameter(2) - RefSigma * RefSigma);
    RefSigmaErrDelta0[i] = fDelta->GetParError(2) * fDelta->GetParameter(2) / RefSigmaDelta0[i];
    MeanSigmaDelta0[i] = fDelta->GetParameter(1);
    MeanSigmaErrDelta0[i] = fDelta->GetParError(1);
    h3d = (TH3D *)outVisual->Get(Form("hDelta3d_%.1f_%.1f", iPt, iPt + DeltaPtRange));
    for (Int_t j = 0; j < nEtaRanges; j++)
    {
      jEta = EtaRangeMin + j * DeltaEtaRange;
      for (Int_t k = 0; k < nPhiRanges; k++)
      {
        if (i == 0 && j == 0)
          PhiMiddle[k] = kPhi + 1 / 2 * DeltaPhiRange;
        kPhi = PhiRangeMin + k * DeltaPhiRange;
        h = h3d->ProjectionX("h", h3d->GetYaxis()->FindBin(jEta), h3d->GetYaxis()->FindBin(jEta + DeltaEtaRange), h3d->GetZaxis()->FindBin(kPhi), h3d->GetZaxis()->FindBin(kPhi + DeltaPhiRange));
        h->Fit(fDelta, "R,WW");
        RefSigmaDelta[i][j][k] = sqrt(fDelta->GetParameter(2) * fDelta->GetParameter(2) - RefSigma * RefSigma);
        RefSigmaErrDelta[i][j][k] = fDelta->GetParError(2) * fDelta->GetParameter(2) / RefSigmaDelta[i][j][k];
        MeanSigmaDelta[i][j][k] = fDelta->GetParameter(1);
        MeanSigmaErrDelta[i][j][k] = fDelta->GetParError(1);
      }
    }
  }

  //---------------------------------------------------------
  // Stampa delle risoluzioni

  for (Int_t i = 0; i < nPtRanges; i++)
    cout << "\nPt= (" << PtRangeMin + i * DeltaPtRange << " , " << PtRangeMin + (i + 1) * DeltaPtRange << ") --> Sigma= (" << RefSigmaDelta0[i] << " +- " << RefSigmaErrDelta0[i] << ")" << endl;
  cout << endl;

  ///////// DA CANCELLARE - Stampa di tutti i valori
  
/*   for (Int_t i = 0; i < nPtRanges; i++)
      for (Int_t j = 0; j < nEtaRanges; j++)
        for (Int_t k = 0; k < nPhiRanges; k++)
          cout << i << j << k << " " << RefSigmaDelta[i][j][k] << " " << RefSigmaErrDelta[i][j][k] << " " << MeanSigmaDelta[i][j][k] << " " << MeanSigmaErrDelta[i][j][k] << endl;
 */
  outVisual->Close();

  //----------------------------------------------------------------------------------------
  // Output plots

  TFile *out_Plots = new TFile("out_Plots.root", "RECREATE");

  // MeanPt
  TGraphErrors *MeanPt = new TGraphErrors(nPtRanges, PtMiddle, MeanSigmaDelta0, 0, MeanSigmaErrDelta0);
  MeanPt->SetTitle("Mean vs p_{T} interval");
  MeanPt->SetName("MeanPt");
  TAxis *XaxisMeanPt = MeanPt->GetXaxis();
  TAxis *YaxisMeanPt = MeanPt->GetYaxis();
  XaxisMeanPt->SetTitle("P_{T} (GeV/c)");
  YaxisMeanPt->SetTitle("Mean #Delta#Deltat (ps)");
  MeanPt->Write();

  // SigmaPt
  TGraphErrors *SigmaPt = new TGraphErrors(nPtRanges, PtMiddle, RefSigmaDelta0, 0, RefSigmaErrDelta0);
  SigmaPt->SetTitle("#sigma_{TOF} vs p_{T} interval");
  SigmaPt->SetName("SigmaPt");
  TAxis *XaxisSigmaPt = SigmaPt->GetXaxis();
  TAxis *YaxisSigmaPt = SigmaPt->GetYaxis();
  XaxisSigmaPt->SetTitle("P_{T} (GeV/c)");
  YaxisSigmaPt->SetTitle("#sigma_{TOF} (ps)");
  SigmaPt->Write();

  // EtaPhi
  TMultiGraph *EtaPhiPt[nPtRanges], *EtaPhiPtMean[nPtRanges];
  TAxis *XaxisEtaPhiPt[nPtRanges], *XaxisEtaPhiPtMean[nPtRanges];
  TAxis *YaxisEtaPhiPt[nPtRanges], *YaxisEtaPhiPtMean[nPtRanges];

  TGraphErrors *graphEtaPt[nEtaRanges], *graphEtaPtMean[nEtaRanges];

  for (Int_t i = 0; i < nPtRanges; i++)
  {
    iPt = PtRangeMin + i * DeltaPtRange;

    EtaPhiPt[i] = new TMultiGraph();
    EtaPhiPt[i]->SetTitle(Form("#sigma_{TOF} shift for p_{T} (%.1f , %.1f)", iPt, iPt + DeltaPtRange));
    XaxisEtaPhiPt[i] = EtaPhiPt[i]->GetXaxis();
    YaxisEtaPhiPt[i] = EtaPhiPt[i]->GetYaxis();
    EtaPhiPt[i]->SetName(Form("EtaPhiPt%i", i));
    XaxisEtaPhiPt[i]->SetTitle("#Phi (rad)");
    YaxisEtaPhiPt[i]->SetTitle("#sigma_{TOF} (ps)");

    EtaPhiPtMean[i] = new TMultiGraph();
    EtaPhiPtMean[i]->SetTitle(Form("Mean Value shift for p_{T} (%.1f , %.1f)", iPt, iPt + DeltaPtRange));
    XaxisEtaPhiPtMean[i] = EtaPhiPtMean[i]->GetXaxis();
    YaxisEtaPhiPtMean[i] = EtaPhiPtMean[i]->GetYaxis();
    EtaPhiPtMean[i]->SetName(Form("EtaPhiPtMean%i", i));
    XaxisEtaPhiPtMean[i]->SetTitle("#Phi (rad)");
    YaxisEtaPhiPtMean[i]->SetTitle("Mean Value Shift (ps)");

    for (Int_t j = 0; j < nEtaRanges; j++)
    {
      jEta = EtaRangeMin + j * DeltaEtaRange;
      if (i != 0)
      {
        delete graphEtaPt[j];
        graphEtaPt[j] = nullptr;
        delete graphEtaPtMean[j];
        graphEtaPtMean[j] = nullptr;
      }
      graphEtaPt[j] = new TGraphErrors(nPhiRanges, PhiMiddle, RefSigmaDelta[i][j], 0, RefSigmaErrDelta[i][j]);
      graphEtaPt[j]->SetName(Form("#eta (%.1f , %.1f)", jEta, jEta + DeltaEtaRange));
      EtaPhiPt[i]->Add(graphEtaPt[j]);

      graphEtaPtMean[j] = new TGraphErrors(nPhiRanges, PhiMiddle, MeanSigmaDelta[i][j], 0, MeanSigmaErrDelta[i][j]);
      graphEtaPtMean[j]->SetName(Form("#eta (%.1f , %.1f)", jEta, jEta + DeltaEtaRange));
      EtaPhiPtMean[i]->Add(graphEtaPtMean[j]);
    }

    EtaPhiPt[i]->Write();
    EtaPhiPtMean[i]->Write();
  }
  out_Plots->Close();

  // return;
}
