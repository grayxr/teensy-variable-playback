#include "ResamplingReader.h"

namespace newdigate {
    // Explicitly specialize some instantiations of functions where
    // they need to be different for the supplied type. e.g. it makes
    // no sense to track buffer statuses for an in-memory "file".
    template<>
    void ResamplingReader<short int,File>::resetStatus(void) {}
    template<>
    void ResamplingReader<short int,File>::getStatus(char* buf) { strcpy(buf,"int[]"); }
    template<>
    void ResamplingReader<short int,File>::triggerReload(void) {}
}