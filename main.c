#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// Function signatures
void read_txt();
void readInputImage(char *filename);
void bintensity();
int get_bintensity(unsigned char *pbn, char *filename);


// Images container
struct images_container {
    // Images width
    int width;  
    // Images height
    int height; 
    // Input RGB image
    unsigned char *prgb; 
    // Output sketches image
    unsigned char *pbn; 
};

// Parameters container
struct parameters_container {
    int width; 
    int height; 
    // Input path
    char iPath[256];
    // Output path 
    char oPath[256]; 
};

// Se define un imageP para las funciones que requiera tratar imagenes
struct imageP {
    char bfType[3];       // "BM"
    int bfSize;           // Size of file in bytes 
    int bfReserved;       // set to 0 
    int bfOffBits;        // Byte offset to actual bitmap data (= 54) 
    int biSize;           // Size of BITMAPINFOHEADER, in bytes (= 40)
    int biWidth;          // Width of image, in pixels 
    int biHeight;         // Height of images, in pixels 
    short biPlanes;       // Number of planes in target device (set to 1) 
    short biBitCount;     // Bits per pixel (24 in this case) 
    int biCompression;    // Type of compression (0 if no compression)
    int biSizeImage;      // Image size, in bytes (0 if no compression) 
    int biXPelsPerMeter;  // Resolution in pixels/meter of display device 
    int biYPelsPerMeter;  // Resolution in pixels/meter of display device 
    int biClrUsed;        // Number of colors in the color table (if 0, use maximum allowed by biBitCount
    int biClrImportant;   // Number of important colors.  If 0, all colors are important 
};

// Este puntero global apunta al contenedor que se usa para guardar los valores de control de flujo del programa que se leen de un archivo de texto
struct parameters_container *p_parametros;

// Este puntero global apunta al contenedor que se usa para guardar las imagenes
struct images_container *p_images;

int main() {
    
    // General counter
    int i; 
    int width, height;

    // Se reserva memoria para parameters_container 
    p_parametros = (struct parameters_container *)malloc(sizeof(struct parameters_container));

    // Se lee el txt
    read_txt();

    // Se reserva memoria para images_container
    p_images = (struct images_container *)malloc(sizeof(struct images_container));

    p_images->width = p_parametros->width;
    p_images->height = p_parametros->height;
    width=p_parametros->width;
    height=p_parametros->height;

    // Se inicializa la memoria de las imagenes del images container
    p_images->prgb=(unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
    p_images->pbn=(unsigned char*)malloc(sizeof(unsigned char)*width*height);

    // Se inicializa la posicion de cada pixel en cero
    for (i=0; i<width*height*3; i++) {
        p_images->prgb[i]=0;
    }
    for (i=0; i<width*height; i++) {
        p_images->pbn[i]=0;
    }

    // Se lee la rgb
    readInputImage(p_parametros->iPath);

    // Se define el caracter InputImageName con su respectiva longitud del string
    char InputImageName[256];
 
    // Se llama a crear la binaria de intensidad 
    bintensity();

    // Se copia el contendio de la salida en formato bmp
    strcpy(InputImageName,"output/bintensity.bmp");

    // Se almacena la salida en una imagen bmp
    get_bintensity(p_images->pbn, InputImageName);

    // Se libera memoria responsablemente
    free(p_images->prgb);
    free(p_images->pbn);
    free(p_images);
    free(p_parametros);

    return 0;
}

//Esta funcion lee imagen RGB de archivo en formato BMP (lee la input)
void readInputImage(char *filename) {
    FILE *fd;
    int width, height;
    int i, j;
    int n;

    fd = fopen(filename, "rb");
    if (fd == NULL) {
        printf("Error: fopen failed\n");
        return;
    }

    unsigned char header[54];

    // Read header
    n=fread(header, sizeof(unsigned char), 54, fd);
    if (n==0) {printf("No se pudieron leer datos\n"); exit(0);}

    // Capture dimensions
    width = *(int*)&header[18];
    height = *(int*)&header[22];

    int padding = 0;

    // Calculate padding
    while ((width * 3 + padding) % 4 != 0)
    {
        padding++;
    }

    // Compute new width, which includes padding
    int widthnew = width * 3 + padding;

    // Allocate temporary memory to read widthnew size of data
    unsigned char* data = (unsigned char *)malloc(widthnew * sizeof (unsigned int));

    // Read row by row of data and remove padded data.
    for (i = 0; i<height; i++) {
        // Read widthnew length of data
        n=fread(data, sizeof(unsigned char), widthnew, fd);
        if (n==0) {printf("No se pudieron leer datos\n"); exit(0);}

        // Retain width length of data, and swizzle RB component.
        // BMP stores in BGR format, my usecase needs RGB format
        for (j = 0; j < width * 3; j += 3) {
            int index = (i * width * 3) + (j);
            p_images->prgb[index + 0] = data[j + 2];
            p_images->prgb[index + 1] = data[j + 1];
            p_images->prgb[index + 2] = data[j + 0];
        }
    }

    free(data);
    fclose(fd);
}

// La siguiente función obtiene la binaria de intensidad en formato bmp con la salida que le da bintensity
int get_bintensity(unsigned char *pbn, char *filename) {
    int i, j, jj, ipos;
    int bytesPerLine;
    unsigned char *line;
    unsigned char *ptempImage;
    int height, width;

    height=p_images->height;
    width=p_images->width;

    FILE *file;
    struct imageP bmph;

    // El tamaño de cada línea debe ser un multiplo de 4 bytes
    bytesPerLine = (3 * (width + 1) / 4) * 4;

    strcpy(bmph.bfType, "BM");
    bmph.bfOffBits = 54;
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = 24;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;
    bmph.biClrImportant = 0;

    file = fopen (filename, "wb");
    if (file == NULL) return(0);

    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);

    line = (unsigned char*) malloc(bytesPerLine);

    if (line == NULL) {
        fprintf(stderr, "Can't allocate memory for BMP file.\n");
        return(0);
    }

    // Se cambia la posicion del sistema de coordenadas de la equina inferior izquierda a la esquina superior izquierda.
    ptempImage = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
    for (i=0;i<width*height;i++) ptempImage[i]=0;
    jj=0;
    for (j=height-1;j>=0;j--) {
        for (i=0;i<width;i++) {
            ptempImage[jj*width+i]= pbn[j*width+i];
        }
        jj++;
    }

    for (i = height - 1; i >= 0; i--) {
        for (j = 0; j < width; j++) {

            ipos = (width * i + j);
            line[3*j] = ptempImage[ipos];
            line[3*j+1] = ptempImage[ipos];
            line[3*j+2] = ptempImage[ipos];
        }
        fwrite(line, bytesPerLine, 1, file);
    }

    free(line);
    fclose(file);
    free(ptempImage);

    return(1);
}

