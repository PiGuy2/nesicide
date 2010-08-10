#if !defined ( NESCPU_H )
#define NESCPU_H

#include "ctracer.h"
#include "cmarker.h"
#include "ccodedatalogger.h"
#include "cregisterdata.h"
#include "cbreakpointinfo.h"

#include "emulator_core.h"

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   CPU_EVENT_EXECUTE_EXACT = 0,
   CPU_EVENT_UNDOCUMENTED,
   CPU_EVENT_UNDOCUMENTED_EXACT,
   CPU_EVENT_RESET,
   CPU_EVENT_IRQ,
   CPU_EVENT_NMI,
   NUM_CPU_EVENTS
};

// Routine to retrieve the ToolTip information for a particular opcode.
const char* OPCODEINFO ( uint8_t op );

// Routine to retrieve the size of the specified opcode.
uint8_t OPCODESIZE ( uint8_t op );

// The C6502DBG class is a container for all debug elements
// relevant to the NES' 6502 CPU.
class C6502DBG
{
public:
   C6502DBG();
   virtual ~C6502DBG();

   static void GOTO ( uint32_t pcGoto ) { nesSetGotoAddress(pcGoto); }
   static void GOTO () { nesSetGotoAddress(0xFFFFFFFF); }

   // Accessor methods for retrieving information from or about
   // the managed entities within the CPU core object.
   // These routines are used by the debugger, not the emulator core.
   // Return the currently calculated effective address.
   static uint32_t _EA ( void ) { return nesGetCPUEffectiveAddress(); }

   // Return the contents of a memory location visible to the CPU.
   static inline uint8_t _MEM ( uint32_t addr ) { return nesGetCPUMemory(addr); }

   // Modify the contents of a memory location visible to the CPU.
   static inline void _MEM ( uint32_t addr, uint8_t data ) { nesSetCPUMemory(addr,data); }

   // Retrieve a pointer to the whole memory.
   static uint8_t* _MEMPTR ( void ) { return nesGetCPUMemory(); }

   // Return whether or not the CPU is currently in the middle of
   // the first cycle of an instruction fetch (the opcode fetch).
   static bool _SYNC ( void ) { return nesCPUIsFetchingOpcode(); }

   // Return whether or not the CPU is currently in the middle of
   // a write memory cycle.
   static bool _WRITING ( void ) { return nesCPUIsWritingMemory(); }

   // Return the current cycle index of the CPU core.
   // The cycle index is a free-running counter of executed CPU cycles.
   // It will roll-over after approximately 40 minutes of emulation.
   // But, roll-over of this counter is not a significant event.
   static inline uint32_t _CYCLES ( void ) { return nesGetCPUCycle(); }

   // Disassembly routines for display.
   static void DISASSEMBLE ();
   static void DISASSEMBLE ( char** disassembly, uint8_t* binary, int32_t binaryLength, uint8_t* opcodeMask, uint16_t* sloc2addr, uint16_t* addr2sloc, uint32_t* sourceLength );
   static char* Disassemble ( uint8_t* pOpcode, char* buffer );

   // Accessor methods for manipulating CPU core registers.
   // These routines are used by the debugger, not by the
   // emulator core.
   static uint32_t __PC ( void ) { return nesGetCPUProgramCounter(); }
   static void __PC ( uint16_t pc ) { nesSetCPUProgramCounter(pc); }
   static uint32_t _SP ( void ) { return nesGetCPUStackPointer(); }
   static void _SP ( uint8_t sp ) { nesSetCPUStackPointer(sp); }
   static uint32_t _A ( void ) { return nesGetCPUAccumulator(); }
   static void _A ( uint8_t a ) { nesSetCPUAccumulator(a); }
   static uint32_t _X ( void ) { return nesGetCPUIndexX(); }
   static void _X ( uint8_t x ) { nesSetCPUIndexX(x); }
   static uint32_t _Y ( void ) { return nesGetCPUIndexY(); }
   static void _Y ( uint8_t y ) { nesSetCPUIndexY(y); }
   static uint32_t _F ( void ) { return nesGetCPUFlags(); }
   static void _F ( uint8_t f ) { nesSetCPUFlags(f); }
   static uint32_t _N ( void ) { return nesGetCPUFlagNegative(); }
   static uint32_t _V ( void ) { return nesGetCPUFlagOverflow(); }
   static uint32_t _B ( void ) { return nesGetCPUFlagBreak(); }
   static uint32_t _D ( void ) { return nesGetCPUFlagDecimal(); }
   static uint32_t _I ( void ) { return nesGetCPUFlagInterrupt(); }
   static uint32_t _Z ( void ) { return nesGetCPUFlagZero(); }
   static uint32_t _C ( void ) { return nesGetCPUFlagCarry(); }
   static void _N ( uint32_t set ) { nesSetCPUFlagNegative(set); }
   static void _V ( uint32_t set ) { nesSetCPUFlagOverflow(set); }
   static void _B ( uint32_t set ) { nesSetCPUFlagBreak(set); }
   static void _D ( uint32_t set ) { nesSetCPUFlagDecimal(set); }
   static void _I ( uint32_t set ) { nesSetCPUFlagInterrupt(set); }
   static void _Z ( uint32_t set ) { nesSetCPUFlagZero(set); }
   static void _C ( uint32_t set ) { nesSetCPUFlagCarry(set); }

