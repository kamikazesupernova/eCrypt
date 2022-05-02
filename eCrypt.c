#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#define BUFFER 128


unsigned char key[16];
unsigned char iv[8];
FILE *fps;
FILE *fpt;

int generate_key()
{
    int i;
    FILE *fd;

    if ((fd = fopen("/dev/random", "r")) == NULL){
       fprintf(stderr, "open error!\n");
      }
    if ((fread(key, sizeof(key), 16, fd)) == 0 ){        
        fprintf(stderr,"read key error!\n");
      }
    if ((fread(iv, sizeof(iv), 8, fd)) == 0){
        fprintf(stderr,"read iv error!\n");
      }
    printf("128 bit key:\n");
    for (i = 0; i < 16; i++)
        printf("%4d ", key[i]);

    printf("\nInitialization vector\n");
    for (i = 0; i < 8; i++)
        printf("%4d ", iv[i]);
    printf("\n");


   fclose(fd);
    return 0;
}

int enCryptFile(const char* filename) {

    // const unsigned BUFSIZE=4096;
    // unsigned char *read_buf = malloc(BUFSIZE);
    // unsigned char *cipher_buf;
    // unsigned blocksize;
    int outLen1 = 0;
    int outLen2 = 0;
    EVP_CIPHER_CTX *ctx;
 
    if(!(ctx = EVP_CIPHER_CTX_new())){
                fprintf(stderr, "CTX error!\n");
        return (-1);
    }


    //initialize key 
    if(EVP_EncryptInit(ctx, EVP_aes_128_cbc(), key, iv) !=1){
       fprintf(stderr, "Init error!\n");
       return (-1);
    }; 
    
    //open source file
    fps = fopen(filename, "rb");

    if(fps == NULL){
        fprintf(stderr, "File error!\n");
        return (-1);
    }

    //open destination file
    fpt = fopen("output.txt", "wb");

    if(fpt == NULL){
        fprintf(stderr, "File error!\n");
        return (-1);
    }

        //Get file size
        fseek(fps, 0L, SEEK_END);
        int fsize = ftell(fps);
        //set back to normal
        fseek(fps, 0L, SEEK_SET);

        //allocate memory
        unsigned char *indata = malloc(fsize);
        unsigned char *outdata = malloc(fsize * 2);

        //Read File
        fread(indata,sizeof(char),fsize, fps);

        //do encryption
        if(EVP_EncryptUpdate(ctx, outdata, &outLen1, indata, fsize)!= 1){
                printf("error in encrypt update\n");
            return (-1);
        }    
       if(EVP_EncryptFinal(ctx,outdata,&outLen2)!= 1){
                printf("error in encrypt update\n");
            return (-1);
        }
        //write File
        fwrite(outdata,sizeof(char),fsize,fpt);

        EVP_CIPHER_CTX_free(ctx);
        EVP_cleanup();

        fclose(fps);
        fclose(fpt);

        return 0;

}

int deCryptFile(const char* filename) {

        int outLen1 = 0;
        int outLen2 = 0;
        EVP_CIPHER_CTX *ctx;

	    if(!(ctx = EVP_CIPHER_CTX_new())){
                fprintf(stderr, "CTX error!\n");
        return (-1);
    }
    //initialize key 
      if(EVP_DecryptInit(ctx, EVP_aes_128_cbc(), key, iv)!= 1){
                printf("error in decrypt init\n");
            return (-1);
        }
    //open source file
    fps = fopen("output.txt", "rb");

    if(fps == NULL){
        fprintf(stderr, "File error!\n");
        return (-1);
    }
    //open destination file
    fpt = fopen("mynote2.txt", "wb");

    if(fpt == NULL){
        fprintf(stderr, "File error!\n");
        return (-1);
    }

    //Get file size
    fseek(fps, 0L, SEEK_END);
    int fsize = ftell(fps);
    //set back to normal
    fseek(fps, 0L, SEEK_SET);

    //allocate memory
    unsigned char *indata = malloc(fsize);
    unsigned char *outdata = malloc(fsize);

    //Read File
    fread(indata, sizeof(char), fsize, fps);

    //do decryption

    if(EVP_DecryptUpdate(ctx, outdata, &outLen1, indata, fsize)!= 1){
                fprintf(stderr,"error in decrypt update\n");
            return (-1);
        }

    if(EVP_DecryptFinal(ctx, outdata, &outLen2)!= 1){
                printf("error in decrypt update\n");
            return (-1);
        }
    fwrite(outdata, sizeof(char), fsize, fpt);
    
    EVP_CIPHER_CTX_free(ctx);
    EVP_cleanup();

    fclose(fps);
    fclose(fpt);

    return 0;
}
int main(int argc, char *argv[])
{    


    generate_key();
    encryptFile(argv[1]);

       
     deCryptFile(argv[1]);

  return 0;
}
