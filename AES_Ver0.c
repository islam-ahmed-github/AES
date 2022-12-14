/*
 * AES_Ver0.c
 *
 *  Created on: Mar 16, 2020
 *      Author: Islam Ahmed
 */


#include"AES_Ver0_PRIVATE.h"



static uint8 RotateBuffer(uint8 *Buffer , uint32 BufferLength , uint32 RotationValue , uint8 RotateDirection )
 {
     // logical error check
     if((RotateDirection!='L')&&(RotateDirection!='R')&&(RotateDirection!='l')&&(RotateDirection!='r')) return 0;
     if((BufferLength<RotationValue) ||(BufferLength<=1) )   return 0;
     // useless processing check
     if((RotationValue==BufferLength)||(RotationValue==0))   return 1;

     uint8 temp;
     sint32 i , RotationStep;
     if( RotationValue > (BufferLength/2))
     {
        if (RotateDirection=='L' || RotateDirection=='l' ) RotateDirection='R';
        else RotateDirection='L';
        RotationValue = BufferLength - RotationValue;
     }


      if( RotateDirection == 'R' || RotateDirection == 'r')
      {
         for( RotationStep=0; RotationStep < RotationValue; RotationStep++ )
         {
           temp = Buffer[(BufferLength-1)];
           for(i=(BufferLength-2);i>=0;i--) Buffer[i+1] = Buffer[i];
           Buffer[0] = temp;
        }// end for
        return 1;
      }// end if

    else //if( RotateDirection == 'L' || RotateDirection == 'l')
    {
       for( RotationStep=0; RotationStep<RotationValue; RotationStep++ )
       {
         temp = Buffer[0];
         for(i=0;i<(BufferLength-1);i++) Buffer[i] = Buffer[i + 1];
         Buffer[(BufferLength-1)] = temp;
       }// end for
        return 1;
    } // end if
     return 0;
 }// end function



static void ScheduleCore(uint8 *Buffer, uint32 i)
{

        uint8 a;
        /* Rotate the input 8 bits to the left */
        RotateBuffer(Buffer,4,1,'L');
        /* Apply Rijndael's s-box on all 4 bytes */
        for(a = 0; a < 4; a++) Buffer[a] = sbox[Buffer[a]];
        /* On just the first byte, add 2^i to the byte */
        Buffer[0] ^= Rcon[i];
}


static void ExpandKey( uint8 *KeyBuffer)
{

    uint8  temp[4];
    uint32 KeyByteIndex = 16;  /* c is 16 because the first sub-key is the user-supplied key */
	uint32 i = 1;
    uint32 a ;
    /* We need 11 sets of sixteen bytes each for 128-bit mode */
    while(KeyByteIndex < 176)
    {
       /* Copy the temporary variable over from the last 4-byte block */
       for(a = 0; a < 4; a++)  temp[a] = KeyBuffer[ a + KeyByteIndex - 4 ];

       /* Every four blocks (of four bytes),  do a complex calculation */
       if(KeyByteIndex % 16 == 0) { ScheduleCore(temp,i); i++; }

       for(a = 0; a < 4; a++){ KeyBuffer[KeyByteIndex]=(KeyBuffer[KeyByteIndex-16]^temp[a]);  KeyByteIndex++; }
    }// end while

}// end function



static void AddRoundKey( State* state, const uint8* RoundKey , uint8 round)
{
  uint8 r,c;
  for (r = 0; r < 4; ++r)
  {
    for (c = 0; c < 4; ++c)
    {
      (*state)[r][c] ^= RoundKey[(round * AES_Ncol * 4) + (r * AES_Ncol) + c];
    }
  }
}




static void SubBytes(State *state)
{
  uint8 r, c;
  for (r = 0; r < 4; ++r)
  {
    for (c = 0; c < 4; ++c)
    {
      (*state)[c][r] = getSBoxValue((*state)[c][r]);
    }
  }
}



