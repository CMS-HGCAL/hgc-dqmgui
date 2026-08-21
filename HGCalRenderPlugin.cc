#include "DQM/DQMRenderPlugin.h"
#include "utils.h"
#include "TProfile2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TColor.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include "TLine.h"
#include "TMath.h"
#include "TText.h"
#include "TPaletteAxis.h"
#include "TPolyMarker.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TLine.h"
#include "TList.h"
#include "TH2Poly.h"
#include "TString.h"
#include "TGraph.h"
#include "HGCalAuxiliaryInfo.h"

#include <iostream>

#include "guidelines_rotated_m5p0.h"
#include "guidelines_rotated_m15p0.h"
#include "guidelines_rotated_m25p0.h"

class HGCalRenderPlugin : public DQMRenderPlugin {
public:
  virtual bool applies(const VisDQMObject &o, const VisDQMImgInfo &) {
    if (o.name.find("HGCAL") != std::string::npos)
      return true;
    else
      return false;
  }

  virtual void preDraw(TCanvas *c, const VisDQMObject &o, const VisDQMImgInfo &, VisDQMRenderInfo &) {
    c->cd();
    // default values
    c->SetRightMargin(0.15);

    // customization for each ME type
    if (dynamic_cast<TH2Poly *>(o.object)) {
      c->SetLeftMargin(0.10);
      preDrawHex(o);
    } else if (dynamic_cast<TH2F *>(o.object)) {
      c->SetLeftMargin(0.20);
      preDrawTH2(c, o);
    } else if (dynamic_cast<TH1F *>(o.object)) {
      c->SetLeftMargin(0.10);
      preDrawTH1(c, o);
    }
  }

  virtual void postDraw(TCanvas *c, const VisDQMObject &o, const VisDQMImgInfo &) {
    if (dynamic_cast<TH2Poly *>(o.object)) {
      gStyle->SetTextSize();
      postDrawHex(c, o);
    } else if (dynamic_cast<TString *>(o.object)) {
      gStyle->SetTextSize(0.20);
    } else if (dynamic_cast<TH1F *>(o.object)) {
      gStyle->SetOptStat("nemruo"); // including underflow and overflow
      c->Modified();
      c->Update();
    } else if (dynamic_cast<TH2 *>(o.object)) {
      gStyle->SetTextSize();
      postDrawTH2(c, o);
//    } else if (dynamic_cast<TProfile *>(o.object)) {
//      TProfile *obj = dynamic_cast<TProfile *>(o.object);
//      assert(obj);
//      TString name(obj->GetName());
//
//      bool isProfileAvgADC = name.Contains("avgadc");
//      if(!isProfileAvgADC) return;
//
//      TH1D *h1d = obj->ProjectionX((name+"_h1d").Data(), "e");
//      if (!gPad || !gPad->FindObject(h1d)) {
//         h1d->Draw();
//      } else {
//         h1d->Paint();
//      }
    }
  }

private:
  void preDrawTH1(TCanvas *c __attribute__((unused)), const VisDQMObject &o) {
    TH1 *obj = dynamic_cast<TH1 *>(o.object);
    assert(obj);
    obj->SetOption("hist");
  }

