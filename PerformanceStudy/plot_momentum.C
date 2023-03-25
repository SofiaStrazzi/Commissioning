#include "VariablesPerformance.h"
void SetStyle();
//----------------------------------------------------------------------------------------
// Chain

void addDFToChain(TString fileName, TChain &chain)
{
  TFile f(fileName, "READ");
  auto *lk = f.GetListOfKeys();
  for (int i = 0; i < lk->GetEntries(); i++)
  {
    TString dfname = lk->At(i)->GetName();
    if (!dfname.Contains("DF_"))
      continue;
    TString tname = Form("%s?#%s/O2deltatof", fileName.Data(), dfname.Data());
    cout << tname << endl;
    chain.Add(tname);
    // break;
  }
}

//----------------------------------------------------------------------------------------
// Inizio funzione principale

TList *plot_momentum(TString fileName = "${HOME}/cernbox2/LHC22m_523308_apass3_relval_cpu2/0/AnalysisResults_trees_TOFCALIB.root")
{
  TList *listOfOutput = new TList();

  TChain chain;
  if (fileName.EndsWith(".root"))
  {
    addDFToChain(fileName, chain);
  }
  else if (fileName.EndsWith(".txt"))
  {
    std::ifstream infile(fileName.Data());
    std::string line;
    int nfiles = 0;
    while (std::getline(infile, line))
    {
      nfiles++;
      if (nfiles > 500)
        break;
      addDFToChain(line, chain);
    }
  }
  // chain.Print();

/*   gStyle->SetOptFit(0111);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetMarkerColor(kBlack);
  gStyle->SetLineColor(kBlack); */

  //----------------------------------------------------------------------------------------
  // Istogramma DoubleDelta per la referenza in una regione cinematica

  TFile *foutP = new TFile("out_Visual.root", "RECREATE");

  cout << chain.GetEntries() << endl;
  chain.ls();
  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->cd();
  // chain.Draw("fP");
  // TH2F *hsqrt = new TH2F("hsqrt", "hsqrt", 40, -10, 10, 100, -1000, 1000);
  // chain.Draw("fDoubleDelta:fP>>hsqrt", "", "Colz");

  TH1F *hReference = new TH1F("hReference", "#Delta#Deltat Reference", 100, -2000, 2000);
  TCut minP = "fP>0.6";
  TCut maxP = "fP<0.7";
  TCut minTOFChi2 = "fTOFChi2<5";
  TCut maxTOFChi2 = "fTOFChi2>=0";

  chain.Draw("fDoubleDelta>>hReference", minP && maxP && minTOFChi2 && maxTOFChi2, "Colz");
  TAxis *XhRef = hReference->GetXaxis();
  XhRef->SetTitle("#Delta#Deltat (ps)");
  TF1 *fRefg = new TF1("fRefg", "gaus");
  fRefg->SetRange(-2000, 2000);
  hReference->Fit(fRefg, "R,WW");
  hReference->Draw("colz");
  hReference->Write();

  RefSigma = fRefg->GetParameter(2) / sqrt(2);
  RefSigmaErr = fRefg->GetParError(2) / sqrt(2);

  cout << "Sigma Reference= (" << RefSigma << "+-" << RefSigmaErr << ")" << endl;

  //----------------------------------------------------------------------------------------
  // Istogramma 2D DoubleDelta VS P

  TH2F *hDoubleDeltaVsP = new TH2F("hDoubleDeltaVsP", "p VS #Delta#Deltat; p (GeV/c); #Delta#Deltat (ps)", 100, 0, 5, 100, -2000, 2000);
  TCanvas *cDDvsP = new TCanvas("cDDvsP", "cDDvsP");
  cDDvsP->cd();
  chain.Draw("fDoubleDelta:fP>>hDoubleDeltaVsP", minTOFChi2 && maxTOFChi2, "Colz");
  hDoubleDeltaVsP->Write();

  //----------------------------------------------------------------------------------------
  // Istogramma 2D DoubleDelta VS P

  TCut minPForEtaPhi = "fP>1.6";
  TH2F *hPhiVsEta = new TH2F("hEtaVsPhi", "#eta VS #varphi;#varphi;#eta", 200, PhiRangeMin, PhiRangeMax, 200, EtaRangeMin, EtaRangeMax);
  TCanvas *cPhiVsEta = new TCanvas("cEtaVsPhi", "cEtaVsPhi");
  cPhiVsEta->cd();
  chain.Draw("fEta:fPhi>>hEtaVsPhi", minTOFChi2 && maxTOFChi2 && minPForEtaPhi, "Colz");
  hPhiVsEta->Write();

  //----------------------------------------------------------------------------------------
  // Istogramma ht-texp-FT0

  TH2F *ht_texp_FT0VSP = new TH2F("ht_texp_FT0VSP", "ht_texp_FT0VSP VS p; p (GeV/c); ht_texp_FT0VSP (ps)", 100, 0, 5, 100, -2000, 2000);
  TCanvas *ct_texp_FT0VSP = new TCanvas("ct_texp_FT0VSP", "ct_texp_FT0VSP");
  ct_texp_FT0VSP->cd();
  chain.Draw("fDeltaTPi-fEvTimeT0AC:fP>>ht_texp_FT0VSP", minTOFChi2 && maxTOFChi2, "Colz");
  ht_texp_FT0VSP->Write();

  //----------------------------------------------------------------------------------------
  // DoubleDelta per una diversa regione cinematica e per i diversi angoli

  Float_t P;
  chain.SetBranchAddress("fP", &P);
  Float_t Eta;
  chain.SetBranchAddress("fEta", &Eta);
  Float_t Phi;
  chain.SetBranchAddress("fPhi", &Phi);
  Float_t TOFChi2;
  chain.SetBranchAddress("fTOFChi2", &TOFChi2);
  Float_t DoubleDelta;
  chain.SetBranchAddress("fDoubleDelta", &DoubleDelta);

  //---------------------------------------------------------
  // Definisco tutti gli istogrammi

  TCanvas *c2 = new TCanvas("c2", "c2");
  TH1F *hDelta0[nPRanges];
  TAxis *XhDelta0[nPRanges];
  TF1 *fDelta00[nPRanges];
  TH3F *hDelta3d[nPRanges];

  for (Int_t i = 0; i < nPRanges; i++)
  {
    iP = PRangeMin + i * DeltaPRange;
    hDelta0[i] = new TH1F(Form("hDelta_%.1f_%.1f", iP, iP + DeltaPRange), Form("hDelta (%.1f , %.1f)", iP, iP + DeltaPRange), 100, -2000, 2000);
    fDelta00[i] = new TF1(Form("fDelta_%.1f_%.1f", iP, iP + DeltaPRange), "gaus", -2000, 2000);
    XhDelta0[i] = hDelta0[i]->GetXaxis();
    XhDelta0[i]->SetTitle("#Delta#Deltat (ps)");
    // Plot in 3D con doubledelta eta phi
    hDelta3d[i] = new TH3F(Form("hDelta3d_%.1f_%.1f", iP, iP + DeltaPRange), Form("hDelta3d (%.1f , %.1f);#Delta#Deltat (ps);#eta;#varphi", iP, iP + DeltaPRange), 100, -2000, 2000, 100, EtaRangeMin, EtaRangeMax, 100, PhiRangeMin, PhiRangeMax);
  }

  //---------------------------------------------------------
  // Riempio tutti gli istogrammi

  const Int_t nentries = chain.GetEntries();
  for (Int_t ientry = 0; ientry < nentries; ientry++)
  {
    if (ientry % 100000 == 0)
      Printf("%.1f", float(ientry) / nentries * 100);

    chain.GetEntry(ientry);
    for (Int_t i = 0; i < nPRanges; i++)
    {
      iP = PRangeMin + i * DeltaPRange;
      if (P > iP && P < (iP + DeltaPRange) && TOFChi2 < 5 && TOFChi2 >= 0)
      {
        hDelta0[i]->Fill(DoubleDelta);
        hDelta3d[i]->Fill(DoubleDelta, Eta, Phi);
        // cout << " " << DoubleDelta << " " << Eta << " " << Phi << endl;
      }
    }
  }

  //---------------------------------------------------------
  // Inserisco gli istogrammi in funzione del p nel file visul_out (già fittati) insieme alla referenza

  for (Int_t i = 0; i < nPRanges; i++)
  {
    hDelta0[i]->Fit(fDelta00[i], "R,WW");
    hDelta0[i]->Write();
  }
  //---------------------------------------------------------
  // Inserisco quelli vs p Eta e Phi in histograms_out

  for (Int_t i = 0; i < nPRanges; i++)
    hDelta3d[i]->Write();

  // foutP->Close();

  /* for (Int_t i = 0; i < nPRanges; i++)
      for (Int_t j = 0; j < nEtaRanges; j++)
        for (Int_t k = 0; k < nPhiRanges; k++)
        {
          delete hDelta[i][j][k];
          hDelta[i][j][k] = nullpr;
        }
  */

  return listOfOutput;
}
