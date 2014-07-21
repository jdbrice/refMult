#ifndef refMultAna_h
#define refMultAna_h

#include "analyzer.h"
#include "TOFrPicoDst.h"

class refMultAna : public analyzer
{
protected:

	TOFrPicoDst * pico;

public:
	refMultAna( TChain * chain, xmlConfig *config );
	~refMultAna();
	
	void loopEvents();

};






#endif