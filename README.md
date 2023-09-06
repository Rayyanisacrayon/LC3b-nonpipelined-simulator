# LC3b-nonpipelined-simulator
This is a cycle by cycle simulator of the LC-3b theoretical architecture. This project was made to complete Labs 4 and 5 of UT Austin's Computer Architecture course.

The architecture of this processor lacked key features, such as interrupts, exceptions, and virtual memory. In lab 4, support for interrupts and exceptions was designed by hand and added. In lab 5, support for virtual memory was designed by hand and added. The design changes can be seen below. Microcode is included to facilitate rtl flow in the simulator.

# Changes made for lab 4:

Updated Finite State Machine

![fsm](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/99987fd2-a8b9-4068-9d67-54872a5b8261)

RTI:
- State 8: MAR gets R6, the stack pointer
- State 44: MDR gets the contents of the address in MAR (the old PC)
- State 46: PC gets MDR
- State 26: Stack pointer is incremented and MAR gets new pointer
- State 48: MDR gets contents of MAR (the old PSR)
- 50: PSR is loaded with MDR
- 49: Stack pointer is incremented and PSR[15] is checked for next state.
- 51: PSR[15] is 0, do nothing and return to state 18.
- 59: PSR[15] is 1, context switch and swap the stack pointers. 

Interrupt/exception handler:
- 63:Load vector with appropriate value. Load MDR with old PSR. Update PSR[15] to 0.
- 45:If the old PSR[15] is one, perform a context switch, swapping stack pointers.
- 37:Decrement stack and load it into MAR.
- 36: Store MDR into the location specified by MAR (saving PSR on stack).
- 38: Decrement stack and load it into MAR.
- 34: Decrement PC and load it into MDR.
- 40: Store MDR into the location specified by MAR (saving PC on stack).
- 42: Load MAR with the vector.
- 41: Load MDR with the value of memory specified by MAR.
- 43: Load PC with MDR and go to state 18, beginning ISR/exception routine.

Other added states:
- 47: After state 18, check for exceptions.
- 10/11: Unknown upcode exception, go to exception handler.
- 39,55: Check for exceptions after MAR has been loaded.
- State 18/19 were updated to check for interrupts and load the exception bits. 
- States 2, 3, 6, and 7 have been updated to load the exception bits.
- States 23 and 24 have been updated to also check for exceptions and go to the exception handler if one is detected. 


Updated Datapath

![datapath](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/50172709-f16d-4711-91fa-7f69ec0c3d49)

SR1MUX and DRMUX were updated to be able to selected R6 as the source and destination for stack operations. 

Registers were added to store the supervisor and user pointers during a context switch, as well as hardware to gate them onto the BUS. A mux was added to decrement or increment the stack pointer in r6.

Large View of vector logic

![image](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/ae811bb4-4640-4e50-bd8a-47337c66512a)


One bit registers were added to store bits that determine if an exception occurred. The unknown opcode bit is set by a mux that loads the exception in state 18. The privilege protection bit is set if we are in user mode and the value on the BUS (which will be MAR) is less than 0x3000. The unaligned bit will be set if DATA.SIZE is word and BUS[0] (which will be MAR) is 1.

The exception bits also serve as the select bits for two muxes that determine which vector will be added to the base of the vector table. Once the correct vector is selected, its left shifted by 1 and loaded into th lower 9 bits of the vector register, effectively adding it to 0x0200. A gate was added to load the vector onto the bus.  







Larger view of PSR logic

![image](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/005faf01-2363-48ab-a9cf-e01a6a54aa24)

This logic was added to store and update the PSR. A mux allows the Privelege bit to updated with supervisor mode or to be changed to the value specified by the bus, such as when loading PSR from the stack. PSR[15] is used as a control signal and is connected to the control store. 

A mux was added to allow the nzp bits to be loaded with the values on the bus, such as when loading PSR from the stack, restoring the old CC. A gate was added to BUS the nzp bits when we store PSR on the stack.




