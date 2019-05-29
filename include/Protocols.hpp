/**
* This header includes this distributed system's protocol definition.
* 
* It is a project for Distributed Systems Curricular Unit of Computer Science from University of Beira Interior @ 2019
* Given by professors Alexandra and Valderi
*
*
* This code is open and can and may be used to learn more about distributed systems functionality and low level network
* programming. This protocol is for layer 2 with a protocol written from scratch.
*
*
*
* If any question, you can contact this file author at almirante.silva(at)ubi.pt (make the right changes)
*/

/*
         L           D      |   BD -- Login
         |           |      |   |      |
    |-----------------------|--PP ------
       |         |          |
       A         M          |

   This protocol must be defined internally and externally
*/

// An operating system check has to be made to ensure homogeinity
/*
    Define homogeinity
*/
// end

#if !defined( __PROTOCOLS_HPP__ )
#define __PROTOCOLS_HPP__
#endif

class Protocols{
    // Define how packets must be!
    
#if !defined( __APPLE__ ) && !defined( __MACH__ )
  int in_cksum(); //This check is windows onlye
#endif
};