  void preDrawHex(const VisDQMObject &o) {
    TH2Poly *obj = dynamic_cast<TH2Poly *>(o.object);
    TString name(obj->GetName());
    assert(obj);

    gStyle->SetPalette(kSunset);
    TColor::InvertPalette();
    obj->SetOption("colz");

    //customize display of hex plots
    if (name.Contains("hex_econdQualityLayer")) {
      Int_t colors[5] = {kGreen+1, kSpring+10, kOrange, kOrange+1, kRed};
      gStyle->SetPalette(5, colors);

      obj->SetMinimum(0.5); // ensure color scale for grade quality starts at 0.5
      obj->SetMaximum(5.5); // ensure color scale for grade quality ends at 5.5

      // Hide z-labels & Add title
      obj->GetZaxis()->SetLabelSize(0);
      obj->GetZaxis()->SetTickLength(0);
      obj->GetZaxis()->SetTitle("Issue Severity");
      obj->GetZaxis()->SetTitleSize(0.04);

      gStyle->SetPaintTextFormat(".0f");
      obj->SetMarkerSize(0.7);
      obj->SetOption("colz");

    } else if (name.Contains("hex_stdadc")) {
      // resctrict noize range in [0.0, 3.0]
      obj->SetMinimum(0.0);
      obj->SetMaximum(3.0);

      TList *functions = obj->GetListOfFunctions();
      if (functions) {
          TIter next(functions);
          TObject *funcObj;
          while ((funcObj = next())) {
              if (funcObj->InheritsFrom("TPolyMarker")) {
                  TPolyMarker *marker = (TPolyMarker*)funcObj;
                  marker->SetMarkerSize(1.5);
                  marker->SetMarkerColor(kWhite);
                  marker->SetMarkerStyle(5); // or 52
                  break; // assume only one TPolyMarker
              }
          }
      }

    } else if (name.Contains("hex_channelId") || name.Contains("hex_hgcrocPin") || name.Contains("hex_sicellPadId")) {
      gStyle->SetPaintTextFormat(".0f");
      obj->SetMarkerSize(0.7);
      obj->SetOption("colz");
    } else if (name.BeginsWith("module_") || name.BeginsWith("hex_avgPayloadLayer") || name.BeginsWith("hex_stdPayloadLayer")) {
      // display text info for hexagonal plots at layer-level
      if (name.Contains("std")) { gStyle->SetPaintTextFormat(".2f"); }
      else { gStyle->SetPaintTextFormat(".0f"); }
      obj->SetMarkerSize(2.0);
      obj->SetOption("colztext");
    } else if (name.Contains("_layer_")) {
      gStyle->SetPaintTextFormat(".2e");
      obj->SetMarkerSize(2.0);
      obj->SetOption("colz");

      // deal with occasional fake-uniform-distribution issue
      double threshold = 1e-3;
      double maxValue = obj->GetMaximum();
      if(maxValue > threshold) {
        obj->SetMinimum(threshold);
        obj->SetMaximum(maxValue);
      }

    } else {
      gStyle->SetPaintTextFormat(".2f");
      obj->SetMarkerSize(0.7);
      obj->SetOption("colz");
    }

    obj->SetStats(kFALSE);

    // // zoom in for TB2025
    // double reduction_factor = 0.3; // 70% width
    // double xmin = obj->GetXaxis()->GetXmin();
    // double xmax = obj->GetXaxis()->GetXmax();
    // double center = (xmax+xmin)*0.5;
    // double x_new_min = xmin + (center-xmin)*reduction_factor;
    // double x_new_max = xmax + (center-xmax)*reduction_factor;
    // obj->GetXaxis()->SetRangeUser(x_new_min, x_new_max);

    // double ymin = obj->GetYaxis()->GetXmin();
    // double ymax = obj->GetYaxis()->GetXmax();
    // center = (ymax+ymin)*0.5;
    // double y_new_min = ymin + (center-ymin)*reduction_factor;
    // double y_new_max = ymax + (center-ymax)*reduction_factor;
    // obj->GetYaxis()->SetRangeUser(y_new_min, y_new_max);

    //c->SetTicks(1,1);
    //c->SetGridx();
    //c->SetGridy();

  }  // end of preDrawHex

  void preDrawTH2(TCanvas *c, const VisDQMObject &o) {
    TH2F *obj = dynamic_cast<TH2F *>(o.object);
    assert(obj);

    // Set up canvas
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetCanvasColor(kWhite);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadBorderSize(0);

    // Set up the histogram
    obj->SetOption("colz");
    obj->GetXaxis()->SetNdivisions(510);
    obj->GetYaxis()->SetNdivisions(510);
    c->SetGridx();
    c->SetGridy();

    // acquire hist name
    TString name(obj->GetName());
    bool isSpecificQualityHist = name.Contains("econdQualityLayer") || (name=="econdQuality") || (name=="econdQualityLS") || (name=="layerQualityLS");
    bool isCorrelation = name.Contains("Corr");
    bool isGeneralEcondOrQuality = (o.name.find("econd") != std::string::npos) || (o.name.find("Quality") != std::string::npos);

    if (isSpecificQualityHist) {
        Int_t colors[5] = {kGreen+1, kSpring+10, kOrange, kOrange+1, kRed};
        gStyle->SetPalette(5, colors);
        gStyle->SetOptStat(10);
        gStyle->SetPaintTextFormat(".0f");

        obj->SetMinimum(0.5);
        obj->SetMaximum(5.5);
        obj->SetMarkerSize(0.7);
        obj->SetStats(0);

        // Hide z-labels & Add title
        obj->GetZaxis()->SetLabelSize(0);
        obj->GetZaxis()->SetTickLength(0);
        obj->GetZaxis()->SetTitle("Issue Severity");
        obj->GetZaxis()->SetTitleSize(0.04);

    } else if (isCorrelation) {
        gStyle->SetPalette(kSunset);
        TColor::InvertPalette();
        obj->SetStats(0);

    } else if (isGeneralEcondOrQuality) {
        gStyle->SetOptStat(10);
        gStyle->SetPalette(kSunset);
        TColor::InvertPalette();
        obj->SetStats(0);

    } else {
        gStyle->SetOptStat(1111);
        gStyle->SetPalette(kSunset);
        TColor::InvertPalette();
        obj->SetStats(0);
    }

  }  // end of preDrawTH2

