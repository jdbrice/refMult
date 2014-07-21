
#include <iostream>
#include "allroot.h"
#include "constants.h"
#include "histoBook.h"
#include "xmlConfig.h"
#include "chainLoader.h"
#include "refMultAna.h"

int main( int argc, char* argv[] ) {

  if ( argc <= 1 ){
    cout << " call with config.xml file" << endl;
    return 0;
  }
  xmlConfig config( argv[ 1 ] );
  config.report();

  TChain* chain = new TChain( "tof" );
  chainLoader::load( chain, config.getString( "input.dataDir" ).c_str(), config.getInt( "input.dataDir:maxFiles" ) );


  refMultAna * rma = new refMultAna( chain, &config );

  rma->loopEvents();

  delete rma;

	return 0;
}