static void InvSubBytes(State* state)
{
  uint8 i, j;
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      (*state)[j][i] = getSBoxInvert((*state)[j][i]);
    }
  }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows(State *state)
{

    uint8 temp;

  // Rotate first row 1 columns to left
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;

  // Rotate second row 2 columns to left
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp           = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp           = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;

}



static void InvShiftRows(State *state)
{


  uint8 temp;

  // Rotate first row 1 columns to right
  temp = (*state)[3][1];
  (*state)[3][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[0][1];
  (*state)[0][1] = temp;

  // Rotate second row 2 columns to right
  temp = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to right
  temp = (*state)[0][3];
  (*state)[0][3] = (*state)[1][3];
  (*state)[1][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[3][3];
  (*state)[3][3] = temp;
}

static uint8 xtime(uint8 x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}

static void MixColumns(State *state)
{
  uint8 i;
  uint8 Tmp, Tm, t;
  for (i = 0; i < 4; ++i)
  {
    t   = (*state)[i][0];
    Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
    Tm  = (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
    Tm  = (*state)[i][3] ^ t ;              Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
  }
}



#if MULTIPLY_AS_A_FUNCTION
static uint8 Multiply(uint8 x, uint8 y)
{
  return (((y & 1) * x) ^
       ((y>>1 & 1) * xtime(x)) ^
       ((y>>2 & 1) * xtime(xtime(x))) ^
       ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
       ((y>>4 & 1) * xtime(xtime(xtime(xtime(x)))))); /* this last call to xtime() can be omitted */
  }
#else
#define Multiply(x, y)                                \
      (  ((y & 1) * x) ^                              \
      ((y>>1 & 1) * xtime(x)) ^                       \
      ((y>>2 & 1) * xtime(xtime(x))) ^                \
      ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^         \
      ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))   \

#endif


static void InvMixColumns(State* state)
{
  sint32 i;
  uint8 a, b, c, d;
  for (i = 0; i < 4; ++i)
  {
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];

    (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
}


// Cipher is the main function that encrypts the PlainText.
static void Cipher(State* state, const uint8 *RoundKey)
{
  uint8 RoundIndex = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(state, RoundKey , RoundIndex);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without MixColumns()
  for (RoundIndex= 1; ; RoundIndex++)
  {
     SubBytes(state);
     ShiftRows(state);
     if(RoundIndex == AES_Nrounds) break;
     MixColumns(state);
     AddRoundKey(state, RoundKey , RoundIndex);
  }
  // Add round key to last round
 AddRoundKey( state , RoundKey , AES_Nrounds );

}

static void InvCipher(State* state, const uint8* RoundKey)
{
  uint8 RoundIndex = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey( state, RoundKey , AES_Nrounds);

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr rounds are executed in the loop below.
  // Last one without InvMixColumn()
  for (RoundIndex = (AES_Nrounds - 1); ; --RoundIndex)
  {
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state , RoundKey , RoundIndex);
    if(RoundIndex == 0) break;
    InvMixColumns(state);
  }

}





/*************************************************************************************************************/
/************************************************** API PUBLICS **********************************************/





#if defined(ECB) && (ECB == 1)
void AES_ECB_Init( AES* Aes, const uint8* key)
{
   uint32 i ;
  for( i =0 ;i< AES_keyExpandedSize;i++) Aes->RoundKey[i] = key[i];
  ExpandKey(Aes->RoundKey);
}
void AES_ECB_Encrypt(const  AES *Aes, uint8* Buffer)
{
  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher((State*)Buffer, Aes->RoundKey);
}

void AES_ECB_Decrypt(const   AES* Aes, uint8* Buffer)
{
  // The next function call decrypts the PlainText with the Key using AES algorithm.
  InvCipher((State*)Buffer, Aes->RoundKey);
}
#endif // #if defined(ECB) && (ECB == 1)


#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))


void AES_CBC_CTR_Init(   AES* Aes, const uint8* key, const uint8* iv)
{
  uint32 i ;
  for( i =0 ;i< AES_keyExpandedSize;i++) Aes->RoundKey[i] = key[i];
  if(iv!=NULL)
  {
	  for( i =0;i< AES_BLOCK_SIZE; i++) Aes->Iv[i] = iv[i];
  }
  ExpandKey(Aes->RoundKey);
}


void AES_Set_Key( AES* Aes, const uint8* key)
{
  if(key==NULL) return;
  for( uint32 i=0; i< AES_keyExpandedSize;i++) Aes->RoundKey[i] = key[i];
  ExpandKey(Aes->RoundKey);
}


void AES_Set_Iv( AES* Aes, const uint8* iv)
{
	uint32 i ;
	if(iv==NULL) for( i=0; i< AES_BLOCK_SIZE;i++) Aes->Iv[i]= 0;
	else for( i=0; i< AES_BLOCK_SIZE;i++) Aes->Iv[i]= iv[i];
}
#endif


#if defined(CBC) && (CBC == 1)
static void XorWithIv(uint8* Buffer, const uint8* Iv)
{

  uint32 i;
  for ( i = 0; i < AES_BLOCK_SIZE; ++i) Buffer[i] = Buffer[i]^Iv[i];

}

void AES_CBC_Encrypt(  AES* Aes, uint8* Buffer, uint32 BufferLength)
{

  uint32 i;
  uint8 *Iv = Aes->Iv;
  for (i = 0; i < BufferLength; i += AES_BLOCK_SIZE)
  {
    XorWithIv(Buffer, Iv);
    Cipher((State*)Buffer, Aes->RoundKey);
    Iv = Buffer;
    Buffer += AES_BLOCK_SIZE;
  }
  /* store Iv in ctx for next call */
  for( i =0 ;i< AES_BLOCK_SIZE; i++) Aes->Iv[i]= Iv[i];
}

void AES_CBC_Decrypt(  AES* Aes, uint8* Buffer, uint32 BufferLength)
{
  uint32 i =0;
  uint8 Iv[AES_BLOCK_SIZE];
  for (i = 0; i < BufferLength; i += AES_BLOCK_SIZE)
  {
    for( uint32 x =0 ;x< AES_BLOCK_SIZE; x++)  Iv[x]= Buffer[x];
    InvCipher((State*)Buffer, Aes->RoundKey);
    XorWithIv(Buffer, Aes->Iv);
    for( uint32 x =0 ;x< AES_BLOCK_SIZE; x++)  Aes->Iv[x]= Iv[x];
    Buffer += AES_BLOCK_SIZE;
  }
}






#endif // #if defined(CBC) && (CBC == 1)





#if defined(CTR) && (CTR == 1)
/* Symmetrical operation: same function for encrypting as for decrypting. Note any IV/nonce should never be reused with the same key */
void AES_CTR_Xcrypt(  AES* Aes, uint8* Buffer, uint32 BufferLength)
{
  uint8  buffer[AES_BLOCK_SIZE];

  uint32 i  , ii;
  sint32 bi;
  for (i = 0, bi = AES_BLOCK_SIZE; i < BufferLength; ++i, ++bi)
  {
    if (bi == AES_BLOCK_SIZE) /* we need to regen xor compliment in buffer */
    {

       for( ii =0 ;ii< AES_BLOCK_SIZE; ii++)  buffer[ii]= Aes->Iv[ii];
      Cipher((State*)buffer,Aes->RoundKey);

      /* Increment Iv and handle overflow */
      for (bi = (AES_BLOCK_SIZE - 1); bi >= 0; --bi)
      {
	/* inc will overflow */
        if (Aes->Iv[bi] == 255)
	    {
          Aes->Iv[bi] = 0;
          continue;
        }
        Aes->Iv[bi] += 1;
        break;
      }
      bi = 0;
    }

     Buffer[i] ^= buffer[bi];
  }
}

#endif // #if defined(CTR) && (CTR == 1)

