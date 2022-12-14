


/*
 * AES_Ver0_PUBLIC.h
 *
 *  Created on: Mar 16, 2020
 *      Author: Islam Ahmed
 */


//#include "Commons.h"
#include "Commons.h"
/*************************************************************************************************************/
/******************************************user defined parameters******************************************/

#define AES128 1
//#define AES192 1
//#define AES256 1


#ifndef CBC
  #define CBC 1   // cipher block chaining
#endif

#ifndef ECB
  #define ECB 0  // electronic code book
#endif

#ifndef CTR
  #define CTR 0   // counter
#endif
/************************************End of user defined parameters*****************************************/
/*************************************************************************************************************/


/*************************************************************************************************************/
/******************************************AES defined parameters******************************************/







#if  defined(AES256) && (AES256 == 1)

    #define AES_Nrounds          14
    #define AES_KeyLen           32
    #define AES_keyExpandedSize  240

#elif defined(AES192) && (AES192 == 1)

    #define AES_Nrounds          12
    #define AES_KeyLen           24
    #define AES_keyExpandedSize  208

#else
    #define AES_Nrounds          10
    #define AES_KeyLen           16
    #define AES_keyExpandedSize  176
#endif


#define AES_BLOCK_SIZE 16

/************************************End of AES defined parameters*****************************************/
/*************************************************************************************************************/
/************************************************** API ******************************************************/

typedef struct{
  uint8 RoundKey[AES_keyExpandedSize];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
  uint8 Iv[AES_BLOCK_SIZE];
#endif
} AES;




#if defined(ECB) && (ECB == 1)
void AES_ECB_Init(struct AES*  , const uint8*  );
void AES_ECB_Encrypt(const struct AES*  , uint8*  );
void AES_ECB_Decrypt(const struct AES*  , uint8*  );
#endif // #if defined(ECB) && (ECB == 1)



#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
void AES_CBC_CTR_Init( AES*, const uint8* , const uint8* iv);
void AES_Set_Key( AES* , const uint8* key);
void AES_Set_Iv( AES* , const uint8* );
#endif


#if (defined(CBC) && (CBC == 1))
void AES_CBC_Encrypt( AES* , uint8* , uint32  );
void AES_CBC_Decrypt(  AES* , uint8* , uint32  );
#endif // #if defined(CBC) && (CBC == 1)





#if defined(CTR) && (CTR == 1)
void AES_CTR_Xcrypt( AES* Aes, uint8* Buffer, uint32 BufferLength);
#endif // #if defined(CTR) && (CTR == 1)





