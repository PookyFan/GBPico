#pragma once

#ifdef TEST_MODE
#define mockable virtual
#else
#define mockable
#endif
