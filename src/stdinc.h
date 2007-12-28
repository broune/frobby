#include <gmp.h>
#include <gmpxx.h>
using namespace std;

//#define LOG
//#define PROFILE

#ifdef PROFILE
static void _noInline() {cerr << "this should not be printed - inlining prevention has gone awry" << endl;}
int _noInlineVariable = time(0); // so the compiler will not know what the value is
// place enough code into the function so that it will not be inlined.
#define PROFILE_NO_INLINE {if (_noInlineVariable == -1){\
_noInline();_noInline();_noInline();_noInline();_noInline();\
_noInline();_noInline();_noInline();_noInline();_noInline();\
_noInline();_noInline();_noInline();_noInline();_noInline();\
_noInline();_noInline();_noInline();_noInline();_noInline();\
_noInline();_noInline();_noInline();_noInline();_noInline();\
_noInline();_noInline();_noInline();_noInline();_noInline();}}
#else
#define PROFILE_NO_INLINE
#endif

#ifdef DEBUG
#define PRINT
#define IF_DEBUG(X) X
#include <cassert>
#define ASSERT(X) assert(X);
#else
#define IF_DEBUG(X)
#define ASSERT(X)
#endif

typedef unsigned int Exponent;
typedef mpz_class Degree;

namespace constants {
  extern const char* version;
}
