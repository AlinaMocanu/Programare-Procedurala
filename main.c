#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    unsigned char r,g,b;

} pixel;
unsigned int calculeaza_dim(char* nume_fisier_sursa)
{
    unsigned int R,latime_img,inaltime_img;

    FILE *fin= fopen(nume_fisier_sursa, "rb");
    if(fin == NULL)
    {
        printf("nu am gasit imaginea sursa din care citesc");
        exit(1);
    }
    fseek(fin, 18, SEEK_SET);
    fread(&latime_img, sizeof(unsigned int), 1, fin);
    fread(&inaltime_img, sizeof(unsigned int), 1, fin);
    fclose(fin);
    R=latime_img*inaltime_img;
    fclose(fin);
    return R;
}


void  xorshift32(unsigned int seed,unsigned int **p)
{
    unsigned int r,k,R;
    r=seed;
    (*p)[0]=seed;
    R=calculeaza_dim("peppers.bmp");
    for(k=1; k<2*R; k++)
    {
        r=r^r<<13;
        r=r^r>>17;
        r=r^r<<5;
        (*p)[k]=r;

    }

}
unsigned int* generator_numere()
{
    unsigned int R,seed=1000,*p;
    R=calculeaza_dim("peppers.bmp");
    p=(unsigned int)malloc(sizeof(unsigned int)*2*R);
    xorshift32(seed,&p);

    return p;

}
void durstenfeld(unsigned int *p,unsigned int **v)
{
    unsigned int i,j,R=calculeaza_dim("peppers.bmp"),aux;

    *v=(unsigned int*)malloc(sizeof(unsigned int)*R);
    for(i=0; i<R; i++)
        (*v)[i]=i;
    for(i=R-1; i>0; i--)
    {
        j=p[i]%i;
        aux=(*v)[j];
        (*v)[j]=(*v)[i];
        (*v)[i]=aux;
    }

}
unsigned char* incarca_header_in_vector(char *nume_fisier_sursa)
{
    FILE * f=fopen(nume_fisier_sursa,"rb");
    unsigned char *p=(unsigned char*)malloc(sizeof(unsigned char)*54);
    fread(p,sizeof(unsigned char),54,f);
    fclose(f);
    return p;

}
pixel* incarca_pixeli_in_vector(char *nume_fisier_sursa)
{
    FILE *fin= fopen(nume_fisier_sursa,"rb");
    unsigned char c;
    unsigned int R=calculeaza_dim("peppers.bmp");

    if(fin == NULL)
    {
        printf("nu am gasit imaginea sursa din care citesc");
        exit(1);
    }

    int i;
    pixel *v;
    fseek(fin,54,SEEK_SET);
    v=(pixel*)malloc(sizeof(pixel)*R);

    for(i=0; i<R; i++)
    {
        if(fread(&c,sizeof(unsigned char),1,fin)==1)
            v[i].b=c;
        if(fread(&c,sizeof(unsigned char),1,fin)==1)
            v[i].g=c;
        if(fread(&c,sizeof(unsigned char),1,fin)==1)
            v[i].r=c;
    }

    fclose(fin);
    return v;
}
pixel* permuta(char* cale_fisier)
{
    unsigned char *header;
    pixel *v,*vprim;
    unsigned int *p,R,i;
    R=calculeaza_dim("peppers.bmp");
    vprim=(pixel*)malloc(sizeof(pixel)*R);
    header=incarca_header_in_vector("peppers.bmp");
    v=incarca_pixeli_in_vector("peppers.bmp");
    ///permutarea vectorului
    unsigned int *permut;
    p=generator_numere();
    durstenfeld(p,&permut);
    for(i=0; i<R; i++)
        vprim[permut[i]]=v[i];

    ///scrierea in fisier a vectorului permutat
    FILE* f=fopen(cale_fisier,"wb");
    fwrite(header,sizeof(unsigned char),54,f);
    for(i=0; i<R; i++)
    {
        fwrite(&vprim[i].b,sizeof(unsigned char),1,f);
        fwrite(&vprim[i].g,sizeof(unsigned char),1,f);
        fwrite(&vprim[i].r,sizeof(unsigned char),1,f);
    }


    fclose(f);

    return vprim;

}

void xoreaza_vector(char* nume_fisier)
{
    pixel* vprim=permuta("peppers2.bmp"),*criptat;
    unsigned int *p=generator_numere(),i,R,SV=123456789;
    unsigned char *header=incarca_header_in_vector("peppers2.bmp"),*pSV=&SV,*pR;
    R=calculeaza_dim("peppers.bmp");
    criptat=(pixel*)malloc(sizeof(pixel)*R);
    pR=&p[R];
    ///xorarea vectorului permutat
    criptat[0].b=(*pSV)^vprim[0].b^(*pR);
    criptat[0].g=(*(pSV+1))^vprim[0].g^(*(pR+1));
    criptat[0].r=(*(pSV+2))^vprim[0].r^(*(pR+2));
    for(i=1; i<R; i++)
    {
        p=&p[i+R];
        criptat[i].b=criptat[i-1].b^vprim[i].b^(*pR);
        criptat[i].g=criptat[i-1].g^vprim[i].g^(*(pR+1));
        criptat[i].r=criptat[i-1].r^vprim[i].r^(*(pR+2));
    }

    ///scrierea in fisier a vectorului criptat
    FILE* f=fopen(nume_fisier,"wb");
    fwrite(header,sizeof(unsigned char),54,f);
    for(i=0; i<R; i++)
    {
        fwrite(&criptat[i].b,sizeof(unsigned char),1,f);
        fwrite(&criptat[i].g,sizeof(unsigned char),1,f);
        fwrite(&criptat[i].r,sizeof(unsigned char),1,f);
    }
    fclose(f);

}
int main()
{

    xoreaza_vector("peppers2.bmp");






    return 0;
}
