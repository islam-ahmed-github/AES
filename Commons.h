
/*
 * Commons.h
 *
 *  Created on: Jan 14, 2019
 *      Author: Islam Ahmed
 */


#ifndef COMMONS_H_
#define COMMONS_H_



/******************************************** User Defined ************************************************************************/

//#define F_CPU_Hz      8000000   // define the CPU Clock in MHz
#define WORD_SIZE     32         // define the word size of the Architecture



#define  __STD_TYPES  // comment or uncomment this line to use standard data types
#define  __DELAYS     // comment or uncomment this line to use delays ( Macros & Functions & Inlines )
#define  __BITS       // comment or uncomment this line to use (Bits & Registers) Level ( Macros & Functions & Inlines )

/********************************************End Of User Defined*******************************************************************/
/**********************************************************************************************************************************/
/*********************************************** STD TYPES ************************************************************************/




#ifdef __STD_TYPES

/* Boolean Data Type */
//typedef unsigned char  bool;

/* Boolean Values */
#ifndef FALSE
#define FALSE       (0u)
#endif

#ifndef TRUE
#define TRUE        (1u)
#endif

#ifndef HIGH
#define HIGH        (1u)
#endif

#ifndef HIGH
#define LOW         (0u)
#endif


typedef unsigned char         uint8;          /*           0 .. 255             */
typedef signed char           sint8;           /*        -128 .. +127            */
typedef unsigned short        uint16;         /*           0 .. 65535           */
typedef signed short          sint16;          /*      -32768 .. +32767          */
typedef unsigned long         uint32;         /*           0 .. 4294967295      */
typedef signed long           sint32;          /* -2147483648 .. +2147483647     */
typedef unsigned long long    uint64;         /*       0..18446744073709551615  */
typedef signed long long      sint64;
typedef float                 float32;
typedef double                float64;

#endif

/*****************************************************END OF STD TYPES*************************************************************/
/**********************************************************************************************************************************/
/******************************************** BITS & BYTES & REGISTERS ************************************************************/

#ifdef __BITS

#define DEREFu64(ADDRESS)     (   *((volatile uint64*)(ADDRESS))  )
#define DEREFu32(ADDRESS)     (   *((volatile uint32*)(ADDRESS))  )
#define DEREFu16(ADDRESS)     (   *((volatile uint16*)(ADDRESS))  )
#define DEREFu8(ADDRESS)      (   *((volatile uint8* )(ADDRESS))  )






//#define BIT(x)   (1<<(x))
//#define BIT1(x)  (1<<(x))

//#define BIT0(x)  (~(BIT1((x))))

#define BITS_SetBit(REG,BitIndex)      ((REG)|=(1<< (BitIndex)))
#define BITS_ClearBit(REG,BitIndex)    ((REG)&=(~(1<<(BitIndex))))
#define BITS_ToggleBit(REG,BitIndex)   ((REG)^=(1<<(BitIndex)))
#define BITS_RotR(REG,BitIndex)        ((REG)=((REG)>>(BitIndex))|((REG)<<((WORD_SIZE)-(BitIndex))))
#define BITS_RotL(REG,BitIndex)        ((REG)=((REG)<<(BitIndex))|((REG)>>((WORD_SIZE)-(BitIndex))))
#define BITS_IsSet(REG,BitIndex)       ((REG)&(1<<(BitIndex)))
#define BITS_IsCleared(REG,BitIndex)   (!((REG)&(1<<(BitIndex))))
#define BITS_ReadBit(REG,BitIndex)     ((REG)&(1<<(BitIndex)))



#define BITS_MASK1s(BitsCount,LSBIndex)   ( ((1<<(BitsCount))-1)<<(LSBIndex))
#define BITS_MASK0s(BitsCount,LSBIndex)   (~(((1<<(BitsCount))-1)<<(LSBIndex)))

#define BITS_SetAdjBitS(REG,BitsCount,LSBIndex)             ((REG)|=BITS_MASK1s((BitsCount),(LSBIndex)))
#define BITS_ClearAdjBitS(REG,BitsCount,LSBIndex)           ((REG)&=BITS_MASK0s((BitsCount),(LSBIndex)))

#define BITS_ReadAdjBitS(REG,BitsCount,LSBIndex)            ( ((REG)>>(LSBIndex)) & ((1<<(BitsCount))-1) )

// the following macros must use highlight bits as ones mask such generated from the above macro ----> #define BIT(x)  (1<<x)

#define BITS_SetBitS(REG,ORED_BITs)     ((REG)|=(ORED_BITs))
#define BITS_ClearBitS(REG,ORED_BITs)   ((REG)&=~(ORED_BITs))
#define BITS_ToggleBitS(REG,ORED_BITs)  ((REG)^=(ORED_BITs))



