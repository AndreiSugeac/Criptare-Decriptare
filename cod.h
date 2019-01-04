#ifndef COD_H_INCLUDED
#define COD_H_INCLUDED

typedef struct
{
    unsigned char R,G,B;
} pixel;

void create_new_img(char *nume,unsigned char *header,pixel *L,long int dim);

void citire_chei(unsigned int *R0,unsigned int *SV);

void calc_W_H(unsigned int *dim,unsigned int *W,unsigned int *H,unsigned char* nume);

int calc_padding(unsigned int W);

pixel *liniarizare(char *nume, unsigned char **header,unsigned int W,unsigned int H,int padding);

unsigned int XORshift32(unsigned int a);

unsigned int *create_rand_sequence(unsigned int R0,unsigned int dim);

unsigned int *create_array(unsigned int dim);

void Durstenfeld(unsigned int **arr,unsigned int *R,unsigned int n);

pixel *permutare(unsigned int *arr,pixel *L,unsigned int dim);

pixel *inv_permutare(unsigned int *arr,pixel *L,unsigned int dim);

pixel XOR_0(unsigned int SV,pixel *P,unsigned int *R,unsigned int k,unsigned int dim);

pixel XOR_k(pixel A,pixel *P,unsigned int *R,unsigned int k,unsigned int dim);

pixel *substitutie(unsigned int *R,pixel *P,unsigned int dim,unsigned int SV);

pixel *inv_substitutie(unsigned int *R,pixel *C,unsigned int dim,unsigned int SV);

void criptare(unsigned char *img,unsigned char *noua_img);

void decriptare(unsigned char *img,unsigned char *decript);

#endif // COD_H_INCLUDED
