
#include <string.h>
#include <memory.h>
 
 
static unsigned int _parseDecimal ( const char** pchCursor );

 
 
static unsigned int _parseHex ( const char** pchCursor );
 
 
 
//Parse a textual IPv4 or IPv6 address, optionally with port, into a binary
//array (for the address, in network order), and an optionally provided port.
//Also, indicate which of those forms (4 or 6) was parsed.  Return true on
//success.  ppszText must be a nul-terminated ASCII string.  It will be
//updated to point to the character which terminated parsing (so you can carry
//on with other things.  abyAddr must be 16 bytes.  You can provide NULL for
//abyAddr, nPort, bIsIPv6, if you are not interested in any of those
//informations.  If we request port, but there is no port part, then nPort will
//be set to 0.  There may be no whitespace leading or internal (though this may
//be used to terminate a successful parse.
//Note:  the binary address and integer port are in network order.
int ParseIPv4OrIPv6 ( const char** ppszText, 
        unsigned char* abyAddr, int* pnPort, int* pbIsIPv6 );
 
 
//simple version if we want don't care about knowing how much we ate
int ParseIPv4OrIPv6_2 ( const char* pszText, 
        unsigned char* abyAddr, int* pnPort, int* pbIsIPv6 );