//#define  BITS_WriteAdjBitS(REG,BitsCount,LSBIndex,value);  (REG)&=~(((1<<(BitsCount))-1)<<(LSBIndex));  (REG)|=((value)<<(LSBIndex));

// the following macro uses ___ do{ multiple semicolon code }while(0) ____ for only syntax consistency
#define BITS_WriteAdjBitS(REG,BitsCount,LSBIndex,Value) do{BITS_ClearAdjBitS(REG,BitsCount,LSBIndex);(REG)|=((Value)<<(LSBIndex));}while(0)

#endif

/******************************************* END OF BITS & BYTES & REGISTERS **********************************************************/
/**************************************************************************************************************************************/
/********************************************* INLINES ********************************************************************************/


/*
 inline void BITS_WriteAdjBitS( uint32 REG , uint8 BitsCount,  uint8 LSBIndex, uint32 value) {

	//To write some data at certain bits without affecting the others

	//Clear these bits first
	BITS_ClearAdjBitS((*(volatile uint32*)REG), BitsCount, LSBIndex);  //macro to clear adjacent bits

   *(volatile uint32*)REG|=(value << LSBIndex);// write the data

}


 inline void BITS_WriteAdjBitS_Ptr( volatile uint32* REG , uint8 BitsCount,  uint8 LSBIndex, uint32 value) {

	//To write some data at certain bits without affecting the others

	//Clear these bits first
	BITS_ClearAdjBitS((*(volatile uint32*)REG), BitsCount, LSBIndex);  //macro to clear adjacent bits

   *REG|=(value << LSBIndex);// write the data

}

*/

/**********************************************END OF INLINES *********************************************************************/
/**********************************************************************************************************************************/
/*********************************************** DELAYS ***************************************************************************/
#ifdef __DELAYS

#define F_CPU_KHz      (((uint32)(F_CPU_Hz))  / 1000)  //  KHz
#define F_CPU_MHz      ((F_CPU_KHz) / 1000)            //  MHz

/*
 *
 *   X = loop counter   = ( (    F_CPU / 7    ) *  delay     ) - (  10/7 )
 *   X = loop counter   = ( (      Y(xhz)     ) *  delay(xs) ) - (   Z   )
 *
 *    Yhz  =  ((F_CPU_Hz) /7)
 *    Ykhz =  ((F_CPU_KHz)/7)
 *    Ymhz =  ((F_CPU_MHz)/7)
 *
 *
 *    Z    =  (10/7)
 *
 *   X = loop counter   = ( (      Yhz     )  *  delay(s)  ) - (   Z   )  // for seconds
 *   X = loop counter   = ( (      Ykhz     ) *  delay(ms) ) - (   Z   )  // for ms
 *   X = loop counter   = ( (      Ymhz     ) *  delay(us) ) - (   Z   )  // for us
 *
 * */

#define _Ykhz_  ((F_CPU_KHz)/7)
#define _Ymhz_  ((F_CPU_MHz)/7)
#define _Z_     ((uint8)((10/7)+1))
/*
inline void Delay_ms(const uint32 ms)
{
	for( uint32 i=0; i<( (_Ykhz_ * ms)-(_Z_) ); i++);
}

inline void Delay_us(const uint32 us)
{
	for( uint32 i=0; i<( (_Ymhz_ * us)-(_Z_) ); i++);
}


inline void Vdelay_ms(uint16 ms)
{
uint32 x= ( (_Ykhz_) * (ms) ) - (_Z_);

for( uint32 i=0; i<x; i++);

}

inline void Vdelay_us(uint32 us)
{
	uint32 x= ( (_Ymhz_) * (us) ) - (_Z_);

	for( uint32 i=0; i<x; i++);
}
*/

#endif
/*********************************************** END OF DELAYS ********************************************************************/
/**********************************************************************************************************************************/
/*************************************************Collection*******************************************************************/


#define  ConvertTwoBytesTouint16(HighByte,LowByte)    (((((uint16)HighByte)<<8)|(LowByte)))
#define  SizeOfArray(Array)                           ((sizeof(Array))/(sizeof(Array[0])))

/*
void CommArr_CopyArray(uint8 * Destination , uint8 *Source , uint32 Offset , uint32 Length)
{
  uint32 i=0;
  for( ; i<Length ; i++ ) Destination[i] = Source[Offset+i];
}
*/

/**********************************************************************************************************************************/
/**********************************************************************************************************************************/








#endif /* COMMONS_H_ */
