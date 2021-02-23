#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// Function prototypes
int nth_bit(int,int);		// Returns the nth bit of a number in binary form
int three_bit(int,int);		// Returns decimal form of 3 bits starting with lsb as nth bit of a number Eg: 111 is returned as 7

/*--------------------------------------------------------------------------------------Defining Data Types---------------------------------------------------------------------------------------------------*/

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

/*----------------------------------------------------------------------------------------End of Data Types---------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------Ico Image Structures--------------------------------------------------------------------------------------------------*/

typedef struct tagICONDIRENTRY
{
       BYTE bWidth;
       BYTE bHeight;
       BYTE bColorCount;
       BYTE bReserved;
       WORD wPlanes;
       WORD wBitCount;
       unsigned int dwBytesInRes;
       unsigned int dwImageOffset;
}ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct tagICONDIR
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?

} ICONDIR, *LPICONDIR;

typedef struct tagBITMAPINFOHEADER
{
unsigned int biSize;  //specifies the number of bytes required by the struct
unsigned int biWidth;  //specifies width in pixels
unsigned int biHeight;  //species height in pixels
WORD biPlanes; //specifies the number of color planes, must be 1
WORD biBitCount; //specifies the number of bit per pixel
 int biCompression;//spcifies the type of compression
 int biSizeImage;  //size of image in bytes
double g1;
double g2; //number of colors that are important
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

typedef struct tagICONIMAGE {

  BITMAPINFOHEADER   icHeader;      // DIB header
   RGBQUAD         icColors[1];   // Color table
   BYTE            icXOR[1];      // DIB bits for XOR mask
   BYTE            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;



/*----------------------------------------------------------------------------------End of Ico Image Structures-----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------Gif Image Structures--------------------------------------------------------------------------------------------------*/

// Structure contains "Header" & "Logical Screen Descriptor"
typedef struct _GifHeader
{
  // Header
  BYTE Signature[3];     /* Header Signature (always "GIF") */
  BYTE Version[3];       /* GIF format version("87a" or "89a") */
  // Logical Screen Descriptor
  WORD ScreenWidth;      /* Width of Display Screen in Pixels */
  WORD ScreenHeight;     /* Height of Display Screen in Pixels */
  BYTE Packed;           /* Screen and Color Map Information */
  BYTE BackgroundColor;  /* Background Color Index */
  BYTE AspectRatio;      /* Pixel Aspect Ratio */
} GIFHEAD;

// Backbone Structure for colour entries of both Global and Local Colour Table
typedef struct _GifColorTable
{
	BYTE Red;          /* Red Color Element       */
	BYTE Green;        /* Green Color Element     */
	BYTE Blue;         /* Blue Color Element      */
} GIFCOLORTABLE;

// Structure for Image Descriptor which contains properties of each the respective image
typedef struct _GifImageDescriptor
{
	BYTE Separator;    /* Image Descriptor identifier */
	WORD Left;         /* X position of image on the display */
	WORD Top;          /* Y position of image on the display */
	WORD Width;        /* Width of the image in pixels */
	WORD Height;       /* Height of the image in pixels */
	BYTE Packed;       /* Image and Color Table Data Information */
} GIFIMGDESC;

// Structure for the Image Data
typedef struct _ImageData
{
	BYTE Compression;
	BYTE Size;
	BYTE Image;
} IMGDATA;

/*----------------------------------------------------------------------------------End of Gif Image Structures-----------------------------------------------------------------------------------------------*/

int main()
{
	int i;
	FILE *fr,*fw;
	fr = fopen("1.ico","rb");
	fw = fopen("final.gif","wb");
	if(fr == NULL || fw == NULL)
	{
		printf("Fail");
		exit(1);
	}

	ICONDIR* x;
	x = (ICONDIR*)malloc( sizeof( ICONDIR ) );

	ICONDIRENTRY* y;
	y = (ICONDIRENTRY*)malloc( sizeof( ICONDIRENTRY) );

	BITMAPINFOHEADER* z;
	z = (BITMAPINFOHEADER*)malloc( sizeof( BITMAPINFOHEADER) );

	GIFHEAD *header;
	header = (GIFHEAD*)malloc(sizeof(GIFHEAD));






	fread(&(x->idReserved), sizeof(WORD),1,fr);
	printf("Reserved : %d\n", x->idReserved);

	fread(&(x->idType), sizeof(WORD),1,fr);
	printf("Id Type: %d\n", x->idType);

	fread(&(x->idCount), sizeof(WORD),1,fr);
	printf("Number of Images : %d\n", x->idCount);


	fread(y,sizeof(ICONDIRENTRY),1,fr);
	printf("Width = %d Height = %d Bits per pixel = %d colour count = %d\n", y->bWidth,y->bHeight,y->wBitCount, y->bColorCount);
	printf("Size of image resource : %d\n",  y->dwBytesInRes);
	printf("Offset to the image : %d\n",  y->dwImageOffset);

	fseek( fr, y->dwImageOffset , SEEK_SET );

	fread(z,sizeof(BITMAPINFOHEADER),1,fr);

	printf("%d %d %d %d\n",z->biBitCount,z->biWidth,z->biHeight,z->biSize );


	header->Signature[0] = 71;
	header->Signature[1] = 73;
	header->Signature[2] = 70;
	printf("\nSignature = %s",header->Signature);
	fwrite(&(header->Signature),sizeof(BYTE),3,fw);

	header->Version[0] = 56;
	header->Version[1] = 55;
	header->Version[2] = 97;
	printf("\nVersion = %s",header->Version);
	fwrite(&(header->Version),sizeof(BYTE),3,fw);

	header->ScreenWidth = y->bWidth;
	printf("\nScreen Width = %d",header->ScreenWidth);
	fwrite(&(header->ScreenWidth),sizeof(WORD),1,fw);

 	header->ScreenHeight = y->bHeight;
	printf("\nScreen Height = %d",header->ScreenHeight);
	fwrite(&(header->ScreenHeight),sizeof(WORD),1,fw);

	header->Packed = 247;
	printf("\nPacked = %d",header->Packed);
	fwrite(&(header->Packed),sizeof(BYTE),1,fw);

	// to calculate size of global color table
	int SizeOfGCT = three_bit(header->Packed,0);
	int NoOfGCTEnt = (1L << (SizeOfGCT + 1));

	// information broken down from "header->Packed"
	printf("\nGlobal Color Table Flag = %d",nth_bit(header->Packed,7));									// indicates the presence of Global Color Table
	printf("\nColor Resolution = %d",three_bit(header->Packed,4));										// used for indicating the bits per color assigned to a pixel
	printf("\nGlobal Color Sort Flag = %d (it is '0' for version 87a)",nth_bit(header->Packed,3));		// indicates whether Global Color Table is to be sorted
	printf("\nNumber of Entries of Global Color Table = %d",NoOfGCTEnt);								// indicates the number of Global Color Entries
	printf("\nNo of Bits for Color Palette = %d",three_bit(header->Packed,4) + 1);						// indicates the bit per colour assigned to a pixel

	header->BackgroundColor = 0;
	printf("\nBackground Color = %d",header->BackgroundColor);
	fwrite(&(header->BackgroundColor),sizeof(BYTE),1,fw);

	header->AspectRatio = 0;
	printf("\nAspect Ratio = %d",header->AspectRatio);
	fwrite(&(header->AspectRatio),sizeof(BYTE),1,fw);



		RGBQUAD arr[256];


		for(i = 0;i < 256;i++)
		{
			fread(&arr[i], sizeof(RGBQUAD),1,fr);

			printf("%d %d %d %d %d\n",i, arr[i].rgbBlue,arr[i].rgbGreen,arr[i].rgbRed,arr[i].rgbReserved);

			fwrite(&(arr[i].rgbRed),sizeof(BYTE),1,fw);
			fwrite(&(arr[i].rgbGreen),sizeof(BYTE),1,fw);
			fwrite(&(arr[i].rgbBlue),sizeof(BYTE),1,fw);

		}

  printf("%ld\n",ftell(fr) );
	int dim = y->bHeight*y->bWidth;
	int j = 0;
	BYTE bits[dim];
	while( j < dim){
	  fread(&bits[j],1,1,fr);
    if (bits[j] == 0){
      bits[j] = bits[0];
    }
    if (bits[j] > 127){
     bits[j] = 32;
    }

	  printf("%d,", bits[j]);
	  j++;
	}
  printf("%ld\n",ftell(fr) );


		GIFIMGDESC *img;
    img = (GIFIMGDESC*)malloc(sizeof(GIFIMGDESC));

    img->Separator = 44;
    img->Left = 0;
    img->Top = 0;
    img->Width = y->bWidth;
    img->Height = y->bHeight;


		printf("\nImage Separator = %d",img->Separator);	// indicates the presence of image or end of file
		fwrite(&(img->Separator),sizeof(BYTE),1,fw);



		printf("\nImage Left Coordinate = %d",img->Left);
		fwrite(&(img->Left),sizeof(WORD),1,fw);


		printf("\nImage Top Coordinate = %d",img->Top);
		fwrite(&(img->Top),sizeof(WORD),1,fw);


		printf("\nImage Width in Pixels = %d",img->Width);
		fwrite(&(img->Width),sizeof(WORD),1,fw);


		printf("\nImage Height = %d",img->Height);
		fwrite(&(img->Height),sizeof(WORD),1,fw);


		img->Packed = 0;
		printf("\nPacked = %d",img->Packed);
		fwrite(&(img->Packed),sizeof(BYTE),1,fw);







    BYTE Comp = 7;
    fwrite(&Comp,1,1,fw);


	      int k = 0;
        BYTE sBlkSiz = y->bWidth + 1;
        BYTE sBlkSizEnd = 0;
        BYTE zero = 0;
        WORD clCode = 128;
        WORD EOICode = 129;

        for (int i = 0; i < y->bHeight; i++ ){
        fwrite(&sBlkSiz,1,1,fw);
        fwrite(&clCode,1,1,fw);
        int q = 0;
        while( q < (y->bWidth)){
          fwrite(&(bits[k]),sizeof(BYTE),1,fw);
          q++;
          k++;
        }
      }



        fwrite(&sBlkSizEnd,1,1,fw);
         fwrite(&EOICode,1,1,fw);
         fwrite(&zero,1,1,fw);
        BYTE ter = 59;
        fwrite(&ter,1,1,fw);




	free(x);
	free(y);
	free(z);
	free(header);
	fclose(fr);
	fclose(fw);


	return 0;
}

// Returns the nth bit of number in binary form
int nth_bit(int a,int n)
{
	return (a>>n & 1);
}

// Returns decimal form of 3 bits starting with lsb as nth bit Eg: 111 is returned as 7
int three_bit(int a,int n)
{
	int num = 0,i;
	for(i=n;i<n+3;i++)
	{
		num += pow(2,i-n) * (a>>i & 1);
	}
	return num;
}