New Control Signals:
- Cond2: for support of new conditions to change next state in the fsm
- DRMUX1, SRMUX1: To support setting r6 as destination and source for context switch
- LD.PSR: Allows PSR and CC to be updated.
- PrivMux: Determines whether to set PSR to supervisor privilege or keep it the same.
- GatePSR: Puts PSR on BUS
- GateVector: Puts Vector on BUS
- LD.Vector: Enables vector to be loaded with 0x0200 + lshf(intv/excv, 1)
- LD.Exc: Allows an exception to be detected
- OPMux: Allows Unknown Opcode exception to be set
- LD.SSP, LD.USP: Allows stack pointers to be stored during context switch
- StackMux: Allows stack pointer in r6 to be decremented or incremented upon push/pop.
- GateSSP, GateUSP: puts the supervisor or user stack pointer on the BUS during context switch
- GateStack: Puts stack + 2 or stack - 2 on the BUS.
- GatePC2: Puts PC - 2 on the BUS during context switch
- GateExc: Lets microsequencer check for an exception and switch to state 63.


Updated Microsequencer 
A condition was added for Interrupt to allow state 18 to transition to the interrupt handler if an interrupt is detected.
A condition was added for PSR[15] to determine if a context switch needs to occur during an interrupt/exception/rti.
The Multiplexer was extended and the high select bit determines whether an exception has occurred meaning the next state should enter the exception handler. Specifically, if a Privelege or Unaligned access error has occurred on a state in which we are checking for exceptions, the next state will be state 63. Extending the mux prevents having to define many states for the beginning of the exception handler.






# Changes made for lab 5

Updated Finite State Machine. Changes in blue are from lab 4, those in green are from lab 5.

![fsm2](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/24287331-c08f-4957-9810-d149cfd546fc)
assets/68311528/df718306-7958-4363-821b-b5924c964dc7)

Translation Sequence:
- State 52: The Virtual Address is saved into the VA register. MAR is loaded with PTBR[15:8] concatenated with MAR[15:9] shifted one left.
- 56: The memory of MAR is read into the MDR, loading it with the PTE. The input of the MDR is also used to check for exceptions from the PTE.
- 58: The reference and modify bits of the PTE located in MDR are updated. 
- 60: The PTE is stored back into the page table. 
- 62: MAR gets the physical address by concatenating the PFN from the PTE with the offset bits from VA. Returns to state specified by Jstore bits. 

States added/modified in exception handler:
- 63: The PSR is now loaded into a temporary register instead of the MDR (MDR gets overwritten during translation).
- 37: Now also transitions to the translation sequence, and stores the J bits pointing to state 57. Updates W register to denote write access. 
- 57: New state added to store contents of temp (PSR) into the MDR. 
- 38: Now also transitions to the translation sequence, and stores the J bits pointing to state 34. Updates W register to denote write access.
- 42: Now also transitions to the translation sequence, and stores the J bits pointing to state 41. Updates W register to denote read access.
States added/modified in RTI handler:
- 8: Now also transitions to the translation sequence, and stores the J bits pointing to state 44. Updates W register to denote read access.
- 26: Now also transitions to the translation sequence, and stores the J bits pointing to state 48. Updates W register to denote read access.


States added/modified elsewhere in fsm:
- 47: Now also transitions to the translation sequence, and stores the J bits pointing to state 33. Updates W register to denote read access.
- 15: Now also transitions to the translation sequence, and stores the J bits pointing to state 28. Updates W register to denote read access. Updates T to denote Trap (privileged) access. 
- 30: Updates T to denote end of trap access. 
- 39: Now also transitions to the translation sequence, and stores the J bits pointing to state 29. Updates W register to denote read access.
- 55: Now also transitions to the translation sequence, and stores the J bits pointing to state 25. Updates W register to denote read access.
- 53: New state to store translation metadata (j bits to state 23 and write access) and detect alignment exceptions before address translation. Transitions to translation sequence.
- 54: New state to store translation metadata (j bits to state 24 and write access) and detect alignment exceptions before address translation. Transitions to translation sequence.



Updated Datapath. Once again, new changes are in green, lab 4 changes are in blue. Larger pictures provided below. 