// Esta funcion obtiene la binaria de intensidad de una cualquier imagen BMP
void bintensity()
{
    int i,j;
    int width, height;

    // Se renombra por comodidad
    width = p_images->width;
    height = p_images->height;

    // Se definen umbrales
    double umbrall_min = 120;
    double umbrall_max = 145;

    // Se calcula la binaria. El resultado se almacena en el espacio que fue alocado en la estructura
    for (j=0;j<height; j++) {
        for (i=0;i<width; i++) {
            if(p_images->prgb[3*j*width+3*i] >= umbrall_min && p_images->prgb[3*j*width+3*i] <= umbrall_max){
                p_images->pbn[j*width+i] = (unsigned char)(0);
            }
            else {
                p_images->pbn[j*width+i] = (unsigned char)(255);
            }
            }
        }
    }
 
// Esta funcion lee los parametros de archivo de parametros de control
void read_txt() {
    FILE *archivo;
    char d1[256], d2[256], d3[256];
    int res;

    //Abriendo archivo en mode de lectura
    char nombreDeArchivo[256]="parameters.txt";
    archivo = fopen(nombreDeArchivo, "r");
    if (!archivo) {
        printf("No se pudo abrir el archivo: %s\n",nombreDeArchivo);
        exit(1);
    }

    printf("\n");
    printf("   Tamaño de la imagen (px):\n");

    //Reading with
    res=fscanf(archivo, "%s %s\n", d1, d2);
    p_parametros->width=(int)atof(d2);
    printf("      Width:  %d\n", p_parametros->width);

    //Reading height
    res=fscanf(archivo, "%s %s\n", d1, d2);
    p_parametros->height=(int)atof(d2);
    printf("      Height: %d\n", p_parametros->height);
    printf("\n");

    //Reading input file
    res=fscanf(archivo, "%s %s %s\n", d1, d2, d3);
    strcpy(p_parametros->iPath,d3);
    printf("   Directorio imagen de entrada: %s\n", p_parametros->iPath);

    //Reading output file
    res=fscanf(archivo, "%s %s %s\n", d1, d2, d3);
    strcpy(p_parametros->oPath,d3);
    printf("   Directorio imagen procesada:  %s\n", p_parametros->oPath);
    printf("\n");

    fclose(archivo);
}