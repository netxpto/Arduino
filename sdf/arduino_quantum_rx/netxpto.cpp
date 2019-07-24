#include "netxpto.h"

using namespace std;

//########################################################################################################################################################
//######################################################### SIGNALS FUNCTIONS IMPLEMENTATION #############################################################
//########################################################################################################################################################

t_integer Signal::ready(){
 auto ready{ bufferLength };

 if (!bufferFull)
  {
    if (inPosition >= outPosition)
    {
 //     ready = inPosition - outPosition;
    }
    else
    {
//      ready = bufferLength + inPosition - outPosition;
    }
  }

  return ready;
}

t_integer Signal::space() {

  return (bufferLength - ready());

}
