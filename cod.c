#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned char R,G,B;
} pixel;

void create_new_img(char *nume,unsigned char *header,pixel *L,long int dim)
{
    FILE *fout;
    fout = fopen(nume,"wb");
    if(fout == NULL)
    {
        printf("Eroare la deschiderea imaginii!!");
        return;
    }
    int i;
    for(i = 0; i < 54; ++i)
    {
        fwrite(&(header[i]),sizeof(unsigned char),1,fout);
    }
    long int j;
    for(j = 0; j < dim; ++j)
    {
        fwrite(&(L[j].B),sizeof(unsigned char),1,fout);
        fwrite(&(L[j].G),sizeof(unsigned char),1,fout);
        fwrite(&(L[j].R),sizeof(unsigned char),1,fout);
    }
    fclose(fout);
}

void citire_chei(unsigned int *R0,unsigned int *SV)
{
    unsigned char *nume = "Chei.txt";
    FILE *fin ;
    fin = fopen(nume,"r");
    if(fin == NULL)
    {
        printf("Eroare la deschiderea fisierului care contine cheile!!!");
        return;
    }
    fscanf(fin,"%u\n",R0);
    fscanf(fin,"%u",SV);
}

void calc_W_H(unsigned int *dim,unsigned int *W,unsigned int *H,unsigned char* nume)
{
    FILE *fin;
    fin = fopen(nume,"rb");
    if(fin == NULL)
    {
        printf("Eroare la deschiderea fisierului!!!");
        return;
    }
    fseek(fin,2,SEEK_SET);
    fread(dim,sizeof(unsigned int),1,fin);
    fseek(fin,18,SEEK_SET);
    fread(W,sizeof(unsigned int),1,fin);
    fread(H,sizeof(unsigned int),1,fin);
    fclose(fin);
}

int calc_padding(unsigned int W)
{
    if(W % 4 != 0)
        return 4 - (3 * W) % 4;
    else return 0;
}

pixel *liniarizare(char *nume, unsigned char **header,unsigned int W,unsigned int H,int padding)
{
    FILE *fin;
    fin = fopen(nume,"rb");
    if(fin == NULL)
    {
        printf("Eroare la deschiderea imaginii!!!");
        return;
    }

    *header = (unsigned char*)calloc(54,sizeof(unsigned char));
    int i;
    for(i = 0; i < 54; ++i)
        fread((&(*((*header) + i))),sizeof(unsigned char),1,fin);

    pixel *L;
    L = (pixel*)calloc((W + padding) * H,sizeof(pixel));
    int poz = 0;
    fseek(fin,54,SEEK_SET);
    while(1)
    {
        if(feof(fin)) break;
        fread(&(L[poz].B),sizeof(unsigned char),1,fin);
        fread(&(L[poz].G),sizeof(unsigned char),1,fin);
        fread(&(L[poz].R),sizeof(unsigned char),1,fin);
        poz++;
    }
    fclose(fin);
    return L;
}

unsigned int XORshift32(unsigned int a)
{
    unsigned int x = a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    a = x;
    return a;
}

unsigned int *create_rand_sequence(unsigned int R0,unsigned int dim)
{
    unsigned int *R;
    R = (unsigned int*)calloc(dim,sizeof(unsigned int));
    int i;
    for(i = 1; i <= dim; ++i)
        if(i == 1) R[i] = XORshift32(R0);
        else R[i] = XORshift32(R[i - 1]);

    return R;
}

unsigned int *create_array(unsigned int dim)
{
    int i;
    unsigned int *arr;
    arr = (unsigned int*)malloc(dim * sizeof(unsigned int));
    for(i = 0; i < dim; ++i)
        arr[i] = i;
    return arr;
}

void Durstenfeld(unsigned int **arr,unsigned int *R,unsigned int n)
{
    int i,k = 1;
    time_t t;
    srand(time(&t));
    for(i = n - 1; i >= 1; --i)
    {
        int j = R[k] % i;
        k++;
        unsigned int aux = *((*arr) + i);
        *((*arr) + i) = *((*arr) + j);
        *((*arr) + j) = aux;
    }
}

pixel *permutare(unsigned int *arr,pixel *L,unsigned int dim)
{
    pixel *P;
    P = (pixel*)calloc(dim,sizeof(pixel));
    int i;
    for(i = 0; i < dim; ++i)
    {
        P[i] = L[arr[i]];
    }
    return P;
}

pixel *inv_permutare(unsigned int *arr,pixel *L,unsigned int dim)
{
    pixel *P;
    P = (pixel*)calloc(dim,sizeof(pixel));
    int i;
    for(i = 0; i < dim; ++i)
    {
        P[arr[i]] = L[i];
    }
    return P;
}