  void postDrawTH2(TCanvas *c __attribute__((unused)), const VisDQMObject &o) {
    TH2 *obj = dynamic_cast<TH2 *>(o.object);
    assert(obj);
    TString name(obj->GetName());

    bool isSpecificQualityHist = name.Contains("Quality") || (name=="econd_lastLS");
    if(isSpecificQualityHist) return; //  no need to add a profile

    // adding profile
    TProfile *prof = obj->ProfileX((name+"_profile").Data(), 1, -1, "s");
    prof->Draw("same");

  }  // End of postDrawTH2

  void postDrawHex(TCanvas *c __attribute__((unused)), const VisDQMObject &o) {
    TH2Poly *obj = dynamic_cast<TH2Poly *>(o.object);
    TString name(obj->GetName());
    assert(obj);

    // Palette
    TPaletteAxis *palette =
        (TPaletteAxis*)obj->GetListOfFunctions()->FindObject("palette");

    if (palette) {
        obj->GetZaxis()->SetTitleOffset(1.2);
        obj->GetZaxis()->SetTitleFont(42);
        obj->GetZaxis()->SetTitleSize(0.035);
    }

    bool drawAuxLines = (name=="hex_avgadc_layer_44");

    // --------------------------------------------------
    // Auxiliary lines
    // --------------------------------------------------
    if(drawAuxLines) {

        for (int i = 0; i < guidelines_m5deg::n_hlines; ++i) {
            drawAuxGraph(
                c,
                Form("aux_m5deg_%d", i),
                guidelines_m5deg::hlines_n[i],
                guidelines_m5deg::hlines_x[i],
                guidelines_m5deg::hlines_y[i]
            );
        }

        for (int i = 0; i < guidelines_m15deg::n_hlines; ++i) {
            drawAuxGraph(
                c,
                Form("aux_m15deg_%d", i),
                guidelines_m15deg::hlines_n[i],
                guidelines_m15deg::hlines_x[i],
                guidelines_m15deg::hlines_y[i]
            );
        }

        for (int i = 0; i < guidelines_m25deg::n_hlines; ++i) {
            drawAuxGraph(
                c,
                Form("aux_m25deg_%d", i),
                guidelines_m25deg::hlines_n[i],
                guidelines_m25deg::hlines_x[i],
                guidelines_m25deg::hlines_y[i]
            );
        }

        drawAuxGraph(
            c,
            "aux_left_boundary",
            guidelines_m15deg::n_left_env_x,
            guidelines_m15deg::left_env_x,
            guidelines_m15deg::left_env_y
        );

        drawAuxGraph(
            c,
            "aux_right_boundary",
            guidelines_m15deg::n_right_env_x,
            guidelines_m15deg::right_env_x,
            guidelines_m15deg::right_env_y
        );
    }

    c->Modified();
    c->Update();
  }  // End of postDrawHex

  void drawAuxGraph( TCanvas *c, const char *name, int n, const double *x, const double *y) {
      c->cd();
  
      TGraph *g = dynamic_cast<TGraph *>( c->GetListOfPrimitives()->FindObject(name) );
  
      if (!g) {
          g = new TGraph(n, x, y);
          g->SetName(name);
  
          g->SetLineWidth(2);
          g->SetLineColor(TColor::GetColor("#1683D8"));
  
          // Let the pad/canvas own and delete it.
          g->SetBit(kCanDelete);
  
          g->Draw("L same");
      }
  }
};

static HGCalRenderPlugin instance;
