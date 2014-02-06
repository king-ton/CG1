#ifndef CG_IMAGEFILE_H
#define CG_IMAGEFILE_H

//---------------------------------------------------------------------------
// Übung 11 - Aufgabe 1b  |  Methoden hinzugefügt
//---------------------------------------------------------------------------

extern int cgImageWriteTGA(const char *filename, int w, int h, const unsigned char *data, int rle);
extern unsigned char *cgImageLoadTGA(const char *filename, int *w, int *h, unsigned char orientation);

extern int cgImageWriteCG1(const char *filename, const unsigned char *data, int width, int height);
extern unsigned char *cgImageLoadCG1(const char *filename, int *width, int *height);

extern int cgImageWrite(const char *filename, unsigned char *data, int width, int height);
extern unsigned char *cgImageLoad(const char *filename, int *width, int *height);

extern int cgImageConvert(const char *filename_in, const char *filename_out);

#endif
