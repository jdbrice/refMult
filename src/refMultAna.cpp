#include "refMultAna.h"
#include "constants.h"

using namespace jdbUtils;

refMultAna::refMultAna( TChain * chain, xmlConfig *config ) : analyzer( chain, config )
{
	pico = new TOFrPicoDst( _chain );
}

refMultAna::~refMultAna() {
	delete pico;

	cout << "[refMultAna.~refMultAna]" << endl;
}

void refMultAna::loopEvents(){

	jdbUtils::timer t;
	t.startTimer();

	if ( !_chain ){
		cout << "[refMultAna." << __FUNCTION__ << "] ERROR: Invalid chain " << endl;
		return;
	}

	Int_t nEvents = (Int_t)_chain->GetEntries();
	cout << "[refMultAna." << __FUNCTION__ << "] Loaded: " << nEvents << " events " << endl;

	book->cd( "" );
	book->make( "h.eventsByRun" );
	book->make( "h.refMultAll" );

	// make some histos here
	vector<int> runs;
	map<int, int> runMap;
	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	_chain->GetEntry(i);

    	int run = pico->run;
    	int refMult = pico->refMult;


    	vector<int>::iterator it = std::find( runs.begin(), runs.end(), run );
    	if ( it == runs.end() ){
    		runMap[ run ] = runs.size();
    		runs.push_back( run  );
    	}
	} // end loop on events

	int firstRun = 100000000;
	int lastRun = -1;
	TH1* hRefMult = book->get( "refMultAll" );
	book->cd( "byRun" );
	for ( int i = 0; i < runs.size(); i++ ){
		TH1 * tmp = (TH1*)(hRefMult->Clone( ("refMultRun" + ts(runs[ i ]) ).c_str() )  );
		book->add( "refMultRun" + ts(runs[ i ]), tmp );

		if ( runs[ i ] < firstRun )
			firstRun = runs[ i ];
		if ( runs[ i ] > lastRun )
			lastRun = runs[ i ];


	}

	cout <<" First run index : " << firstRun << endl;
	cout << "Last run index : " << lastRun << endl;

	double vzMin = config->getDouble( "cut.vz:min", -200 );
	double vzMax = config->getDouble( "cut.vz:max", 200 );
	double vrMax = config->getDouble( "cut.vrMax", 200 );

	// loop over all events
	for(Int_t i=0; i<nEvents; i++) {
    	_chain->GetEntry(i);

    	int run = pico->run;
    	int refMult = pico->refMult;
    	
    	double vZ = pico->vertexZ;
    	double vY = pico->vertexY;
    	double vX = pico->vertexX;
    	double vR = TMath::Sqrt( vX*vX + vY*vY ); 

    	if ( vZ < vzMin || vZ > vzMax )
    		continue;
    	if ( vR > vrMax )
    		continue;
    	book->cd( "" );
    	book->fill( "refMultAll", refMult );
    	book->cd( "byRun" );
    	book->fill( "refMultRun" + ts( run ), refMult );
	} // end loop on events

	book->cd("");
	book->make1D( "refMultAvg", "<refMult> by Run; run index; <refMult>;", runs.size(), 0, runs.size() );
	TH1 * refMultAvg = book->get( "refMultAvg" );

	book->cd( "byRun" );
	for ( int i = 0; i < runs.size(); i++ ){

		double avg = (book->get( "refMultRun" + ts( runs[ i ] ) ))->GetMean();
		int iRun = runs[ i ] - firstRun;
		cout << "iRun : " << runMap[ runs[ i ] ] << " <refMult> = " << avg << endl;
		refMultAvg->Fill( runMap[ runs[ i ] ], avg );

	}

	//TLine *line = new TLine(-3,ymax,3,ymax);
  	//line->SetLineColor(kRed);
  	//line->Draw();
	//book->fill( "eventsByRun", run - Constants::firstRun );

    
	
	book->cd( "" );
	report->newPage();
	book->style( "refMultAll" )->set( "style.log1D" )->draw();
	report->savePage();

	report->newPage();
	book->style( "refMultAvg" )->set( "style.refMultAvg" )->draw();
	report->savePage();
	


	cout << "[refMultAna." << __FUNCTION__ << "] completed in " << t.elapsed() << " seconds " << endl;
}