![datapath](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/4149e747-2fcd-4999-9dd9-a3449f7ccd1b)

A PTBR register was added to store the pattern table base, and the high bit can be gated onto the bus. 

Large view of exception logic

![image](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/e06d2873-eb46-4912-962a-27dcaf8a36bd)


The vector logic has been altered to support the specifications of lab 5. 
One bit registers were added to store bits that determine if an exception occurred. The unknown opcode bit is set by a mux that loads the exception in state 18. The privilege protection bit is set if we are in user mode, a trap instruction is not being executed, and the PTE protection bit is a 0, indicating a protected page. The unaligned bit will be set if DATA.SIZE is word and BUS[0] (which will be MAR) is 1. The page fault bit is set if PTE valid bit is a 0. 

The exception bits also serve as the select bits for two muxes that determine which vector will be added to the base of the vector table. Once the correct vector is selected, its left shifted by 1 and loaded into the lower 9 bits of the vector register, effectively adding it to 0x0200. The vector can be gated onto the bus. 


Some logic was added to support virtual memory operations. 

![image](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/696b172a-c4ca-4944-9e67-cbe3adf04ac3)


A temporary register was added to save the value of the MDR before address translation. 
A VA register was added to save the virtual address during translation. The low 9 bits can be gated onto the bus.
The 7 most significant bits of the MAR can also be right shifted by 8 and gated onto the bus in order to calculate the address of the PTE in the same cycle we load VA. 
A signal (signal0) was added that sends bits 2 and 3 of the memory output to the exception logic in order to calculate exceptions using the PTE. 
Additional logic was added for the MDR, which can be seen below. 

MDR Logic

![image](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/90ae3d21-e92f-4c07-b155-cb8aacc85537)

Bits 13-9 of the MDR can be gated onto the bus in order to grab the PFN.
The mdr mux was expanded to support updating the reference and modified bits. The value in MDR is OR’d with 1, setting reference, and then OR’d with lshf(W, 1), setting the modify bit if conduction a write access and leaving it unchanged otherwise. The MDR can be loaded with this new value when the PTEMux signal is asserted. 
A 1 bit W register is added to store whether we are doing a read or write access. This requires asserting the R/W signal in an earlier signal as well, which does not adversely affect the rest of the machine.  



New control signals:
- TR.Enable: Enables the transition to the translation sequence. Stores the J bits in Jstore, updates the value in the W reg, and sets the next state in the microsequencer to state 52. 
- GateJ: Sets the next state in the microsequencer to the one specified by Jstore. 
- PTEMux: Allows MDR to be set with the old value of MDR after the bottom two bits are updated as according to translation. 
- GatePA: Allows the PFN (from the MDR) and the offset bits (bottom 9 bits of VA) to be gated onto the bus together. 
- VA.Enable: Allows VA to be loaded with MAR, gates MAR[15:9] >>8 and PTBR[15:8] onto the bus, allowing the address of the PTE to be calculated. 
- LD.T: Allows the T register to be updated if we are doing a trap instruction. 
- LD.Temp: Allows temp register to be loaded. 
- GateTemp: Allows temp register to be gated onto the bus.
- The OPMux signal was changed to ExcMux, which now serves multiple purposes: This signal can be used to clear or set the exception bits of the various exception registers. It is also used to specify what value is set into the T register. 
- LD.Exc was split into LD.Exc0 and LD.Exc1, allowing align to be set separately from privilege and page fault exceptions. 


Updated micro sequencer. 

![microseq](https://github.com/Rayyanisacrayon/LC3b-nonpipelined-simulator/assets/68311528/a012b372-ce3c-41a0-b559-8907ca1c8fac)

The microsequencer was updated to support Page Fault exceptions by adding the PF signal to the OR gate connected to the select bit of the mux. The mux was also expanded, and Tr.Enable was added as the 2nd select bit. This allows the microsequencer to navigate to state 52 when TR.enable is 1 and no exception is detected.

A second mux was added to allow the microsequencer to use the value of the Jstore reg as the next state. This register is set when Tr.Enable is 1, and is set to the value of the current J bits.  
