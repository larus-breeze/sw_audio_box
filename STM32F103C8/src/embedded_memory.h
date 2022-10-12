/** ***********************************************************************
 * @file	embedded_memory.h
 * @brief	declarations for different flavors of memory
 * @author	Dr. Klaus Schaefer
 **************************************************************************/

#ifndef EMBEDDED_MEMORY_H_
#define EMBEDDED_MEMORY_H_

#define COMMON
#define CONSTEXPR_ROM constexpr __attribute__ ((section (".rodata")))
#ifndef ROM
#define ROM const __attribute__ ((section (".rodata")))
#endif

#endif /* EMBEDDED_MEMORY_H_ */