   // The following routines are support for the runtime
   // disassembly of RAM if it is executed by the CPU core.
   // An "opcode mask" is tracked for each byte of accessible
   // RAM.  If a memory location is fetched by the CPU as an
   // opcode (see _SYNC), the memory location is marked
   // as having been executed.  Disassembly of the executed
   // instruction is generated real-time.  If the debuggers
   // need to display disassembly of a memory location they
   // retrieve it from the database generated by the runtime
   // disassembler.
   static inline void OPCODEMASK ( uint32_t addr, uint8_t mask ) { *(m_RAMopcodeMask+addr) = mask; }
   static inline void OPCODEMASKCLR ( void ) { memset(m_RAMopcodeMask,0,sizeof(m_RAMopcodeMask)); }
   static inline char* DISASSEMBLY ( uint32_t addr ) { return *(m_RAMdisassembly+addr); }
   static uint32_t SLOC2ADDR ( uint16_t sloc ) { return *(m_RAMsloc2addr+sloc); }
   static uint16_t ADDR2SLOC ( uint32_t addr ) { return *(m_RAMaddr2sloc+addr); }
   static inline uint16_t SLOC () { return m_RAMsloc; }

   // Interface to retrieve the database defining the registers
   // of the CPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the CPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // CPU registers are declared in the source file.
   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static inline int32_t NUMREGISTERS ( void ) { return m_numRegisters; }

   // Interface to retrieve the database of CPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the CPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  CPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int32_t NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // Interface to retrieve the database of execution markers.
   // Execution markers maintain PPU-cycle/frame counts for the
   // start/finish of marked sections of code.  Whenever the
   // CPU executes an opcode marked as the start or finish of
   // a marked section of code, it updates the database of markers.
   // The Execution Visualizer debugger inspector displays this
   // database visually.
   static CMarker& MARKERS() { return m_marker; }

   // The CPU's Code/Data Logger display is generated by the CPU core
   // because the CPU core maintains all of the information necessary
   // to generate it.
   static inline void CodeDataLoggerInspectorTV ( int8_t* pTV ) { m_pCodeDataLoggerInspectorTV = pTV; }
   static void RENDERCODEDATALOGGER ( void );

   // The Execution Visualizer display is generated by the CPU core
   // because the CPU core maintains all of the information necessary
   // to generate it.
   static inline void ExecutionVisualizerInspectorTV ( int8_t* pTV ) { m_pExecutionVisualizerInspectorTV = pTV; }
   static void RENDEREXECUTIONVISUALIZER ( void );

protected:
   // The data structures that support runtime disassembly of executed code.
   static uint8_t   m_RAMopcodeMask [ MEM_2KB ];
   static char*           m_RAMdisassembly [ MEM_2KB ];
   static uint16_t  m_RAMsloc2addr [ MEM_2KB ];
   static uint16_t  m_RAMaddr2sloc [ MEM_2KB ];
   static uint32_t    m_RAMsloc;

   // This points to the last execution tracer tag that
   // is where the disassembly of the instruction should
   // be placed.
   static TracerInfo* pDisassemblySample;

   // Database used by the Execution Visualizer debugger inspector.
   // The data structure is maintained by the CPU core as it executes
   // instructions that are marked.
   static CMarker         m_marker;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the CPU core as it performs fetches, reads,
   // writes, and DMA transfers to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger m_logger;

   // The database for CPU core registers.  Declaration
   // is in source file.
   static CRegisterData** m_tblRegisters;
   static int32_t             m_numRegisters;

   // The database for CPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;

   // The memory for the Code/Data Logger display.  It is allocated
   // by the debugger inspector and passed to the CPU core for use
   // during emulation.
   static int8_t*          m_pCodeDataLoggerInspectorTV;

   // The memory for the Execution Visualizer display.  It is allocated
   // by the debugger inspector and passed to the CPU core for use
   // during emulation.
   static int8_t*          m_pExecutionVisualizerInspectorTV;
};

// Structure representing each instruction and
// addressing mode possible within the CPU core.
typedef struct _C6502_opcode
{
   // Self-referential index.
   int32_t op;

   // Instruction printable name.
   const char* name;

   // Addressing mode of this particular entry.
   int32_t amode;

   // Number of CPU cycles required to execute this particular entry.
   int32_t cycles;

   // Is the instruction part of the documented 6502 ISA?
   bool documented;

   // Do we force an extra cycle for this instruction variant?
   bool forceExtraCycle;
} C6502_opcode;

#endif