pixel XOR_0(unsigned int SV,pixel *P,unsigned int *R,unsigned int k,unsigned int dim)
{
    int i;
    pixel C1;
    for(i = 0; i < sizeof(pixel); ++i)
    {
        unsigned char byte = *((unsigned char*)&SV + i);
        unsigned char byte1 = *((unsigned char*)&R[dim + k] + i);
        if(i == 0)
            C1.B = byte ^ P[k].B ^ byte1;
        else if(i == 1)
            C1.G = byte ^ P[k].G ^ byte1;
        else if(i == 2)
            C1.R = byte ^ P[k].R ^ byte1;
    }
    return C1;
}

pixel XOR_k(pixel A,pixel *P,unsigned int *R,unsigned int k,unsigned int dim)
{
    int i;
    pixel C1;
    for(i = 0; i < sizeof(pixel); ++i)
    {
        unsigned char byte1 = *((unsigned char*)&R[dim + k] + i);
        if(i == 0)
            C1.B = A.B ^ P[k].B ^ byte1;
        else if(i == 1)
            C1.G = A.G ^ P[k].G ^ byte1;
        else if(i == 2)
            C1.R = A.R ^ P[k].R ^ byte1;
    }
    return C1;
}

pixel *substitutie(unsigned int *R,pixel *P,unsigned int dim,unsigned int SV)
{
    int k;
    pixel *C;
    C = (pixel*)calloc(dim,sizeof(pixel));
    for(k = 0; k < dim; ++k)
    {
        if(k == 0)
        {
            C[k] = XOR_0(SV,P,R,k,dim);
        }
        else
            C[k] = XOR_k(C[k - 1],P,R,k,dim);
    }
    return C;
}

pixel *inv_substitutie(unsigned int *R,pixel *C,unsigned int dim,unsigned int SV)
{
    int k;
    pixel *Cp;
    Cp = (pixel*)calloc(dim,sizeof(pixel));
    for(k = 0; k < dim; ++k)
    {
        if(k == 0)
        {
            Cp[k] = XOR_0(SV,C,R,k,dim);
        }
        else
            Cp[k] = XOR_k(C[k - 1],C,R,k,dim);
    }
    return Cp;
}

//Functia in care initializam variabilele si in care apelam functiile necesare criptarii
void criptare(unsigned char *img,unsigned char *noua_img)
{
    //folosim variabilele  W, H si dim pentru a stoca dimensiunile imaginii
    unsigned int W,H,dim;
    calc_W_H(&dim,&W,&H,img);
    int padding;
    padding = calc_padding(W);
    //L reprezinta vectorul de pixeli in care vom salva imaginea liniarizata
    pixel *L;
    //in header retinem primii 54 de octeti ai imaginii (header-ul imaginii)
    unsigned char *header;
    L = liniarizare(img,&header,W,H,padding);
    unsigned int *R;
    unsigned int R0,SV;
    //citim cheile secrete
    citire_chei(&R0,&SV);
    //creem secventa de numere aleatoare folosind R0 ca seed value
    R = create_rand_sequence(R0,2 * (W + padding) * H - 1);
    unsigned int *arr;
    //arr reprezinta vectorul pe care il utilizam pentru a reprezenta permutarea
    arr = create_array(W * H);
    //creem permutarea utilizand algoritmul Durstenfeld
    Durstenfeld(&arr,R,(W + padding) * H);
    pixel *P;
    //aplicam permutarea asupra imaginii liniarizate
    P = permutare(arr,L,(W + padding) * H);
    free(L);
    free(arr);
    pixel *C;
    //aplicam relatia de substitutie asupra fiecarui pixel permutat din imagine
    C = substitutie(R,P,(W + padding) * H,SV);
    free(R);
    free(P);
    //creem o noua imagine cu pixelii obtinuti dupa substitutie
    create_new_img(noua_img,header,C,(W + padding) * H);
    free(header);
}

//Functia de baza pentru algoritmul de decriptare
void decriptare(unsigned char *img,unsigned char *decript)
{
    unsigned int dim,W,H;
    calc_W_H(&dim,&W,&H,img);
    pixel *C;
    unsigned int *header;
    int padding = calc_padding(W);
    //salvam imaginea criptata in forma liniarizata
    C = liniarizare(img,&header,W,H,padding);
    unsigned int R0,SV;
    //citim cheile secrete
    citire_chei(&R0,&SV);
    unsigned int *R;
    //creem secventa de numere aleatoare
    R = create_rand_sequence(R0,2 * (W + padding) * H);
    unsigned int *vect;
    vect = create_array(W * H);
    Durstenfeld(&vect,R,(W + padding) * H);
    pixel *C_p;
    //aplicam inversa substitutie asupra fiecarui pixel din imaginea criptata
    C_p = inv_substitutie(R,C,(W + padding) * H,SV);
    free(C);
    free(R);
    pixel *D;
    //permutam invers fiecare pixel substituit pentru a obtine pixelii imaginii originale
    D = inv_permutare(vect,C_p,(W + padding) * H);
    free(C_p);
    free(vect);
    //se creaza imaginea originala
    create_new_img(decript,header,D,(W + padding) * H);
    free(header);
}
