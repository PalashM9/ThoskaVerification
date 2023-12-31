
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
 
int main()
{
    char* outText;
 
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
    if (api->Init("C:\\CODE\\ocr\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
 
    // Opening input image with leptonica library
    Pix* image = pixRead("C:\\temp\\text1.png");
    api->SetImage(image);
    // Get OCR result
    outText = api->GetUTF8Text();
    printf("OCR output:\n%s", outText);
 
    api->End();
    delete api;
    delete[] outText;
    pixDestroy(&image);
 
    return 0;
}
