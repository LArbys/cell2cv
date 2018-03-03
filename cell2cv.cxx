#include <iostream>
#include <vector>

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "DataFormat/IOManager.h"
#include "DataFormat/EventImage2D.h"

int main(int nargs, char** argv ) {

  std::string input = "~/data/PhysicsRun-2016_4_9_19_48_19-0005839-00000_20160410T171534_ext_bnb_20160413T184959_merged_5100.root";

  TFile f(input.c_str(),"open");
  
  TTree* sim = (TTree*)f.Get("Event/Sim");

  int run = 0;
  int subrun = 0;
  int event = 0;

  TClonesArray *calibGaussian_wf = 0;
  int calibGaussian_nChannel = 0;
  std::vector<int>* calibGaussian_channelId = 0;

  TClonesArray *nf_wf =0;
  int nf_nChannel = 0;
  std::vector<int>* nf_channelId = 0;
    
  sim->SetBranchAddress("runNo",    &run);
  sim->SetBranchAddress("subRunNo", &subrun);
  sim->SetBranchAddress("eventNo",  &event);
  sim->SetBranchAddress("calibGaussian_nChannel",  &calibGaussian_nChannel);
  sim->SetBranchAddress("calibGaussian_channelId", &calibGaussian_channelId );
  sim->SetBranchAddress("calibGaussian_wf",        &calibGaussian_wf);
  sim->SetBranchAddress("nf_nChannel",  &nf_nChannel);
  sim->SetBranchAddress("nf_channelId", &nf_channelId );
  sim->SetBranchAddress("nf_wf",        &nf_wf);

  size_t entry = 0;
  long bytes = sim->GetEntry(entry);

  larcv::IOManager io( larcv::IOManager::kWRITE );
  io.set_out_file( "baka.root" );
  io.initialize();
  
  while ( bytes>0 )  {
    std::cout << "(" << run << "," << subrun << "," << event << ")" << std::endl;
    std::cout << "  calibGaussian num channels: " << calibGaussian_nChannel << " " << calibGaussian_channelId << " " << calibGaussian_wf << std::endl;

    io.set_id( run, subrun, event );

    std::vector<larcv::Image2D> img_v;
    for ( size_t p=0; p<3; p++) {
      larcv::ImageMeta meta( 3456, 2400*2.0, 2400, 3456, 0, 2400+1008*6, p );
      larcv::Image2D img(meta);
      img.paint(0.0);
      img_v.emplace_back( std::move(img) );
    }

    for (size_t ich=0; ich<calibGaussian_channelId->size(); ich++) {
      TH1F* wf = (TH1F*)calibGaussian_wf->At(ich);
      int chId = calibGaussian_channelId->at(ich);
      std::cout << "  [" << ich << "] ch=" << calibGaussian_channelId->at(ich) << "  wf-bins=" << wf->GetNbinsX() << std::endl;

      int p = 0;
      int w = chId;
      if ( chId>=2400 && chId<4800 ) {
	p = 1;
	w = chId-2400;
      }
      else if ( chId>=4800 ) {
	p = 2;
	w = chId-4800;
      }

      larcv::Image2D& img = img_v[p];

      for (int i=0; i<wf->GetNbinsX(); i++) {
	float val = wf->GetBinContent(i);
	if ( val>0 )
	  img.set_pixel( 2400-1-i, w, val );
      }

      // for (int i=0; i<wf->GetNbinsX(); i++) {
      // 	std::cout << wf->GetBinContent(i) << " ";
      // }
      // std::cout << std::endl;
    }

    larcv::EventImage2D* evimg = (larcv::EventImage2D*)io.get_data( larcv::kProductImage2D, "wire" );
    evimg->Emplace( std::move(img_v) );
    
    // std::cout << "  nf num channels: " << nf_nChannel << " ids=" << nf_channelId->size() << " " << nf_wf << std::endl;    
    // for (size_t ich=0; ich<nf_channelId->size(); ich++) {
    //   TH1F* wf = (TH1F*)nf_wf->At(ich);
    //   std::cout << "  [" << ich << "] ch=" << nf_channelId->at(ich) << "  wf-bins=" << wf->GetNbinsX() << std::endl;
    //   // for (int i=0; i<wf->GetNbinsX(); i++) {
    //   // 	std::cout << wf->GetBinContent(i) << " ";
    //   // }
    //   // std::cout << std::endl;
    // }

    io.save_entry();
    
    entry++;
    bytes = sim->GetEntry(entry);
    if ( entry>10 )
      break;
  }

  io.finalize();

  return 0;
}

